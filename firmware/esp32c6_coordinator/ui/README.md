# Coordinator UI

This directory owns the Waveshare screen design. Keep appearance and page decisions here; board pins belong in `../boards/waveshare_touch_1_9/`, and Zigbee remains in the pinned upstream firmware.

| Change | Edit |
| --- | --- |
| Colors | [ui_theme.h](ui_theme.h) |
| Text, spacing, counts and page layout | [ui_view.c](ui_view.c) |
| Tabs, status validation, freshness timeout | [ui_model.c](ui_model.c), [ui_model.h](ui_model.h) |
| Rendering cadence, touch polling, bounded diagnostics | [ui_runtime.c](ui_runtime.c) |
| LCD orientation, pins and touch bus | [board driver](../boards/waveshare_touch_1_9/board_display.c), [pin map](../boards/waveshare_touch_1_9/board_pins.h) |
| Host registry/availability mapping | [host extension](../host/extensions/coordinator-display.cjs) |

Two read-only pages are implemented: Network and System. Touch tabs never open joining or change network settings. Network values come from the existing host; no example values are compiled into firmware. Unreported availability remains unknown. Host values expire after 15 seconds.

The renderer uses one 170×16 RGB565 tile (5,440 bytes), renders about once per second and stays independent of ESP-IDF. The runtime is a low-priority task; panel failure does not stop NCP startup. No LVGL dependency or second host installation is added.

Run [model/view tests](tests/model_and_preview.c) with a native C compiler, from this directory:

```sh
cc -std=c11 -Wall -Wextra -Werror -I. ui_model.c ui_view.c tests/model_and_preview.c -o /tmp/ui-test
/tmp/ui-test
/tmp/ui-test network > /tmp/ui-network.ppm
```

`system` and `waiting` produce the other preview states. Preview counts are test fixtures only. The same command can run inside the pinned ESP-IDF Docker image; its native `gcc` produces the test executable, not ESP firmware. [Host codec test](../host/extensions/test-display.cjs) runs with the existing portable Node from the integration directory:

```powershell
./host/.runtime/node-v22.23.4-win-x64/node.exe ./host/extensions/test-display.cjs
```

Build/flash/start commands and measured results remain in the [integration operations guide](../README.md). [Display architecture](../../../docs/Firmware/ESP32-C6%20Coordinator/Display%20UI.md) owns the host/firmware data contract. Work is on `codex/waveshare-coordinator-ui`; Git branches do not store ignored keys or runtime network databases.
