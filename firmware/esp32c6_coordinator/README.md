# Waveshare ESP32-C6 coordinator

This branch builds one board: **Waveshare ESP32-C6-Touch-LCD-1.9**, USB ZBOSS NCP with LCD/touch status UI. The identified board is COM14, MAC `ac:eb:e6:39:fe:ec`, ESP32-C6 rev0.2, 8 MB detected flash, security disabled. Re-identify the port after reconnecting; PCB revision has not been read.

The UI lives in [ui/](ui/README.md); hardware pins stay in [boards/waveshare_touch_1_9/](boards/waveshare_touch_1_9/board_pins.h). The [architecture](../../docs/Firmware/ESP32-C6%20Coordinator/Architecture.md) explains implemented code. [Code map](../../docs/Firmware/ESP32-C6%20Coordinator/Code%20Reference.md) and [Doxygen tooling](../../tools/documentation/README.md) own code navigation.

## Active product

Waveshare is the sole maintained product build. Build, flash and host commands run directly from this checkout, without a board selector or Waveshare tag. Development is on `POC/zigbee_coordinator_dev/waveshare` and returns to `POC_main` through PR review.

The discontinued XIAO build, its commands and validation are preserved only at historical tag `coordinator-xiao-v1`; it is not maintained alongside Waveshare. Git does not restore ignored network keys or device databases.

## Build

From this directory, in PowerShell 7 with Docker's Linux engine running:

```powershell
./Build-Firmware.ps1
```

The active [sdkconfig.defaults](sdkconfig.defaults) selects 8 MB flash, framed USB NCP traffic and no parallel UART/text logs. The user reports modifying the Waveshare PCB antenna path for an external antenna; routing is a hardware choice. The patched application initializes only the active Waveshare board.

[upstream.lock.json](upstream.lock.json) pins [our fork](https://github.com/Priyanshu0901/esp-coordinator) / [tostmann upstream](https://github.com/tostmann/esp-coordinator) at `b292868d1ae5426dc048ee7749d7a710e1ccd789` (1.5.79), ESP-IDF 5.5.1 by Docker digest, target and source timestamp. This is an upstream dependency, not a second maintained board version. Its ignored checkout must be clean. Build glue archives that revision, adds the separate UI/board components and applies [the small integration patch](patches/waveshare-ui.patch) to the container build copy. The protocol core remains upstream-owned.

One existing Docker volume, `sustain-c6-waveshare-touch-usb`, holds compiler intermediates. The build also verifies/downloads the exact LVGL source archive pinned by [ui/lvgl.lock.json](ui/lvgl.lock.json), and compiles the checked-in EEZ output after provenance checks. Outputs go directly into ignored `artifacts/`: `factory.bin`, `app.bin`, effective `sdkconfig`, dependency lock, flash arguments, hash manifest, **CycloneDX SBOM** and licence notices. [SBOM.md](SBOM.md) owns package pins, licence provenance and inventory coverage. A failed container build invalidates the success manifest. Run one build at a time. [CI](../../.github/workflows/coordinator-build.yml) builds this same board; it never flashes hardware.

Normal builds enforce the reviewed [dependencies.lock](dependencies.lock). For an intentional dependency update, move that reviewed lock aside, run `./Build-Firmware.ps1 -ResolveDependencies`, review `artifacts/dependencies.lock`, restore the reviewed lock with the intended changes, then build normally. Resolution artifacts cannot be flashed. Preserve upstream MIT and component license notices. Do not run upstream's automatic version-bump/commit helper.

## Flash

Install the local pinned flash tools once with a real CPython 3.12 executable:

```powershell
./Install-FlashTools.ps1 -Python C:/Path/To/Python312/python.exe
./.tools/esptool/Scripts/python.exe -m esptool --chip esp32c6 --port COM14 flash-id
```

Stop the serial host/monitors before programming. For this initialized board, update the application after building:

```powershell
./Flash-Firmware.ps1 -Port COM14 -ExpectedBaseMac ac:eb:e6:39:fe:ec -AppOnly
```

A full image uses the same command without `-AppOnly`. App-only validates the hashed layout and writes at `0x20000`, retaining NVS/partition data. Initial full programming or a changed layout is a separate operation; `-EraseDevelopmentFlash` deliberately destroys state and cannot combine with app-only. The flasher validates board identity, 8 MB capacity, security, artifact/dependency/local-source hashes and verifies before boot. It does not change eFuses or automatically back up flash. The original development firmware was not backed up under the user's waiver.

## Host and display

Use the existing installation/data directory; no second broker, serial owner or global Node installation is required. Node 22.23.4, Zigbee2MQTT 2.14.2 and Mosquitto 2.1.2 are pinned by [host provenance](host/tool-versions.json) and [npm lock](host/package-lock.json). [Install-Host.ps1](host/Install-Host.ps1) prepares missing tools.

In separate PowerShell terminals:

```powershell
./host/Start-Broker.ps1
```

```powershell
./host/Start-Zigbee2MQTT.ps1 -Port COM14
```

From Git Bash, invoke `.ps1` through `powershell.exe -NoProfile -ExecutionPolicy Bypass -File SCRIPT_PATH`. The Waveshare launcher always installs the [display extension](host/extensions/coordinator-display.cjs) into existing ignored data. It uses the same serialized NCP queue and requires external JavaScript to be enabled in the existing host configuration. Availability comes from actual host publications; absent availability remains unknown. Display values expire after 15 seconds. MQTT `sustain/zigbee/bridge/display` carries diagnostics.

The launcher creates local configuration from [the example](host/configuration.example.yaml) only if absent, preserves it afterward, scopes Git discovery and supplies npm's missing package-local dependency junction for converter reloads. Installed npm source stays unchanged.

The screen uses **LVGL 9.5.0**, matching the version offered by **EEZ Studio 0.29.0**. The arrow-free active-page icon (with title in landscape) at top left opens a bounded dropdown for **Network**, **Settings**, **Debug** and **About**, with a RayKestrel logo at top right. Settings has explicit rotation and display-sleep choices; About identifies **RayKestrel Systems**, ESPC6, product firmware version and LVGL version. Debug shows touch events, uptime, refreshes, draw errors and free heap, without a redundant LCD-ready row. Settings persist in NVS, with transient storage feedback. The Display sleep info icon shows touch-to-wake/radio-active help as a toast. Idle switches the panel/backlight off; first touch wakes only. USB/Zigbee/touch continue; no energy reduction has been measured. [UI operations](ui/README.md) owns EEZ editing/regeneration and native rendering tests.

The boot splash and header/About logos use [the replaceable original-named PNG](ui/assets/raykestrel-symbol-refined-white.png). Replace it or change the filename in [splash.json](ui/assets/splash.json), then rebuild. The standard-library converter preserves aspect ratio and alpha in LVGL image descriptors; no website folder or image package is needed to build. Splash rendering runs in the UI task without holding up coordinator startup.

Frontend: [127.0.0.1:8080](http://127.0.0.1:8080). MQTT: `127.0.0.1:1883`, base `sustain/zigbee`, channel 15. Ctrl+C stops each foreground process. Keep joining closed outside intentional pairing. No service/autostart is configured. Keys/database/logs remain ignored. ZBOSS remains [experimental in Zigbee2MQTT](https://www.zigbee2mqtt.io/guide/adapters/zboss.html); do not assume backup/migration support.

## Validation and signal comparison

[validation-waveshare.json](validation-waveshare.json) separates earlier hardware evidence, the custom UI checkpoint and the current `lvglPort` results. [Nordic results](../nordic_zigbee/README.md) own node experiments. Earlier physical observations do not establish the generated LVGL screen's appearance or gestures.

The 2026-10-31 LVGL 9.5.0 port passed the pinned Docker build, real LVGL pointer/menu tests in all four orientations, address/undefined sanitizers, model/power/dirty-rectangle tests, an eight-byte C6 MAC-buffer regression and actual host codec checks. Portrait/landscape previews were inspected. The app is 1,116,656 bytes with 43% of its partition free; live free heap was 182 KiB. The SBOM passed CycloneDX 1.6 structural schema validation; this is not a vulnerability audit.

App-only programming on COM14 verified at `0x20000` without erase. The existing host resumed channel 15 with joining closed and one registered node. Live diagnostics reported panel/touch ready, zero draw errors, 182 KiB heap and saved display settings loaded before any touch. The network and host updates resumed normally. Popup bounds and all 96 pointer option selections, scroll reachability, header/version and toast lifecycle passed native regression tests. Physical readability, gesture behaviour and persistence after power removal await user observation; power reduction was not measured. The existing host/broker remain running for inspection.

The user reports Waveshare improving from **LQI 154 to 198 after its PCB antenna-path modification**, using the **same external antenna as XIAO**; XIAO was approximately 254. These are 0–255 link-quality indications, not RSSI in dBm or percentages. The earlier onboard-versus-external explanation does not apply to this modified board. The exact modification has not been inspected by this agent, and the remaining difference from XIAO has not been isolated by a controlled comparison. The [board reference](../../docs/Firmware/ESP32-C6%20Coordinator/Board%20Profiles.md) contains manufacturer routing sources. No firmware adjustment or RF measurement was performed for this correction.

No new RF-range, mesh, physical power-cycle or soak result is claimed. Existing Docker runtime recovery directories `run.pre-coordinator-20261025` and `run.pre-waveshare-20261030` remain outside this repo; they contain runtime-socket remnants, not firmware backups. Do not remove VM disks/settings to address a stale runtime socket.
