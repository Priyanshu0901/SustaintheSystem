# Coordinator board profiles and antenna routing

## Hardware status

The user connected the **Waveshare ESP32-C6-Touch-LCD-1.9** on 2026-10-30 and selected it to replace XIAO as coordinator. Its profile includes the implemented status display and touch navigation. Hardware observations/results remain in the integration operations guide; the precise PCB revision has not been read from a board marking. Prior XIAO results remain recorded separately.

## Existing firmware profiles

| Profile | Antenna / pins | Validation boundary |
| --- | --- | --- |
| `xiao-ext-usb` | Upstream external-antenna switch enabled: GPIO3 low, GPIO14 high; 4 MB flash | XIAO only; requires an attached antenna |
| `generic-usb` | External switch disabled; upstream still configures GPIO3 and GPIO14 as inputs | Comparison build only; not an approved display-board image |
| `waveshare-touch-usb` | 8 MB, onboard antenna, optional UI component; XIAO RF handling excluded entirely | Waveshare 1.9-inch touch variant only |

All profiles reserve USB for framed NCP traffic and disable parallel UART1. The Waveshare adds framed private UI status/diagnostic requests; it does not put logs or raw UI text on USB. Its optional patch is applied only to the build copy, while XIAO/generic builds retain unpatched upstream behavior.

## Waveshare mapping

| Peripheral | Pins/settings |
| --- | --- |
| ST7789V2 LCD | MOSI4, clock5, DC6, CS7, reset14; 20 MHz SPI; 170×320 portrait with column gap35 |
| Backlight | GPIO15, active low through the board's P-channel transistor |
| CST816 touch | I2C SDA18/SCL8, address0x15; polled coordinates in portrait orientation |
| USB NCP | Native USB-Serial/JTAG; no second serial client |

The board driver's pin header owns these definitions. SD card, IMU, Wi-Fi and GPIO expander functions are not initialized by this profile. The board's external-antenna hardware route is not the XIAO software switch. Sources are the [manufacturer example](https://github.com/waveshareteam/ESP32-C6-LCD-1.9/tree/1fed15e32977de11afe1f710a0f5e915bfcc35a5/02_Example/ESP-IDF/07_LVGL_Test) and [touch schematic](https://files.waveshare.com/wiki/ESP32-C6-LCD-1.9/ESP32-C6-Touch-LCD-1.9-Schematic.pdf).

The [Seeed guide](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/) documents the antenna switch. Configuration validation can establish that the external route was compiled in; it cannot establish connector quality, antenna gain or measured range.

[Existing firmware architecture](Architecture.md)
