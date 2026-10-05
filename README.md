# RPI-microbit-roboproj
A project where I develop a controllable drone

## PlatformIO

Both PlatformIO environments target the BBC micro:bit V2 (`bbcmicrobit_v2`):

- `microbit_arduino` is the default Arduino environment. It builds source files
	from `microbit/`; Raspberry Pi files in `rpi/` are excluded. Only one file in
	`microbit/` should define `setup()` and `loop()`.
- `microbit_zephyr_ble` builds the Zephyr BLE firmware. It compiles `.c` and
	`.cpp` application files from `ble/`; its Zephyr configuration is in `zephyr/`.

On Windows, add `%USERPROFILE%\.platformio\penv\Scripts` to your user `Path`
environment variable so the `pio` command is available in the terminal. 

Build the default Arduino firmware from the project directory:

```sh
pio run
```

Build the Zephyr BLE firmware instead:

```sh
pio run -e microbit_zephyr_ble
```

Connect the micro:bit V2 over USB and upload it with:

```sh
pio run -e microbit_arduino --target upload
```

The Arduino build output is in `.pio/build/microbit_arduino/`. Its upload
configuration uses the `mbed` protocol and the detected `D:` drive; update
`upload_port` in `platformio.ini` if Windows assigns a different drive letter.
A micro:bit V1 needs a different target and is not covered by these environments.

