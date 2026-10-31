# Active coordinator board and antenna routing

## Hardware status

The user connected the **Waveshare ESP32-C6-Touch-LCD-1.9** on 2026-10-30 and selected it to replace XIAO as coordinator. Its implementation includes the implemented status display and touch navigation. Hardware observations/results remain in the integration operations guide; the precise PCB revision has not been read from a board marking. Prior XIAO results remain recorded separately.

## Active board

This branch has one active Waveshare target: 8 MB flash, native framed USB NCP and LCD/touch status UI. `sdkconfig.defaults` owns the active configuration. Build/flash/host scripts do not select a board version.

XIAO's external-antenna coordinator is retained by Git tag `coordinator-xiao-v1` at `8506d72`. Check out that version for its implementation/commands; no dormant XIAO RF setup runs in the current Waveshare application. The coordinator development branch includes subsequent Nordic and Waveshare work. Runtime pairing/keys remain local.

## Waveshare mapping

| Peripheral | Pins/settings |
| --- | --- |
| ST7789V2 LCD | MOSI4, clock5, DC6, CS7, reset14; 20 MHz SPI; 170×320 portrait with column gap35 |
| Backlight | GPIO15, active low through the board's P-channel transistor |
| CST816 touch | I2C SDA18/SCL8, address0x15; polled coordinates in portrait orientation |
| USB NCP | Native USB-Serial/JTAG; no second serial client |

The board driver's pin header owns these definitions. SD card, IMU, Wi-Fi and GPIO expander functions are not initialized by this implementation. The board's external-antenna hardware route is not the XIAO software switch. Sources are the [manufacturer example](https://github.com/waveshareteam/ESP32-C6-LCD-1.9/tree/1fed15e32977de11afe1f710a0f5e915bfcc35a5/02_Example/ESP-IDF/07_LVGL_Test) and [touch schematic](https://files.waveshare.com/wiki/ESP32-C6-LCD-1.9/ESP32-C6-Touch-LCD-1.9-Schematic.pdf).

Display rotation is implemented in the portable UI pixel/touch transforms. The board retains native portrait addressing and its 35-pixel column gap. Display saving switches the panel and active-low backlight off while CST816 polling and the coordinator continue. The separate backlight module uses inverted10kHz/10-bit low-speed LEDC PWM, retaining a10–100% setting and restoring it on wake. Configuration follows [ESP-IDF5.5.1 LEDC](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32c6/api-reference/peripherals/ledc.html); perceived brightness and power have not been calibrated.

The board identity reader requests `ESP_MAC_BASE` and reserves eight SDK bytes before formatting the six-byte base address. ESP-IDF5.5.1's factory-eFuse helper returns an eight-byte EUI-64 on C6; treating that call as a six-byte write corrupts adjacent state. The contract is isolated in `board_identity.c` and covered by a native sanitizer regression.

[Existing firmware architecture](Architecture.md)

## Interpreting the reported link quality

The user reports Waveshare improving from LQI 154 to 198 after modifying its PCB antenna path, using the same external antenna as XIAO; XIAO was approximately 254. These observations are user-reported, not agent-measured. LQI is not RSSI in dBm or a packet-delivery percentage. [Zigbee2MQTT explains the measurement limits](https://www.zigbee2mqtt.io/guide/faq/#how-to-interpret-rssi-and-lqi-values).

The saved XIAO configuration selects an external antenna. The Waveshare's [manufacturer guide](https://docs.waveshare.com/ESP32-C6-LCD-1.9) states that its IPEX route requires solder-pad modification; the user confirms a PCB modification and external-antenna use. The exact solder-pad/resistor arrangement has not been inspected here. An onboard-versus-external antenna comparison therefore does not explain the reported remaining difference. Its cause, packet-delivery impact and radio range remain unmeasured.
