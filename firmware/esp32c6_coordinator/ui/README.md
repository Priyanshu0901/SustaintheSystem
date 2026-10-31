# Coordinator UI

This directory owns the Waveshare screen design. Keep appearance and page decisions here; board pins belong in `../boards/waveshare_touch_1_9/`, and Zigbee remains in the pinned upstream firmware.

| Change | Edit |
| --- | --- |
| Colors | [ui_theme.h](ui_theme.h) |
| Text, spacing, counts and page layout | [ui_view.c](ui_view.c) |
| Menu, page selection, rotation, idle timer, status validation | [ui_model.c](ui_model.c), [ui_model.h](ui_model.h) |
| Rotation packing and inverse touch coordinates | [ui_geometry.c](ui_geometry.c), [ui_geometry.h](ui_geometry.h) |
| Rendering cadence, touch polling, bounded diagnostics | [ui_runtime.c](ui_runtime.c) |
| Panel addressing, backlight, pins and touch bus | [board driver](../boards/waveshare_touch_1_9/board_display.c), [pin map](../boards/waveshare_touch_1_9/board_pins.h) |
| Replaceable boot logo and generated asset | [assets/splash.json](assets/splash.json), [generate_splash.py](generate_splash.py) |
| Host registry/availability mapping | [host extension](../host/extensions/coordinator-display.cjs) |

The hamburger opens icon rows for Network, Settings and Debug. Network owns live counts; Debug owns board health and touch/frame/error counters. Settings cycles four clockwise rotations and display-saving timeout off/15/30/60 seconds (default 30). These two local settings persist in the `display` NVS namespace; failed storage is shown on Settings. No screen opens joining or changes the network. Values come from the existing host, unreported availability stays unknown and host values expire after 15 seconds.

Portrait is 170×320; landscape is 320×170. The pure C renderer uses a logical tile and a separate DMA/wire tile, each bounded to 320×16 RGB565 pixels (20,480 bytes total). Software rotation maps every tile to native portrait LCD addresses and inversely maps touch. Transfer completion owns reuse; a timed-out DMA buffer is retained. Normal refresh is about once per second with immediate touch redraw. No rendering occurs during display sleep. The panel and backlight turn off, touch polling continues, and the first touch only wakes without activating a control. Holding touch prevents idle sleep; host heartbeats do not reset the timer. No MCU/radio sleep is requested.

Startup shows the original RayKestrel PNG for 1.8 seconds, or until touch. Replace the original-named asset or edit `assets/splash.json`, then rebuild. A non-interlaced 8-bit RGB/RGBA PNG up to 4096×4096 is supported. `generate_splash.py` validates PNG CRCs/filters, preserves aspect ratio and area-averages color/alpha onto black. Its header is generated only in the build tree. Build/flash manifests cover the PNG, JSON, converter, UI and board sources. The UI task runs independently of NCP startup; no LVGL or image-decoding dependency is added.

Run [model/view/rotation tests](tests/model_and_preview.c) with Python and native GCC from this directory:

```sh
python tests/run.py --output ../build/ui-navigation --previews
```

This uses address/undefined sanitizers and checks invalid host data, timeout/wraparound, menu/settings/wake consumption, every pixel/touch inverse in all rotations and tile bounds. It exports Network, waiting, menu, Debug, Settings and splash PPMs in portrait and landscape. Preview counts are test fixtures only. The same command runs inside the pinned ESP-IDF Docker image with `/integration/ui/tests/run.py --output /integration/build/ui-navigation --previews` after mounting the integration at `/integration`; native GCC tests the UI, not ESP firmware. [Host codec test](../host/extensions/test-display.cjs) runs with the existing portable Node from the integration directory:

```powershell
./host/.runtime/node-v22.23.4-win-x64/node.exe ./host/extensions/test-display.cjs
```

Build/flash/start commands and measured results remain in the [integration operations guide](../README.md). [Display architecture](../../../docs/Firmware/ESP32-C6%20Coordinator/Display%20UI.md) owns the host/firmware contract. Waveshare is the sole maintained product, developed under `POC/zigbee_coordinator_dev/waveshare`; XIAO is historical tag-only.
