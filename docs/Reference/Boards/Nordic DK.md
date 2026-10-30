# nRF54LM20 DK hardware and measurement

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

DK-specific hardware/software follow-up checked **2026-10-10**; the rest of this catalog retains its original verification date.

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

The documented kit has no onboard microphone or IMU. The microphone array and power-measurement instruments are external additions; board features alone do not establish a complete sensing product.

Sample and release limits: [radio SDK support](../Radio/SDK%20Support.md). The Axon sample requires the Edge AI Add-on and a compatible compiled model; it has not been built or tested locally.


[Existing DK firmware profiles](../../Firmware/Nordic%20Zigbee/Board%20Profiles.md)
