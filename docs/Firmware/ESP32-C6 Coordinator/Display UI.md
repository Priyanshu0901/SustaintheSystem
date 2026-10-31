# Waveshare status display

The Waveshare ESP32-C6-Touch-LCD-1.9 runs both the USB ZBOSS coordinator and its local status display. Project-owned `ui/` code is separate from the board driver and the upstream protocol implementation. No Wi-Fi transport is used.

## Written components

| Component | Responsibility |
| --- | --- |
| `ui/editor/coordinator.eez-project`, `ui/generated/` | EEZ0.29.0 no-Flow design and genuine generated LVGL9.5.0 widget tree |
| `ui/ui_model.c` | Host validation/freshness, stable page IDs, explicit control values and idle/wake state |
| `ui/ui_view.c` | Bind truthful labels, screen selection and image sources |
| `ui/ui_navigation.c`, `ui/ui_config.h` | One ordered registry; hide final Debug entry without gaps or page-ID changes |
| `ui/ui_settings.c` | Default-compatible NVS display preferences; persist completed selections |
| `boards/waveshare_touch_1_9/board_backlight.c` | Inverted GPIO15 PWM and restoration of selected brightness on wake |
| `ui/ui_layout.c` | Shared ten-pixel gutter, header and responsive widget geometry |
| `ui/ui_controls.c` | Selection callbacks and popup bounds/scroll reachability |
| `ui/ui_feedback.c` | Reusable transient help/save feedback, reflow and three-second expiry |
| `ui/firmware_version.h` | Single product release version used by About/manifest/SBOM |
| `ui/ui_geometry.c` | Rotate arbitrary LVGL dirty rectangles and inversely map touch |
| `ui/ui_runtime.c` | Sole LVGL task, display/input callbacks, NVS, display saving and diagnostics |
| `ui/lvgl.lock.json`, `ui/lv_conf.h` | Exact dependency/generator pins and shared native/ESP configuration |
| `ui/Generate-UI.ps1`, `tools/check_ui_generated.py` | Regenerate/verify canonical source-output provenance |
| `ui/assets/`, `ui/generate_splash.py` | Replaceable PNG and package-free alpha-preserving LVGL image conversion |
| `boards/waveshare_touch_1_9/` | LCD/touch, transfer completion, panel power and bounded base-MAC identity reader |
| `patches/waveshare-ui.patch` | Waveshare startup/request hooks; historical XIAO RF routines removed from the build copy |
| `host/extensions/coordinator-display.cjs` | Project host extension; registry/availability snapshots through the existing driver queue |

