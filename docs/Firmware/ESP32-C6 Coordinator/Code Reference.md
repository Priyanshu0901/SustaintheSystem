# Coordinator code reference

The protocol implementation remains in the pinned upstream fork. This project owns build/flash wrappers, active board configuration and the native Windows host setup; it does not maintain a second copy of the protocol core.

## Original integration code

Paths are relative to [the integration source](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/esp32c6_coordinator).

| Source | Responsibility |
| --- | --- |
| `Build-Firmware.ps1` | Enforce pinned/clean source, archive it and launch the single-board container |
| `tools/build.py` | Hash inputs, invalidate stale success state, build with ESP-IDF, validate effective settings and export hashed images |
| `ui/ui_model.c`, `ui/ui_model.h` | Host validation/expiry, menu/page state, local rotation and display saving |
| `ui/ui_geometry.c`, `ui/ui_geometry.h` | Logical-to-panel tile packing and inverse touch transforms |
| `ui/generate_splash.py`, `ui/assets/` | Replaceable original PNG/manifest and build-time RGB565 generation |
| `ui/ui_view.c`, `ui/ui_theme.h` | Platform-independent screen layout and theme |
| `ui/ui_runtime.c`, `ui/coordinator_ui.h` | Rendering/touch runtime, local NVS settings, panel sleep/wake and bounded vendor interface |
| `boards/waveshare_touch_1_9/` | Board pins, LCD DMA lifecycle and CST816 I2C polling |
| `patches/waveshare-ui.patch` | Waveshare build-copy hooks; upstream dependency checkout remains clean |
| `host/extensions/coordinator-display.cjs` | Send truthful registry/availability snapshots through the existing serial queue |
| `Install-FlashTools.ps1` | Prepare the local pinned esptool environment |
| `Flash-Firmware.ps1` | Check artifact identity, selected device and security state; program and verify before boot |
| `host/Install-Host.ps1` | Prepare isolated pinned Node, npm host dependencies and broker |
| `host/Start-Broker.ps1` | Start the loopback-only foreground broker |
| `host/Start-Zigbee2MQTT.ps1` | Preserve runtime config, select the serial device, scope Git discovery and supply the converter dependency junction |
| `sdkconfig.defaults` | One active Waveshare USB/flash configuration |

`tools/build.py` defines `sha(path)` for file hashing, `run(*args, **kwargs)` for checked subprocesses, and `main()` for the container build/export lifecycle. These Python functions and their source are indexed by Doxygen. PowerShell scripts are mapped here and retain their actual parameters beside source; Doxygen does not parse their APIs.

## Generated reference

The shared [generator guide](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/tools/documentation) owns installation, generation commands, output locations and parser coverage. This maintained code map is included directly in its generated reference.

[Upstream and builds](https://github.com/Priyanshu0901/SustaintheSystem/blob/master/docs/Firmware/ESP32-C6%20Coordinator/Upstream%20and%20Builds.md) explains the design; [operations](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/esp32c6_coordinator) owns commands, pins, dependency provenance and measured results.
