# Table of Content
- <a href="#description">Description</a>
- <a href="#building">Building and Install</a>
- <a href="#troubleshooting">Troubleshooting</a>

# Description <a name="description"></a>

Bought a SCHENKER MEDIA 15 (E23) Notebook using Ubuntu.
Well, could not disable the annoying Keyboardbacklight. The Fans where noisy . An could not configure Powerprofiles.
For Windows there are drivers and a Control-Center-Tool.

Modified only one File (tuxedo_compatibility_check.c) changing the Vendor Strings.

It works for my Notebook. Now i can control the Keyboardbacklight. The Fan is quiter and i can configure powerprofiles. The charing profile can altough be configured.

Drivers for several platform devices for TUXEDO meant for the DKMS.

## Features
- Driver for Fn-keys
- SysFS control of brightness/color/mode for most TUXEDO keyboards
    - [https://docs.kernel.org/leds/leds-class.html](https://docs.kernel.org/leds/leds-class.html)
    - [https://docs.kernel.org/leds/leds-class-multicolor.html](https://docs.kernel.org/leds/leds-class-multicolor.html)
- Hardware I/O driver for TUXEDO Control Center

## Modules included in this package
- tuxedo_compatibility_check
- tuxedo_keyboard
- clevo_acpi
- clevo_wmi
- uniwill_wmi
- tuxedo_io
- ite_8291
- ite_8291_lb
- ite_8297
- ite_829x
- tuxedo_nb05_ec
- tuxedo_nb05_power_profiles
- tuxedo_nb05_sensors
- tuxedo_nb05_keyboard
- tuxedo_nb04_keyboard
- tuxedo_nb04_wmi_ab
- tuxedo_nb04_wmi_bs
- tuxedo_nb04_sensors
- tuxedo_nb04_power_profiles
- tuxedo_nb04_kbd_backlight

# Building and Install <a name="building"></a>

## Warning when installing the module:

The `make install` and `make dkmsinstall` targets are only intended for development purposes. Only use them if you know how to recover from a botched root file system or DKMS state!

For production systems use the version from the package repository where available or the deb or rpm package generated by `make package-deb` and `make package-rpm` respectively.

## Dependencies:
All:
- make

`make install`:
- gcc or clang
- linux-headers

`make dkmsinstall`:
- dkms
- linux-headers

`make package-deb`:
- devscripts
- debhelper

`make package-rpm`:
- rpm

# Troubleshooting <a name="troubleshooting"></a>

## The keyboard backlight control and/or touchpad toggle key combinations do not work
For all devices with a touchpad toggle key(-combo) and some devices with keyboard backlight control key-combos the driver does nothing more then to send the corresponding key event to userspace where it is the desktop environments duty to carry out the action. Some smaller desktop environments however don't bind an action to these keys by default so it seems that these keys don't work.

Please refer to your desktop environments documentation on how to set custom keybindings to fix this.

For keyboard brightness control you should use the D-Bus interface of UPower as actions for the key presses.

For touchpad toggle on X11 you can use `xinput` to enable/disable the touchpad, on Wayland the correct way is desktop environment specific.
