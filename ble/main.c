#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/nus.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LENGTH (sizeof(DEVICE_NAME) - 1)

static const struct gpio_dt_spec button_a = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec button_b = GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios);

static bool connected;
static bool notifications_enabled;

static const struct bt_data advertising_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_SRV_VAL),
};

static const struct bt_data scan_response[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LENGTH),
};

static struct bt_nus_cb nus_callbacks = {
    .notif_enabled = notification_changed,
    .received = data_received,
};

BT_CONN_CB_DEFINE(connection_callbacks) = {
    .connected = connected_callback,
    .disconnected = disconnected_callback,
    .recycled = recycled_callback,
};

static void notification_changed(bool enabled, void *context)
{
    ARG_UNUSED(context);
    notifications_enabled = enabled;
    printk("Pi notifications %s\n", enabled ? "enabled" : "disabled");
}

static void data_received(struct bt_conn *connection, const void *data,
                          uint16_t length, void *context)
{
    ARG_UNUSED(context);

    if (length == 4 && memcmp(data, "PING", 4) == 0) {
        static const char response[] = "PONG\n";
        bt_nus_send(connection, response, sizeof(response) - 1);
        printk("Received PING from Pi\n");
    } else {
        static const char response[] = "ERR,UNKNOWN\n";
        bt_nus_send(connection, response, sizeof(response) - 1);
        printk("Received an unrecognized BLE message\n");
    }
}

static void connected_callback(struct bt_conn *connection, uint8_t error)
{
    ARG_UNUSED(connection);

    if (error != 0) {
        printk("BLE connection failed (error %u)\n", error);
        return;
    }

    connected = true;
    printk("Pi connected over BLE\n");
}

static void disconnected_callback(struct bt_conn *connection, uint8_t reason)
{
    ARG_UNUSED(connection);
    ARG_UNUSED(reason);

    connected = false;
    notifications_enabled = false;
    printk("Pi disconnected; waiting for Zephyr to release the connection\n");
}

static void recycled_callback(void)
{
    printk("Connection released; restarting advertising\n");
    int error = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, advertising_data,
                                ARRAY_SIZE(advertising_data), scan_response,
                                ARRAY_SIZE(scan_response));
    if (error != 0 && error != -EALREADY) {
        printk("Restart advertising failed (error %d)\n", error);
    }
}

static bool read_button(const struct gpio_dt_spec *button, int *value)
{
    int result = gpio_pin_get_dt(button);
    if (result < 0) {
        return false;
    }

    *value = result;
    return true;
}

int main(void)
{
    int error = gpio_pin_configure_dt(&button_a, GPIO_INPUT);
    if (error != 0) {
        printk("Could not configure button A (error %d)\n", error);
        return error;
    }

    error = gpio_pin_configure_dt(&button_b, GPIO_INPUT);
    if (error != 0) {
        printk("Could not configure button B (error %d)\n", error);
        return error;
    }

    error = bt_nus_cb_register(&nus_callbacks, NULL);
    if (error != 0) {
        printk("Could not register BLE UART callbacks (error %d)\n", error);
        return error;
    }

    error = bt_enable(NULL);
    if (error != 0) {
        printk("Bluetooth initialization failed (error %d)\n", error);
        return error;
    }

    error = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, advertising_data,
                            ARRAY_SIZE(advertising_data), scan_response,
                            ARRAY_SIZE(scan_response));
    if (error != 0) {
        printk("BLE advertising failed (error %d)\n", error);
        return error;
    }

    printk("Advertising as %s; waiting for Raspberry Pi\n", DEVICE_NAME);

    int candidate_a = -1;
    int candidate_b = -1;
    int stable_a = -1;
    int stable_b = -1;
    int sent_a = -1;
    int sent_b = -1;
    uint32_t candidate_a_since = 0;
    uint32_t candidate_b_since = 0;
    bool first_sample = true;

    while (true) {
        int value_a;
        int value_b;
        uint32_t now = k_uptime_get_32();

        if (!read_button(&button_a, &value_a) || !read_button(&button_b, &value_b)) {
            k_msleep(20);
            continue;
        }

        if (first_sample) {
            candidate_a = stable_a = value_a;
            candidate_b = stable_b = value_b;
            candidate_a_since = now;
            candidate_b_since = now;
            first_sample = false;
        }

        if (value_a != candidate_a) {
            candidate_a = value_a;
            candidate_a_since = now;
        } else if (candidate_a != stable_a && now - candidate_a_since >= 50) {
            stable_a = candidate_a;
        }

        if (value_b != candidate_b) {
            candidate_b = value_b;
            candidate_b_since = now;
        } else if (candidate_b != stable_b && now - candidate_b_since >= 50) {
            stable_b = candidate_b;
        }

        if (connected && notifications_enabled && stable_a >= 0 && stable_b >= 0 &&
            (stable_a != sent_a || stable_b != sent_b)) {
            char message[24];
            int length = snprintk(message, sizeof(message), "BUTTONS,%d,%d\n",
                                  stable_a, stable_b);
            error = bt_nus_send(NULL, message, length);
            if (error == 0) {
                sent_a = stable_a;
                sent_b = stable_b;
            }
        }

        k_msleep(20);
    }

    return 0;
}