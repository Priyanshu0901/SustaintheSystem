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

One existing Docker volume, `sustain-c6-waveshare-touch-usb`, holds compiler intermediates. Outputs go directly into ignored `artifacts/`: `factory.bin`, `app.bin`, effective `sdkconfig`, dependency lock, flash arguments and hash manifest. A failed build invalidates the success manifest. Run one build at a time. [CI](../../.github/workflows/coordinator-build.yml) builds this same board; it never flashes hardware.

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

On the screen, the top-left hamburger opens icon rows for **Network**, **Settings** and **Debug**. Network shows host status and node counts without a project-name banner. Debug holds touch/frame/error counters and board health. Settings cycles rotation through 0/90/180/270 degrees and display saving through off/15/30/60 seconds (30 seconds by default). Settings persist in NVS. When idle, the panel/backlight turn off; the first touch only wakes it. Zigbee, USB and touch polling continue. This saves display power, not whole-device sleep; no energy reduction has been measured.

The boot splash uses [the replaceable original-named PNG](ui/assets/raykestrel-symbol-refined-white.png). Replace that asset, or change the filename in [splash.json](ui/assets/splash.json), then rebuild. The standard-library build converter preserves aspect ratio, composites onto black and emits a small RGB565 array; no website folder or image package is needed to build. Splash rendering runs in the UI task without holding up coordinator startup.

Frontend: [127.0.0.1:8080](http://127.0.0.1:8080). MQTT: `127.0.0.1:1883`, base `sustain/zigbee`, channel 15. Ctrl+C stops each foreground process. Keep joining closed outside intentional pairing. No service/autostart is configured. Keys/database/logs remain ignored. ZBOSS remains [experimental in Zigbee2MQTT](https://www.zigbee2mqtt.io/guide/adapters/zboss.html); do not assume backup/migration support.

## Validation and signal comparison

[validation-waveshare.json](validation-waveshare.json) separates earlier hardware evidence from the new UI validation. The earlier run established full/app-only flashing, formation/resume, display/touch and expiry/reconnect; it initially had zero registered nodes. [Nordic results](../nordic_zigbee/README.md) own node experiments. Physical observations from the old two-tab UI do not establish the new menu, rotation, settings persistence or display-saving behaviour.

The 2026-10-31 UI passed the pinned Docker build, sanitizer-backed native navigation/power/rotation tests, actual host codec tests and inspected portrait/landscape previews. App-only programming on COM14 verified at `0x20000` without erase. The existing host resumed channel 15 with joining closed and one registered node. Live diagnostics reported panel/touch ready, zero draw errors, display sleeping and unchanged frame count across fresh host updates. Physical readability, rotation, wake gestures and settings persistence await user observation; power reduction was not measured. The existing host/broker remain running for inspection.

The user reports Waveshare improving from **LQI 154 to 198 after its PCB antenna-path modification**, using the **same external antenna as XIAO**; XIAO was approximately 254. These are 0–255 link-quality indications, not RSSI in dBm or percentages. The earlier onboard-versus-external explanation does not apply to this modified board. The exact modification has not been inspected by this agent, and the remaining difference from XIAO has not been isolated by a controlled comparison. The [board reference](../../docs/Firmware/ESP32-C6%20Coordinator/Board%20Profiles.md) contains manufacturer routing sources. No firmware adjustment or RF measurement was performed for this correction.

No new RF-range, mesh, physical power-cycle or soak result is claimed. Existing Docker runtime recovery directories `run.pre-coordinator-20261025` and `run.pre-waveshare-20261030` remain outside this repo; they contain runtime-socket remnants, not firmware backups. Do not remove VM disks/settings to address a stale runtime socket.
