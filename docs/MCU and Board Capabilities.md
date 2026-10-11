# MCU and board capabilities

Verified against manufacturer documentation: **2026-09-26**. [Overview](Overview.md) · [Project](Project.md)

Factual reference for project discussions and decisions. Scope: **nRF54LM20A/B**, their **nRF54L family**, the **nRF54LM20 DK**, and **XIAO nRF54LM20A Sense**. No application or architecture is selected here. Specifications describe documented capability, not measurements on our hardware. Linked `latest` documentation can change.

## What the names refer to

| Name | What it is |
| --- | --- |
| MCU | Processor, memory and control peripherals |
| Wireless SoC | MCU functionality plus radio and other integrated hardware |
| nRF54LM20A / nRF54LM20B | Two wireless SoCs within Nordic's nRF54L series |
| nRF54LM20 DK | Nordic development board for these SoCs |
| XIAO nRF54LM20A Sense | Seeed board combining the LM20A with additional components and sensors |

Project inventory status: the supplied discussion reports three XIAO Sense boards ordered; receipt and revisions have not been verified. The DK was discussed as a future purchase. This note does not establish ownership.

## LM20A and LM20B: processing and memory

| Capability | LM20A | LM20B |
| --- | --- | --- |
| Main CPU | 128 MHz Arm Cortex-M33 | Same |
| CPU computation | DSP instructions and floating-point unit | Same |
| Coprocessor | 128 MHz RISC-V, including SoftPeripheral support | Same |
| Internal nonvolatile memory | 2036 KB RRAM; marketed as 2 MB NVM | Same |
| RAM | 512 KB | Same |
| Dedicated Axon NPU | Absent | 128 MHz accelerator |

