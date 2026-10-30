# ESP32-C6 coordinator firmware

The ESP32-C6 runs a USB ZBOSS network coprocessor. XIAO and Waveshare touch profiles share the upstream core; the Waveshare also runs an isolated status UI. The host runs Zigbee2MQTT and MQTT and owns device definitions and decoded state.

| Question | Read |
| --- | --- |
| What code and runtime boundaries exist? | [Architecture](Architecture.md) |
| How is the separate status UI wired to the host? | [Display UI](Display%20UI.md) |
| Why this upstream and how are builds isolated? | [Upstream and builds](Upstream%20and%20Builds.md) |
| Which board and antenna settings are active? | [Active board and Git-retained versions](Board%20Profiles.md) |
| How are radio, host and application responsibilities divided? | [Host integration](Host%20Integration.md) |
| Where are original scripts documented? | [Code reference](Code%20Reference.md) |
| How do I build, flash, start the host or inspect results? | [Firmware operations](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/esp32c6_coordinator) |

The operations guide and validation record own measured results.
