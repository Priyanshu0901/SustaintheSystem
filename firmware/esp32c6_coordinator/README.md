# Waveshare ESP32-C6 coordinator

This branch builds one board: **Waveshare ESP32-C6-Touch-LCD-1.9**, USB ZBOSS NCP with LCD/touch status UI. The identified board is COM14, MAC `ac:eb:e6:39:fe:ec`, ESP32-C6 rev0.2, 8 MB detected flash, security disabled. Re-identify the port after reconnecting; PCB revision has not been read.

The UI lives in [ui/](ui/README.md); hardware pins stay in [boards/waveshare_touch_1_9/](boards/waveshare_touch_1_9/board_pins.h). The [architecture](../../docs/Firmware/ESP32-C6%20Coordinator/Architecture.md) explains implemented code. [Code map](../../docs/Firmware/ESP32-C6%20Coordinator/Code%20Reference.md) and [Doxygen tooling](../../tools/documentation/README.md) own code navigation.

## Versions live in Git

| Version | Git reference |
| --- | --- |
| Validated XIAO external-antenna coordinator | Tag `coordinator-xiao-v1` at `8506d72` |
| First validated Waveshare UI implementation | Commit `6ac8bb0` |
| Integrated coordinator/node/Waveshare work | Branch `POC_main`; coordinator branch `POC/zigbee_coordinator` |
| Pristine Nordic baseline | Branch `POC/zigbee_nrf` at `d644be9` |

There is no board-selection argument on this branch. For XIAO, save current changes and check out its tag, then use the instructions **from that checkout**:

```powershell
git switch --detach coordinator-xiao-v1
```

Git stores firmware/configuration versions. Ignored network keys/databases remain local and are not restored by checkout. Before running the old XIAO host, stop the Waveshare host and remove only `host/data/external_extensions/coordinator-display.cjs`; leave other local state intact. Coordinator network migration is not established by a Git switch.

## Build

From this directory, in PowerShell 7 with Docker's Linux engine running:

```powershell
./Build-Firmware.ps1
```

The active [sdkconfig.defaults](sdkconfig.defaults) selects 8 MB flash, framed USB NCP traffic and no parallel UART/text logs. The user reports modifying the Waveshare PCB antenna path to use the same external antenna as XIAO. That hardware routing is separate from firmware; no XIAO RF-switch initialization is present in the patched application. Historical profiles and the UI enable flag are absent from this branch.

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

Frontend: [127.0.0.1:8080](http://127.0.0.1:8080). MQTT: `127.0.0.1:1883`, base `sustain/zigbee`, channel 15. Ctrl+C stops each foreground process. Keep joining closed outside intentional pairing. No service/autostart is configured. Keys/database/logs remain ignored. ZBOSS remains [experimental in Zigbee2MQTT](https://www.zigbee2mqtt.io/guide/adapters/zboss.html); do not assume backup/migration support.

## Validation and signal comparison

[validation-waveshare.json](validation-waveshare.json) records the earlier full/app-only flash, formation/resume, display/touch, expiry/reconnect and user-confirmed readable screen/both tabs. That run initially had zero registered nodes. The workflow simplification is built and checked separately; it is not reflashed in this task. [validation.json](validation.json) is historical XIAO evidence, also available at its tag. [Nordic results](../nordic_zigbee/README.md) record the prior XIAO single-node experiment.

The user reports Waveshare improving from **LQI 154 to 198 after its PCB antenna-path modification**, using the **same external antenna as XIAO**; XIAO was approximately 254. These are 0–255 link-quality indications, not RSSI in dBm or percentages. The earlier onboard-versus-external explanation does not apply to this modified board. The exact modification has not been inspected by this agent, and the remaining difference from XIAO has not been isolated by a controlled comparison. The [board reference](../../docs/Firmware/ESP32-C6%20Coordinator/Board%20Profiles.md) contains manufacturer routing sources. No firmware adjustment or RF measurement was performed for this correction.

No new RF-range, mesh, physical power-cycle or soak result is claimed. Existing Docker runtime recovery directories `run.pre-coordinator-20261025` and `run.pre-waveshare-20261030` remain outside this repo; they contain runtime-socket remnants, not firmware backups. Do not remove VM disks/settings to address a stale runtime socket.
