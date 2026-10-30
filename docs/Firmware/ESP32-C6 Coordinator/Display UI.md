# Waveshare status display

The Waveshare ESP32-C6-Touch-LCD-1.9 runs both the USB ZBOSS coordinator and its local status display. Project-owned `ui/` code is separate from the board driver and the upstream protocol implementation. No Wi-Fi transport is used.

## Written components

| Component | Responsibility |
| --- | --- |
| `ui/ui_model.c` | Validate the bounded host snapshot, track its age, select the tab |
| `ui/ui_view.c`, `ui/ui_theme.h` | Pure C layout, glyphs and RGB565 colors, independent of ESP-IDF |
| `ui/ui_runtime.c` | Low-priority task, touch polling, one-second tile rendering and diagnostics |
| `boards/waveshare_touch_1_9/` | LCD/SPI/touch configuration and transfer completion |
| `patches/waveshare-ui.patch` | Waveshare startup/request hooks; historical XIAO RF routines removed from the build copy |
| `host/extensions/coordinator-display.cjs` | Project host extension; registry/availability snapshots through the existing driver queue |

Paths refer to [the saved coordinator integration](https://github.com/Priyanshu0901/SustaintheSystem/tree/c81f6b06622c0e0f06c419320156cac85812c71c/firmware/esp32c6_coordinator). Its `ui/README.md` owns the edit map and test commands; the integration README owns build/flash/start operations and measured results. The work is local until published.

The renderer uses a 170×16 RGB565 tile, not a full framebuffer. A single DMA buffer is reused only after transfer completion; after a transfer timeout it is never reused or freed while DMA might own it. LCD initialization/drawing failure is isolated from the NCP. The System page reports panel/touch readiness, uptime, touch events, completed frames and draw errors.

## Screen semantics

Network shows host/network status, channel, registered-node count excluding the coordinator, available/stale counts and an explicit unknown remainder. Registered means membership in the host registry, not proof of present RF reachability. Availability comes only from actual Zigbee2MQTT availability publications. With availability disabled or not yet reported, devices remain unknown; a sleepy device is not guessed offline from a short UI timer.

The host sends updates every five seconds. After 15 seconds without a valid update, the screen labels host data expired and replaces live counts/channel with dashes. Radio operation may continue while the host is absent; the screen does not claim the Zigbee network stopped. Bottom touch tabs select Network/System only; they cannot change channel, open joining or erase state.

## Bounded USB extension

USB retains ZBOSS framing, CRC, sequence numbers and one serial owner. Two private development commands use the host's existing serialized ZBOSS queue. Their schemas are registered in memory by the project external extension; installed npm package files are unchanged. Compatibility is checked against pinned zigbee-herdsman 10.10.0. This is a project-specific extension, not a standard ZBOSS API.

| Request | Payload | Result |
| --- | --- | --- |
| `0x7F10` status | Nine bytes: version u8, flags u8, channel u8, registered u16, available u16, stale u16 | Validate and update model, return diagnostics |
| `0x7F11` diagnostics | Empty | Read diagnostics without renewing host-data age |

Integers are little-endian. Version is 1. Request flags are network-up bit 0, MQTT-up bit 1, permit-join bit 2. Counts cannot exceed the upstream 200-node capacity, availability counts cannot exceed registered count, and active network channels are 11–26. Invalid payloads leave the model intact and return generic invalid-parameter status.

The 21-byte response contains category/status, UI version, diagnostic flags, page, touch count, frame count, draw-error count, uptime, host-data age and free heap in KiB. Flags indicate panel-ready bit 0, touch-ready bit 1, fresh-host-data bit 2 and last-reported network-up bit 3. Field sizes/serialization are owned by `ui/coordinator_ui.h`, `ui/ui_runtime.c` and the host schema. MQTT `bridge/display` exposes timestamped non-secret diagnostics. UI update failures do not terminate Zigbee2MQTT; after three failures polling stops and the panel's data expires.

The [manufacturer's pinned example](https://github.com/waveshareteam/ESP32-C6-LCD-1.9/tree/1fed15e32977de11afe1f710a0f5e915bfcc35a5/02_Example/ESP-IDF/07_LVGL_Test) and [touch-board schematic](https://files.waveshare.com/wiki/ESP32-C6-LCD-1.9/ESP32-C6-Touch-LCD-1.9-Schematic.pdf) establish the board mapping. [Board profiles](Board%20Profiles.md) owns pins and isolation; [architecture](Architecture.md) shows the complete implemented path.