Paths refer to [the Waveshare development integration](https://github.com/Priyanshu0901/SustaintheSystem/tree/POC/zigbee_coordinator_dev/waveshare/firmware/esp32c6_coordinator). Its `ui/README.md` owns the edit map and test commands; the integration README owns build/flash/start operations and measured results. The branch is local until published.

LVGL9.5.0 uses partial rendering with a logical draw buffer plus separate DMA/wire buffer, each bounded to 320×16 RGB565 pixels (20,480 bytes total), and a fixed72KiB allocator pool. `lv_conf.h` is authoritative on native tests and firmware; ESP-IDF's LV_CONF_SKIP is disabled. Only the UI task calls LVGL, so USB requests touch only the separately locked model. The adapter owns orientation: LVGL stays at rotation0 and changes logical resolution; geometry rotates dirty rectangles and inversely maps touch. This avoids a second LVGL input rotation. DMA storage is reused after completion and retained on timeout; display failure does not stop the NCP.

The editable EEZ project and generated C are tracked together. Normal firmware builds check source/output/version hashes, download/verify the pinned LVGL archive and compile generated code; the desktop editor is not needed. EEZ Flow is disabled and no EEZ runtime is linked. [UI operations](https://github.com/Priyanshu0901/SustaintheSystem/blob/POC/zigbee_coordinator_dev/waveshare/firmware/esp32c6_coordinator/ui/README.md) owns regeneration/test commands, and the [SBOM guide](https://github.com/Priyanshu0901/SustaintheSystem/blob/POC/zigbee_coordinator_dev/waveshare/firmware/esp32c6_coordinator/SBOM.md) owns inventory/licence coverage. Native previews and pointer tests run actual LVGL9.5.0.

## Screen semantics

Network shows host/network status, channel, registered-node count excluding the coordinator, available/stale counts and an explicit unknown remainder. Registered means membership in the host registry, not proof of present RF reachability. Availability comes only from actual Zigbee2MQTT availability publications. With availability disabled or not yet reported, devices remain unknown; a sleepy device is not guessed offline from a short UI timer.

The host sends updates every five seconds. After15 seconds without a valid update, Network labels host data expired and replaces counts/channel with dashes. Radio operation may continue without the host. The arrow-free active page icon, with title in landscape, opens a native dropdown bounded to a ten-pixel gutter for Network, Settings, About and Debug; RayKestrel's logo aligns by its actual image dimensions to the same right gutter. Stable protocol page IDs are Network=0, Debug=1, Settings=2, About=3. Debug reports touch events, uptime, refreshes, draw errors and free heap; LCD/touch readiness stays in host diagnostics rather than a redundant on-screen LCD-ready row. About uses centered portrait rows at a consistent32-pixel pitch and shows maker RayKestrel Systems, ESPC6, the product firmware version and LVGL version. The front page has no project-name banner. No screen changes network channel, joining or network state.

Startup displays the replaceable RayKestrel asset on black for 1.8 seconds or until touched. Conversion runs at build time and its inputs are hashed with the firmware; there is no runtime PNG dependency. The UI task does not delay NCP startup.

Settings offers explicit native dropdown choices for 0/90/180/270-degree clockwise rotation and display sleep off/15/30/60 seconds (default30). A live brightness slider spans10–100% (default100% for older settings); drag samples update brightness without writing NVS, and release commits the value once. Re-selecting a value does not write NVS; periodic snapshots preserve an open user's selection. Local values persist in namespace `display`, separately from host/network state. The compact18-pixel Display sleep info icon retains an expanded invisible touch target and opens transient touch-to-wake/radio-active help. A storage-result toast appears only after actual NVS completion; there is no persistent Saved row. Storage failure does not stop the coordinator. The upstream application owns NVS initialization; the UI never initializes/erases it and retries reads briefly at startup. The board module drives the active-low backlight using inverted10kHz/10-bit PWM. Display sleep turns off panel/backlight and stops LVGL processing/rendering while touch, USB and Zigbee continue. The first touch restores the selected brightness and is suppressed until release; held touch prevents sleep and host updates do not reset the idle timer. Missing touch keeps the display awake. No MCU deep sleep or measured energy claim is implied.

## Bounded USB extension

USB retains ZBOSS framing, CRC, sequence numbers and one serial owner. Three private commands use the existing serialized ZBOSS queue. Their schemas are registered in memory; installed npm files are unchanged. Compatibility is checked against herdsman10.10.0. These are project extensions, not standard ZBOSS APIs.

| Request | Payload | Result |
| --- | --- | --- |
| `0x7F10` status | Nine bytes: version u8, flags u8, channel u8, registered u16, available u16, stale u16 | Validate and update model, return diagnostics |
| `0x7F11` diagnostics | Empty | Read diagnostics without renewing host-data age |
| `0x7F12` last display fault | Empty | Read18-byte stage/error/rectangle/orientation record without renewing host age |

Integers are little-endian. Version is 1. Request flags are network-up bit 0, MQTT-up bit 1, permit-join bit 2. Counts cannot exceed the upstream 200-node capacity, availability counts cannot exceed registered count, and active network channels are 11–26. Invalid payloads leave the model intact and return generic invalid-parameter status.

The fault response is category/status/version/stage (four u8), ESP error u32, x/y/width/height (four u16), adapter rotation u8 and LVGL rotation u8. Stages are0 none,1 geometry validation,2 panel transfer and4 unexpected internal LVGL rotation. It records the first failed draw. The host queries it after a nonzero draw-error count and adds `displayFault` to MQTT diagnostics; older firmware without this optional endpoint remains usable. Original status/diagnostics packet sizes and version stay unchanged.

The 21-byte response contains category/status, UI version, diagnostic flags, page, touch count, frame count, draw-error count, uptime, host-data age and free heap in KiB. Flags indicate panel-ready bit 0, touch-ready bit 1, fresh-host-data bit 2, last-reported network-up bit 3, display-sleeping bit 4 and settings-loaded/saved bit 5. Bits 6–7 encode clockwise rotation in quarter turns. Packet size and version remain unchanged; the added flags were previously unused. Field sizes/serialization are owned by `ui/coordinator_ui.h`, `ui/ui_runtime.c` and the host schema. MQTT `bridge/display` exposes timestamped non-secret diagnostics, including displaySleeping, settingsSaved and rotationDegrees. UI update failures do not terminate Zigbee2MQTT; after three failures polling stops and the panel's data expires.

The [manufacturer's pinned example](https://github.com/waveshareteam/ESP32-C6-LCD-1.9/tree/1fed15e32977de11afe1f710a0f5e915bfcc35a5/02_Example/ESP-IDF/07_LVGL_Test) and [touch-board schematic](https://files.waveshare.com/wiki/ESP32-C6-LCD-1.9/ESP32-C6-Touch-LCD-1.9-Schematic.pdf) establish the board mapping. [Board profiles](Board%20Profiles.md) owns pins and isolation; [architecture](Architecture.md) shows the complete implemented path.
