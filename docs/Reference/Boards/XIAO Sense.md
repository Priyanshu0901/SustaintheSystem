# XIAO nRF54LM20A Sense board and sensors

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

| Board addition | Documented specification |
| --- | --- |
| Storage | 8 MB external flash, separate from internal NVM and RAM |
| Power management | Nordic nPM1300, regulation and battery charging |
| Input power | USB-C 5 V or nominal 3.7 V battery |
| Radio connection | IPEX4 external-antenna connector |
| Controls | RGB user LED, charging indicator, reset and user buttons |
| Size / temperature | 21 × 17.8 mm; −20 to +70°C |
| Expansion | Advertised 28 GPIO; pin multiplexing and onboard use constrain availability |

Seeed reports BAT-powered System OFF at **4.76 µA**, **4.92 µA with GRTC wake**, and ship mode at **0.33 µA**. These are vendor test results, not measurements of our boards. The board contains an **LM20A, so no Axon NPU**. Source: [Seeed board guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_getting_started/).

## Onboard sensors

Seeed identifies **LSM6DS3TR-C** for motion and **MSM261DGT006** for audio. Its examples use I²C for the IMU and PDM for the microphone, and demonstrate interrupt wake-up. These components are board additions, not integrated LM20A sensors. [Seeed onboard-sensor guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_with_onboard/).

Microphone follow-up checked **2026-10-10**: the [MEMSensing manufacturer catalog](https://www.memsensing.com/product/176.html) lists the exact **MSM261DGT006** as PDM, top port, size 4.0 × 2.0 × 1.1, sensitivity −26, SNR 65 and AOP 120. The catalog headers do not state units, reference conditions, weighting or tolerances; do not promote these summary values to a complete calibrated specification. Full exact-part frequency response, low-frequency noise, clock/power modes and current were not verified. Specifications for similarly named DGT003 parts are not substituted.

Seeed's [recorder source](https://files.seeedstudio.com/wiki/XIAO_nRF54LM20A/main.c) uses **16 kHz, 16-bit mono**, external-flash capture and subsequent BLE NUS/WAV upload. Its guide describes a maximum ten-second recording followed by transfer to a Windows receiver. This is vendor code/documentation, not a local build, live phone stream or measured body-sound capability. PDM output and configured sample rate do not establish acoustic bandwidth.

| LSM6DS3TR-C function | Component capability |
| --- | --- |
| Acceleration | Three axes; selectable ±2/4/8/16 g |
| Angular velocity | Three axes; ±125/250/500/1000/2000 degrees/s |
| Buffering | Up to 4 KB FIFO |
| Events | Wake, free-fall, orientation, tap/double-tap, tilt and step functions |
| Other | Embedded temperature sensor; I²C/SPI component interfaces |

Source: [ST component specification](https://www.st.com/en/mems-and-sensors/lsm6ds3tr-c.html). Component features still require matching board wiring and driver configuration. This is a six-axis IMU, not a magnetometer. No ultrasonic microphone bandwidth, calibrated sound-pressure accuracy, or assembled-board vibration bandwidth has been verified here.

[SDK and pin uncertainties](SDK%20Compatibility.md) · [Node board profiles](../../Firmware/Nordic%20Zigbee/Board%20Profiles.md)
