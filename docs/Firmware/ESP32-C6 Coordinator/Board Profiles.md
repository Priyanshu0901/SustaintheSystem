# Active coordinator board and antenna routing

## Hardware status

The user connected the **Waveshare ESP32-C6-Touch-LCD-1.9** on 2026-10-30 and selected it to replace XIAO as coordinator. Its implementation includes the implemented status display and touch navigation. Hardware observations/results remain in the integration operations guide; the precise PCB revision has not been read from a board marking. Prior XIAO results remain recorded separately.

## Active board

This branch has one active Waveshare target: 8 MB flash, native framed USB NCP and LCD/touch status UI. `sdkconfig.defaults` owns the active configuration. Build/flash/host scripts do not select a board version.

XIAO's external-antenna coordinator is retained by Git tag `coordinator-xiao-v1` at `8506d72` and existing branch `POC/zigbee_coordinator`. Check out that version for its implementation/commands; no dormant XIAO RF setup runs in the current Waveshare application. Runtime pairing/keys remain local.

## Waveshare mapping

| Peripheral | Pins/settings |
| --- | --- |
| ST7789V2 LCD | MOSI4, clock5, DC6, CS7, reset14; 20 MHz SPI; 170×320 portrait with column gap35 |
| Backlight | GPIO15, active low through the board's P-channel transistor |
| CST816 touch | I2C SDA18/SCL8, address0x15; polled coordinates in portrait orientation |
| USB NCP | Native USB-Serial/JTAG; no second serial client |

The board driver's pin header owns these definitions. SD card, IMU, Wi-Fi and GPIO expander functions are not initialized by this implementation. The board's external-antenna hardware route is not the XIAO software switch. Sources are the [manufacturer example](https://github.com/waveshareteam/ESP32-C6-LCD-1.9/tree/1fed15e32977de11afe1f710a0f5e915bfcc35a5/02_Example/ESP-IDF/07_LVGL_Test) and [touch schematic](https://files.waveshare.com/wiki/ESP32-C6-LCD-1.9/ESP32-C6-Touch-LCD-1.9-Schematic.pdf).

The [Seeed guide](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/) documents the antenna switch. Configuration validation can establish that the external route was compiled in; it cannot establish connector quality, antenna gain or measured range.

[Existing firmware architecture](Architecture.md)

## Interpreting the reported link quality

The user reports Waveshare LQI 198 versus XIAO approximately 254. LQI is not RSSI in dBm or a packet-delivery percentage; compare delivery/sequence behavior along with link quality. [Zigbee2MQTT explains the measurement limits](https://www.zigbee2mqtt.io/guide/faq/#how-to-interpret-rssi-and-lqi-values).

The saved XIAO configuration selects an external antenna. The Waveshare uses its onboard ceramic antenna; its [manufacturer guide](https://docs.waveshare.com/ESP32-C6-LCD-1.9) states that using the touch board's IPEX connector requires solder-pad modification. Antenna type, placement, orientation and nearby electronics can affect the link, but their contribution here has not been isolated by a controlled measurement. The lower number alone does not demonstrate a firmware fault or establish radio range.