The B adds acceleration; it does not add RAM or a faster main CPU. CPU-based models can run without an NPU. Memory available to application data is less than the total once firmware, stacks, buffers and runtime allocations are included. Sources: [LM20B product specification](https://www.nordicsemi.com/Products/nRF54LM20B), [LM20A/B datasheet](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html).

## LM20A/B peripheral inventory

| Function | Documented hardware capability |
| --- | --- |
| Serial interfaces | Seven EasyDMA serial instances; alternative functions share resources |
| SPI | One up to 32 MHz; six up to 8 MHz |
| UART | One up to 4 Mbps; six up to 1 Mbps |
| I²C / TWI | Six up to 400 kHz |
| ADC | Eight programmable-gain channels; 10-bit up to 2 MS/s; oversampled 12-bit at 125 kS/s or 14-bit at 62.5 kS/s |
| Audio inputs/interfaces | PDM; TDM configurable as I²S |
| PWM | Three four-channel units |
| Counters | Seven 32-bit timers; two quadrature decoders |
| Supervision | Two watchdogs; temperature sensor |
| Wake and timing | Global RTC in System OFF; comparator/low-power comparator wake support |
| NFC | NFC-A listening peripheral |
| Debug | SWD and trace facilities |

These are SoC resources, not a promise that every board exposes every signal. ADC rate/resolution combinations are alternatives, not 14-bit conversion at 2 MS/s. Source: [LM20A/B peripheral specifications](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html).

### Audio acquisition

Audio acquisition follow-up checked **2026-10-10**: the [LM20A/B PDM peripheral](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/pdm.html) documents one/two microphone inputs, hardware decimation to **16-bit PCM**, nominal 8/16/32/48 kHz output and EasyDMA. The [register table](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/pdm.html-topic) lists high-pass pole/disable controls; no configured cutoff/default or SDK driver exposure was established here. The [TDM peripheral](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/tdm.html) supports mono/stereo I²S, EasyDMA, 8/16/24/32-bit sample widths and controller/target clocks. These are hardware capabilities, not a built sample or a promise every microphone clock/rate/format works. Exact wiring, driver, filter and resource compatibility need verification. [Chest wearable assessment](Research/Opportunity%20Assessment.md#lm20b-chest-body-sensing-pdm-versus-i2s--2026-10-10) owns the application tradeoff. This targeted follow-up does not refresh the full catalog.

### USB

USBHS is a USB 2.0 **device** peripheral: documented modes include 480 Mbps high speed, 12 Mbps full speed and 1.5 Mbps low speed; up to 16 bidirectional endpoints and DMA access. OTG is not supported. These are bus rates, not guaranteed application transfer rates. A board's USB-C connector does not by itself establish which SoC signals it exposes. Source: [USBHS specification](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/usbhs.html).

## Wireless capabilities and boundaries

Detailed protocol explanations and radio specifications: [Bluetooth and 2.4 GHz radio](Bluetooth%20and%202.4%20GHz%20Radio.md).

| Capability | LM20A/B status |
| --- | --- |
| Bluetooth LE PHYs | 1M, 2M and LE Coded |
| Channel Sounding | Supported hardware capability |
| IEEE 802.15.4 | 250 kbps; basis for Thread and Zigbee |
| Proprietary 2.4 GHz | 1, 2 and 4 Mbps modes |
| Higher-level protocols | Nordic lists Matter, Bluetooth Mesh and Aliro |
| Wi-Fi | Requires an external nRF70 companion; not an integrated Wi-Fi radio |

Source: [Nordic LM20B connectivity specifications](https://www.nordicsemi.com/Products/nRF54LM20B).

Nordic also lists PAwR (Periodic Advertising with Responses) at series level. Its presence in the portfolio is not evidence that a particular SDK release, board target and role combination has been tested here. [Nordic family overview](https://www.nordicsemi.com/Products/nRF54LM20A).

Interpretation boundaries: the 4 Mbps proprietary mode is not a 4 Mbps Bluetooth LE mode. Multiprotocol support does not establish simultaneous independent radios. Packet overhead, scheduling, interference and firmware determine usable throughput. Channel Sounding support alone establishes neither range nor accuracy for a finished board. This reference does not assert Bluetooth Classic, cellular, GNSS, LoRa or native Wi-Fi support.

## Electrical limits, packages and security

LM20A/B operating envelope: **1.7–3.6 V**, **−40 to +85°C**. Package-dependent GPIO counts are **66 (CSP98), 40 (CSP61), 32 (QFN52)**. These limits apply to the chip, not automatically the assembled board. [Nordic family/package table](https://www.nordicsemi.com/Products/nRF54LM20A).

Nordic's LM20B page reports RX **3.3 mA**, TX **5.0 mA at 0 dBm**, and sleep modes **0.7–4.3 µA**, at **3 V**. Maximum TX is **+8 dBm CSP / +7 dBm QFN**. These are vendor operating-point figures, not whole-board averages or a battery-life estimate. [LM20B power specifications](https://www.nordicsemi.com/Products/nRF54LM20B).

TrustZone isolation, cryptographic acceleration, key management, secure boot/storage facilities, tamper detection and side-channel protections are documented. Their presence does not mean project firmware has configured them. [LM20A/B security features](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html).

## Axon: what the B adds

Axon accelerates supported neural-network inference. Nordic also describes Neuton models executing on the CPU; **LM20A is not restricted to DSP-only processing**. The earlier proposed A/B division was an architecture suggestion, not a chip limitation. [Nordic Edge AI overview](https://www.nordicsemi.com/Products/Technologies/Edge-AI/Axon-NPU).

| Software capability | Documented path |
| --- | --- |
| Neural inference | Compile model, integrate generated data/code and invoke Axon driver |
| Model inputs | int8 `.tflite`/`.lite`; local compiler also documents Keras `.h5` with quantization data |
| Compilation | Nordic Edge AI Lab or local compiler |
| Validation | Compiler accuracy checks/test vectors; on-device inference comparison |
| Compatibility | Model operators, shapes and buffer requirements must satisfy compiler/runtime support |

The compiler documents a model-support scanner. A TensorFlow Lite file is not automatically supported merely because it fits storage. Neither source establishes a general on-device model-training capability. Sources: [Axon compiler](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/axon_compiler/Axon_npu_tflite_compiler.html), [driver integration](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/quick_start/axon_driver_inference.html).

Axon additionally exposes **DSP intrinsics**, including vector arithmetic, FFT/power spectrum, FIR filtering, mathematical and 2D operations. These can be called without compiling a neural model. Sources: [DSP API workflow](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/quick_start/axon_driver_intrinsics.html), [documented intrinsic tests](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/tests/axon/intrinsics/README.html).

For performance context only, Nordic reports keyword-spotting inference at **4.5 ms / 40.5 µJ**, and its anomaly-detection benchmark at **1.3 ms / 13.7 µJ**, with Axon at 128 MHz and current measured at 3 V. These specific benchmark results do not establish our model's latency, accuracy or energy. [Vendor benchmarks](https://www.nordicsemi.com/Products/Technologies/Edge-AI/Axon-NPU).

## The two development boards

### Nordic nRF54LM20 DK

DK-specific hardware/software follow-up checked **2026-10-10**; the rest of this catalog retains its original verification date. Proposed application utilization is in [Opportunity assessment](Research/Opportunity%20Assessment.md#development-kit-utilization-for-ideas-25--2026-10-10).

Nordic's feature list specifies **LM20B silicon**, with **LM20A as an emulated target**. It also lists:

- SEGGER J-Link onboard programmer/debugger.
- 8 MB external flash.
- 2.4 GHz and NFC antennas; SWF RF connector.
- Four user LEDs and four buttons.
- Two virtual serial UART interfaces.
- Pins for external current measurement; this is not an integrated power profiler.

The page has a conflicting lower heading describing an A-equipped kit. The explicit feature list and Axon sample target support the B-equipped interpretation; the physical kit revision is still unverified. Sources: [DK features](https://www.nordicsemi.com/Products/Development-hardware/nRF54LM20-DK), [Hello Axon target `nrf54lm20dk/nrf54lm20b/cpuapp`](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/samples/axon/hello_axon/README.html).

The [hardware guide](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/) additionally documents:

- [GPIO headers](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/page/ug/nrf54lm20_dk/hw_desription/connector_if.html?contentId=zyFFD8xtqL5Vn0qxo~wahw) for external devices. Pins share board functions; 5 V peripheral supply headers do not establish 5 V GPIO tolerance. Sensors and signal conditioning must match the actual pin map and electrical limits.
- [USB-powered nPM1300 supply](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/page/ug/nrf54lm20_dk/hw_desription/power_supply.html?contentId=QDRMnZ3ju3EJ8KL_fVGSGA), with programmable SoC supply from 1.8 to 3.3 V. This is not evidence of an included battery or verified battery-powered application.
- [Current measurement](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/page/ug/nrf54lm20_dk/measurements/current_measurements.html?contentId=mS~9Ba84N4rF3YfFGU~60A) requires an external PPK2, ammeter, oscilloscope or analyzer. P14 normally measures the application SoC domain, not total kit-plus-sensor consumption. [Flash supply configuration](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/page/ug/nrf54lm20_dk/hw_desription/external_memory.html?contentId=p6nFIm58HZYRdP~TunphSA) determines whether external-memory consumption enters that measurement.
- [Stand-alone external-supply mode](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/page/ug/nrf54lm20_dk/hw_desription/stand_alone_supply.html?contentId=rmwHy59aIfbEZRVmbgK3jQ) makes the debugger, virtual serial ports, external flash and LEDs unavailable. Normal USB-supported kit operation and this manually configured mode must not be treated as interchangeable.
- [Debug-out](https://docs.nordicsemi.com/r/bundle/ug_nrf54lm20_dk/page/ug/nrf54lm20_dk/program_debug/debug_out_ext_board.html?contentId=p8_VHkie_oOrQDPF1vnIcg) can program/debug supported external Nordic boards; external power, voltage and connector requirements apply. This is not a universal programmer for arbitrary boards.

The documented kit has no onboard IMU, microphone, calibrated environmental probe, water flow/pressure/level sensor or GNSS receiver. A die-temperature sensor is not a calibrated grain/ambient probe. Wi-Fi/cellular backhaul and measurement instruments are external additions. The kit is a development platform, not an established animal wearable or weatherproof field instrument.

The current [Channel Sounding reflector sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/bluetooth/channel_sounding/ras_reflector/README.html) lists LM20 DK A/B targets and requires a compatible initiator peer. Its reviewed `latest` page identifies NCS 3.4.99; this is a moving development reference, not verification of our installed or chosen stable release. The Axon sample requires the Edge AI Add-on and compatible compiled model. Neither sample has been built or tested locally.

### Seeed XIAO nRF54LM20A Sense

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

### Sense sensors

Seeed identifies **LSM6DS3TR-C** for motion and **MSM261DGT006** for audio. Its examples use I²C for the IMU and PDM for the microphone, and demonstrate interrupt wake-up. These components are board additions, not integrated LM20A sensors. [Seeed onboard-sensor guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_with_onboard/).

Microphone follow-up checked **2026-10-10**: the [MEMSensing manufacturer catalog](https://www.memsensing.com/product/176.html) lists the exact **MSM261DGT006** as PDM, top port, size 4.0 × 2.0 × 1.1, sensitivity −26, SNR 65 and AOP 120. The catalog headers do not state units, reference conditions, weighting or tolerances; do not promote these summary values to a complete calibrated specification. Full exact-part frequency response, low-frequency noise, clock/power modes and current were not verified. Specifications for similarly named DGT003 parts are not substituted.

Seeed's [recorder source](https://files.seeedstudio.com/wiki/XIAO_nRF54LM20A/main.c) uses **16 kHz, 16-bit mono**, external-flash capture and subsequent BLE NUS/WAV upload. Its guide describes a maximum ten-second recording followed by transfer to a Windows receiver. This is vendor code/documentation, not a local build, live phone stream or measured body-sound capability. PDM output and configured sample rate do not establish acoustic bandwidth. Application/coupling and phone feasibility are assessed in [Opportunity assessment](Research/Opportunity%20Assessment.md#phone-connected-stethoscope-feasibility--2026-10-10).

| LSM6DS3TR-C function | Component capability |
| --- | --- |
| Acceleration | Three axes; selectable ±2/4/8/16 g |
| Angular velocity | Three axes; ±125/250/500/1000/2000 degrees/s |
| Buffering | Up to 4 KB FIFO |
| Events | Wake, free-fall, orientation, tap/double-tap, tilt and step functions |
| Other | Embedded temperature sensor; I²C/SPI component interfaces |

Source: [ST component specification](https://www.st.com/en/mems-and-sensors/lsm6ds3tr-c.html). Component features still require matching board wiring and driver configuration. This is a six-axis IMU, not a magnetometer. No ultrasonic microphone bandwidth, calibrated sound-pressure accuracy, or assembled-board vibration bandwidth has been verified here.

## Related nRF54 family members

NVM figures below are Nordic's rounded product labels. All listed nRF54L parts have a 128 MHz Cortex-M33; peripheral and security sets differ.

| Part | NVM / RAM | Distinction |
| --- | --- | --- |
| nRF54LM20A | 2 MB / 512 KB | Extended peripherals, USBHS |
| nRF54LM20B | 2 MB / 512 KB | LM20 capability plus Axon |
| nRF54L15 | 1.5 MB / 256 KB | Five serial instances, PDM/I²S; no LM20 USBHS/NPU |
| nRF54L10 | 1 MB / 192 KB | Smaller-memory L variant |
| nRF54L05 | 0.5 MB / 96 KB | Smaller-memory L variant |

Source: [Nordic L15 family comparison](https://www.nordicsemi.com/Products/nRF54L15).

| Additional variant | NVM / RAM | Distinction |
| --- | --- | --- |
| [nRF54LC10A](https://www.nordicsemi.com/Products/nRF54LC10A) | 1 MB / 192 KB | Three serial instances; compact packages; shipping hibernation specified below 50 nA |
| [nRF54LV10A](https://www.nordicsemi.com/Products/nRF54LV10A) | 1 MB / 192 KB | Low-voltage supply and GPIO: 1.2–1.7 V |
| [nRF54LS05A](https://www.nordicsemi.com/Products/nRF54LS05A) | 0.5 MB / 64 KB | Entry-level Bluetooth/proprietary-radio part; baseline security |
| [nRF54LS05B](https://www.nordicsemi.com/Products/nRF54LS05B) | 0.5 MB / 96 KB | More RAM than LS05A; baseline security |

Do not transfer the LM20 TrustZone, NPU, coprocessor or protocol list to every variant. Pin compatibility is package-specific and sometimes qualified with limitations. Announced capability does not establish distributor stock. [Nordic comparison and package table](https://www.nordicsemi.com/Products/nRF54LM20A).

**nRF54H20 is a separate branch**, not an LM20 revision: multiple Cortex-M33 processors up to 320 MHz, RISC-V coprocessors, 2 MB NVM, 1 MB RAM, high-speed USB and CAN FD. Its features must not be attributed to LM20A/B. [Nordic H20 specification](https://www.nordicsemi.com/Products/nRF54H20).

## Software support and unresolved details

Seeed documents nRF Connect SDK/Zephyr and its PlatformIO platform. Its NCS guide specifies **NCS and toolchain 3.3.0**, with separately supplied XIAO board definitions because they are not yet merged upstream according to that guide. This is the vendor's documented setup, not a locally validated environment or a claim about every later release. [Seeed NCS guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_ncs/).

The Axon pages reviewed identify **Edge AI Add-on 2.3.0**; their `latest` links are moving references. Exact SDK/add-on compatibility must come from the release used. No firmware build, flash, sensor capture, radio test or power measurement was performed for this reference.

| Unresolved item | Evidence boundary |
| --- | --- |
| XIAO analog-pin count | Seeed advertises nine analog pins; Nordic specifies eight ADC channels. Do not treat this as nine independent ADC channels. Pin map needs reconciliation. |
| XIAO I²C pin descriptions | Getting-started table labels external I²C as including IMU, while dedicated IMU rows/examples use different pins. Use revision-matched schematic and board definition before wiring. |
| XIAO USB routing | Native LM20 USBHS access through the board connector is not established by the USB-C feature label. |
| Owned hardware | Receipt, exact SKU, PCB revision and silicon revision unknown. |
| Feature combinations | PAwR roles, Channel Sounding configuration, multiprotocol concurrency and model compatibility not tested locally. |

Pin-documentation discrepancies come from the [Seeed guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_getting_started/) and [sensor examples](https://wiki.seeedstudio.com/xiao_nrf54lm20a_with_onboard/). This note should be updated in place when those details are verified.
