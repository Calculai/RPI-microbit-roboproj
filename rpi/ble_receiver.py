import asyncio

from bleak import BleakClient, BleakScanner
from bleak.exc import BleakError

# a receiver that handles NUS (Nordic UART Service) messages over BLE for the Microbit-Controller
# made to understand how to communicate with the Microbit-Controller over BLE

DEVICE_NAME = "Microbit-Controller"
NUS_SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e" 
NUS_RX_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
NUS_TX_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

SCAN_TIMEOUT_SECONDS = 10.0
RETRY_DELAY_SECONDS = 2.0


def on_notification(_characteristic, data):
    # decode the incoming BLE message, replacing any errors with a placeholder character and stripping any trailing whitespace
    message = data.decode("utf-8", errors="replace").rstrip() 
    print(f"Controller> {message}", flush=True)


async def find_microbit():
    # scan for BLE devices advertising NUS (Nordic UART Service) and return the first one found 
    # doesnt use additional identification so could conflict if more devices utilizing the same service are nearby
    return await BleakScanner.find_device_by_filter(
        lambda _device, advertisement: NUS_SERVICE_UUID in advertisement.service_uuids,
        timeout=SCAN_TIMEOUT_SECONDS,
    )


async def run_session(device):
    
    disconnected = asyncio.Event() 

    def on_disconnect(_client):
        # format callback for handling disconnection events 
        disconnected.set()

    # BleakClient connects on entry and disconnects automatically when this block exits. 
    async with BleakClient(device, disconnected_callback=on_disconnect) as client:

        print(f"Connected to {device.name or device.address}", flush=True)

        # start listening for notifications from the microbit using the NUS TX characteristic
        await client.start_notify(NUS_TX_UUID, on_notification) 

        # send ping to the microbit for purpose of troubleshooting
        await client.write_gatt_char(NUS_RX_UUID, b"PING", response=False)
       
        print("Sent PING; press buttons A or B to see BUTTONS messages. Ctrl+C to quit.", flush=True)

        # listens for disconnection events
        await disconnected.wait()

    print("Disconnected from microbit", flush=True)


async def main():
    while True:
        print(f"Scanning for {DEVICE_NAME} BLE UART service...", flush=True)
        try:
            device = await find_microbit()
            
            # if no device was found, continue scanning
            if device is None: 
                print("Not found yet; still looking.", flush=True)
                continue
            await run_session(device)

        except (BleakError, asyncio.TimeoutError, OSError) as error:
            print(f"BLE error: {error}", flush=True)

        # in case of a disconnect or caught error, wait before retrying
        await asyncio.sleep(RETRY_DELAY_SECONDS)


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass