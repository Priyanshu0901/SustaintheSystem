# Coordinator UI

This directory owns the Waveshare UI. **LVGL 9.5.0** renders it; **EEZ Studio 0.29.0** is the editable design/code generator. EEZ Flow is disabled. The desktop editor is a development tool and no EEZ runtime is linked into firmware.

| Change | Canonical source |
| --- | --- |
| Portrait widget tree, text, static colours and styles | [editor/coordinator.eez-project](editor/coordinator.eez-project), opened in EEZ Studio |
| Generated C/header files | [generated/](generated/screens.c), regenerated rather than edited |
| Live data, page/settings callbacks, logo binding and landscape layout | [ui_view.c](ui_view.c) |
| Host snapshot validation, page IDs and idle/wake state | [ui_model.c](ui_model.c), [ui_model.h](ui_model.h) |
| Dirty-rectangle packing and inverse touch transforms | [ui_geometry.c](ui_geometry.c), [ui_geometry.h](ui_geometry.h) |
| Sole LVGL task, display/input callbacks, NVS and diagnostics | [ui_runtime.c](ui_runtime.c) |
| LVGL memory, fonts, widgets and runtime features | [lv_conf.h](lv_conf.h) |
| Exact LVGL release/source hash and EEZ version | [lvgl.lock.json](lvgl.lock.json) |
| Replaceable logo and image conversion | [assets/splash.json](assets/splash.json), [generate_splash.py](generate_splash.py) |
| Board pins, panel/backlight and touch bus | [board driver](../boards/waveshare_touch_1_9/board_display.c), [pin map](../boards/waveshare_touch_1_9/board_pins.h) |

## Edit and regenerate

Open `editor/coordinator.eez-project` in **EEZ Studio 0.29.0**, then save. It targets **LVGL 9.5.0**, the version offered by that editor, matching the actual firmware library. Keep Flow disabled and the output folder `../generated`. Run from the integration directory:

```powershell
./ui/Generate-UI.ps1
```

The default executable is the isolated local copy under `.tools/eez/app/`. To use an installed copy:

```powershell
./ui/Generate-UI.ps1 -EezExecutable 'C:/Path/To/EEZ Studio.exe'
```

The wrapper checks the editor version and successful generation, then records canonical UTF-8/LF project/output hashes in `generated/generation.json`. Normal Docker builds check those hashes and compile the tracked generated C; they need neither the desktop editor nor a licence token. A project/output mismatch requires regeneration. The directory-local Git attribute permits EEZ's space-only lines in `screens.c`; generated C is kept verbatim. The original portrait design stays editable in EEZ; the binding layer adjusts positions and sizes for landscape. Keep hardware, NVS and host logic outside generated files.

## Implemented screens

The active-page icon opens an LVGL dropdown for **Network**, **Settings**, **Debug** and **About**. A RayKestrel logo sits at the top right. The front page has no project-name banner. Network shows only actual host data, with unknown availability and 15-second expiry. Debug shows touch events, uptime, refreshes, draw errors and free heap; hardware readiness remains available in host diagnostics. About shows **RayKestrel Systems**, the Waveshare device, base MAC and LVGL version.

Settings has explicit dropdown options for 0/90/180/270-degree rotation and display sleep off/15/30/60 seconds (default 30). Re-selecting a value does not write NVS. Selections persist in the separate `display` namespace. Idle turns off panel/backlight and stops LVGL rendering; USB, Zigbee and touch polling continue. First touch wakes only, held touch prevents sleep, and host updates do not reset the idle timer. Missing touch input keeps the display awake.

Startup shows the replaceable original-named PNG for 1.8 seconds or until touch. The dependency-free converter produces alpha-preserving LVGL image descriptors for splash/header/About; replace the asset or edit its JSON filename, then rebuild. No runtime PNG decoder is used.

## Runtime and checks

Only the low-priority UI task calls LVGL (`LV_OS_NONE`). The LVGL allocator has a fixed **72 KiB** pool. A logical draw buffer plus a separate DMA/wire buffer each hold at most 320x16 RGB565 pixels (**20,480 bytes total**). Full portrait is 170x320, landscape 320x170. Native panel addressing and its gap stay unchanged; arbitrary LVGL dirty rectangles rotate in the packing adapter, and touch uses the inverse mapping. DMA storage is reused after completion and retained on timeout. The runtime never sends display logs onto USB NCP framing.

Native tests use the exact locked LVGL source, real widgets, a pointer driver and a memory display, with address/undefined sanitizers. In the pinned Docker image, mount the integration at `/integration` and run:

```sh
python /integration/ui/tests/run.py --output /integration/build/lvgl-ui --previews
```

This validates host/model expiry, display sleep/wake, every pixel/touch transform in four rotations, arbitrary dirty rectangles, real pointer opening of the menu, explicit selection callbacks and generated screens. PPM previews come from LVGL itself with test fixture data. [The host codec test](../host/extensions/test-display.cjs) checks the unchanged framed USB contract using the existing portable Node.

[Operations](../README.md) owns build/flash/host commands and actual hardware results. [SBOM](../SBOM.md) owns package pins, licences and inventory coverage. [Display architecture](../../../docs/Firmware/ESP32-C6%20Coordinator/Display%20UI.md) owns runtime/protocol boundaries. Development remains on `POC/zigbee_coordinator_dev/waveshare`; XIAO is historical tag-only.
