# Waveshare status display

The Waveshare ESP32-C6-Touch-LCD-1.9 runs the USB ZBOSS coordinator and a local status display. Project-owned UI code and the board driver remain separate from the upstream protocol core. No Wi-Fi transport is used. [Code reference](Code%20Reference.md) maps the components; [UI operations](https://github.com/Priyanshu0901/SustaintheSystem/blob/POC/zigbee_coordinator_dev/waveshare/firmware/esp32c6_coordinator/ui/README.md) owns the edit map, regeneration and test commands.

## Rendering and task ownership

Only the low-priority UI task calls **LVGL 9.5.0** (`LV_OS_NONE`). USB requests update a separately locked model. The adapter owns pixel rotation and inverse touch mapping; LVGL stays at rotation 0 while its logical resolution changes between 170×320 and 320×170. Native panel addressing and the 35-pixel column gap stay fixed.

Partial rendering uses one logical draw buffer and two alternating DMA/wire buffers, each bounded to 320×32 RGB565 pixels: **61,440 bytes total**, separate from the fixed **72 KiB** LVGL allocator pool. At **40 MHz** SPI, a flush packs into the free wire tile while the previous transfer runs, then yields on a FreeRTOS completion semaphore at handover. Submission returns so LVGL can render the next tile during DMA. The final tile completes before frame accounting, sleep or backlight enable. A timeout/submission failure stops display writes and retains both DMA buffers; the NCP continues. Refresh/input timers use a **20 ms** period. There is no mapped panel TE signal; physical scroll timing and tearing remain unmeasured.

The editable **EEZ Studio 0.29.0** project and generated C are tracked together. Normal builds verify source/output/version hashes and the pinned LVGL archive, then compile the generated code without the desktop editor. EEZ Flow is disabled and no EEZ runtime is linked. [SBOM coverage](https://github.com/Priyanshu0901/SustaintheSystem/blob/POC/zigbee_coordinator_dev/waveshare/firmware/esp32c6_coordinator/SBOM.md) records pins and licence scope.

## Host data and screen state

Network shows host/network status, channel and registered-node counts excluding the coordinator. Registered means membership in the host registry, not proven RF reachability. Available/stale counts come from actual Zigbee2MQTT availability publications; the remainder stays unknown. A sleepy device is not guessed offline from a short display timer.

The host sends updates every five seconds. After **15 seconds** without a valid update, host data expires and counts/channel become dashes. Radio operation may continue. No screen changes channel, joining or network state.

The active-page dropdown orders Network, Settings, About and optional Debug under a fixed 48-pixel header. Bodies scroll vertically; scrollbars appear only during scrolling. Protocol page IDs remain Network=0, Debug=1, Settings=2, About=3 regardless of menu order. Debug shows touch events, uptime, frames, draw errors and free heap. About shows RayKestrel Systems, ESPC6, product firmware version and LVGL version. Layout/control details belong to the UI operations guide.

The replaceable splash displays for 1.8 seconds or until touched. Saved orientation and dimensions are applied before screen creation; boot/wake completes a full frame through DMA before enabling the backlight. Image conversion happens at build time, with inputs hashed into the firmware manifest. Splash work does not delay NCP startup.

## Local preferences and power

Settings persists clockwise rotation (0/90/180/270 degrees), display sleep (off/15/30/60 seconds; default 30) and brightness (10–100%; default 100%) in the separate `display` NVS namespace. Brightness dragging updates PWM immediately; release persists once. Unchanged selections do not write NVS. Successful saves stay silent; failures show an error that a later success clears. The upstream application initializes NVS; the UI retries reads briefly and never erases it.

The board module drives active-low backlight PWM at 10 kHz/10-bit. Display sleep turns off panel/backlight and suspends LVGL processing/rendering while touch, USB and Zigbee continue. First touch restores brightness and is suppressed until release. Held touch prevents sleep; host updates do not reset the idle timer. Missing touch input keeps the display awake. This is display saving, with no measured energy result.

Reset to defaults writes only rotation 0°, sleep 30 seconds and brightness 100%, then returns Settings to the top. Restart queues `esp_restart()` outside the LVGL callback after pending persistence. Neither action erases network state. Native action tests do not establish physical button behaviour or restart/host recovery.

## Bounded USB extension

USB retains ZBOSS framing, CRC, sequence numbers and one serial owner. Three project commands use the existing serialized queue. Schemas are registered in memory without changing installed npm files; compatibility was checked against zigbee-herdsman 10.10.0.

| Request | Payload | Result |
| --- | --- | --- |
| `0x7F10` status | Nine bytes: version u8, flags u8, channel u8, registered u16, available u16, stale u16 | Validate/update model; return 21-byte diagnostics |
| `0x7F11` diagnostics | Empty | Return diagnostics without renewing host-data age |
| `0x7F12` last display fault | Empty | Return 18-byte fault record without renewing host-data age |

Integers are little-endian; protocol version is 1. Request flags are network-up bit 0, MQTT-up bit 1 and permit-join bit 2. Registered count cannot exceed 200; available plus stale cannot exceed registered. Active network channels are 11–26. Invalid status payloads leave the model intact and return invalid-parameter status.

The 21-byte status/diagnostics response contains category/status (two u8), version/flags/page (three u8), touches u16, frames u32, draw errors u16, uptime seconds u32, host age seconds u16 and free heap KiB u16. Flags encode panel-ready bit 0, touch-ready bit 1, fresh-host-data bit 2, last-reported network-up bit 3, display-sleeping bit 4 and settings-loaded/saved bit 5. Bits 6–7 encode clockwise rotation in quarter turns.

The fault response contains category/status/version/stage (four u8), ESP error u32, x/y/width/height (four u16), adapter rotation u8 and LVGL rotation u8. Stages are 0 none, 1 geometry validation, 2 panel transfer and 4 unexpected internal LVGL rotation. It records the first failed draw. The host queries it after a nonzero draw-error count and adds `displayFault` to MQTT diagnostics; older firmware without this optional endpoint remains usable.

Serialization is implemented in `ui/ui_runtime.c` and the host schema. MQTT `bridge/display` exposes timestamped diagnostics including displaySleeping, settingsSaved and rotationDegrees. After three update failures, display polling stops and the panel's host data expires; Zigbee2MQTT continues.

[Board profiles](Board%20Profiles.md) owns pins and manufacturer references. [Architecture](Architecture.md) shows the implemented integration. [Firmware operations and validation](https://github.com/Priyanshu0901/SustaintheSystem/tree/POC/zigbee_coordinator_dev/waveshare/firmware/esp32c6_coordinator#validation-and-signal-comparison) owns measured results and remaining physical checks.
