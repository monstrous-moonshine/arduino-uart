# arduino-uart
A minimal uart echo example with the arduino.

## Build
To build and flash this example, you will need the `avr-gcc` toolchain and the `avrdude` tool. You'll also need a serial
terminal program to interact with the arduino. On Arch, these could be installed by:
```console
$ sudo pacman -S avr-gcc avr-libc avrdude picocom
```

Now, assuming your arduino is connected to your PC and is available at `/dev/ttyACM0`, run:
```console
$ make flash
```

If you get permission errors on `/dev/ttyACM0`, you have two options:
1. Run as root (use `sudo` or something), or
2. Add yourself to the `uucp` group

You can add yourself to the `uucp` group with the following command:
```console
$ sudo usermod -aG uucp <your-username>
```
For the group membership to take effect, you'll have to log out and log back in once.

That's it for building and flashing. Now connect to the arduino with a serial terminal:
```console
$ picocom -b 115200 /dev/ttyACM0
```

Arduino will echo whatever you type into the terminal, and with each keypress also toggle the LED.
