# RPI-microbit-roboproj
A project where I develop a controllable drone

## PlatformIO

The current PlatformIO environment targets the BBC micro:bit V2 (`bbcmicrobit_v2`).
The prototype displays a heart on the micro:bit V2 5x5 LED matrix.
PlatformIO builds source files from `microbit/` only. Add micro:bit `.cpp` files
there; Raspberry Pi source files can live in a separate folder such as `rpi/`
without being included in this firmware build. These micro:bit files are linked
into one program, so only one file should define `setup()` and `loop()`.

On Windows, add `%USERPROFILE%\.platformio\penv\Scripts` to your user `Path`
environment variable so the `pio` command is available in the terminal. 

Build the firmware from the project directory:

```sh
pio run
```

Connect the micro:bit V2 over USB and upload it with:

```sh
pio run --target upload
```

The generated firmware is in `.pio/build/microbit_v2/`. The current configuration
uses the `mbed` upload protocol and the detected `D:` drive; update `upload_port`
in `platformio.ini` if Windows assigns the micro:bit a different drive letter.
A micro:bit V1 needs a different target and is not covered by this configuration.

