# Nordic board SDK compatibility and open pin questions

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

Seeed documents nRF Connect SDK/Zephyr and its PlatformIO platform. Its NCS guide specifies **NCS and toolchain 3.3.0**, with separately supplied XIAO board definitions because they are not yet merged upstream according to that guide. This is the vendor's documented setup, not a locally validated environment or a claim about every later release. [Seeed NCS guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_ncs/).

The Axon pages reviewed identify **Edge AI Add-on 2.3.0**; their `latest` links are moving references. Exact SDK/add-on compatibility must come from the release used. No firmware build, flash, sensor capture, radio test or power measurement was performed for this reference.

| Unresolved item | Evidence boundary |
| --- | --- |
| XIAO analog-pin count | Seeed advertises nine analog pins; Nordic specifies eight ADC channels. Do not treat this as nine independent ADC channels. Pin map needs reconciliation. |
| XIAO I²C pin descriptions | Getting-started table labels external I²C as including IMU, while dedicated IMU rows/examples use different pins. Use revision-matched schematic and board definition before wiring. |
| XIAO USB routing | Native LM20 USBHS access through the board connector is not established by the USB-C feature label. |
| Feature combinations | PAwR roles, Channel Sounding configuration, multiprotocol concurrency and model compatibility not tested locally. |

Pin-documentation discrepancies come from the [Seeed guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_getting_started/) and [sensor examples](https://wiki.seeedstudio.com/xiao_nrf54lm20a_with_onboard/). Update this note when revision-specific pin details are verified.

The application now uses its own [pinned inputs and validation](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/nordic_zigbee#inputs-and-provenance); the older vendor-guide version above is reference history, not the project build selection.
