import asyncio

from bleak import BleakClient, BleakScanner


DEVICE_NAME = "Microbit-RPI"
NUS_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
NUS_RX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
NUS_TX_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"


def on_notification(_characteristic, data):
    message = data.decode("utf-8", errors="replace").rstrip()
    print(f"micro:bit> {message}", flush=True)


async def main():
    print(f"Scanning for {DEVICE_NAME} BLE UART service...")
    device = await BleakScanner.find_device_by_filter(
        lambda _device, advertisement: NUS_SERVICE_UUID in advertisement.service_uuids,
        timeout=20.0,
    )
    if device is None:
        raise RuntimeError("Could not find the micro:bit BLE UART service. Check that Zephyr BLE firmware is advertising.")

    async with BleakClient(device) as client:
        print(f"Connected to {device.name}")
        await client.start_notify(NUS_TX_UUID, on_notification)
        await client.write_gatt_char(NUS_RX_UUID, b"PING", response=False)
        print("Sent PING; press buttons A or B to see BUTTONS messages. Ctrl+C to quit.")

        while True:
            await asyncio.sleep(1)


if __name__ == "__main__":
    asyncio.run(main())