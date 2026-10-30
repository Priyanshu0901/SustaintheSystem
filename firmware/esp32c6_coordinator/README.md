# ESP32-C6 coordinator and Waveshare UI

USB ZBOSS NCP firmware from [our fork](https://github.com/Priyanshu0901/esp-coordinator), with [tostmann](https://github.com/tostmann/esp-coordinator) as upstream. [upstream.lock.json](upstream.lock.json) pins revision `b292868d1ae5426dc048ee7749d7a710e1ccd789` / version `1.5.79`, ESP-IDF 5.5.1 by digest, target and source timestamp. The checkout stays clean. Only Waveshare receives the separate UI/board components and reviewed hook patch in its build copy.

The upstream application is MIT-licensed and uses separately licensed Espressif/ZBOSS components. Preserve their license notices; the application license does not replace dependency terms. Upstream test descriptions are source-review evidence, not tests performed on our boards.

Current board, identified 2026-10-30: **Waveshare ESP32-C6-Touch-LCD-1.9 on COM14**, ESP32-C6 rev0.2, detected 8 MB flash, MAC `ac:eb:e6:39:fe:ec`, secure boot/encryption disabled. esptool names the package ESP32-C6FH4; detected flash capacity is 8 MB. The PCB marking has not been read. Earlier XIAO was COM12 / 4 MB / MAC `e4:b3:23:b4:a4:10`; it is not currently connected. Re-identify ports after reconnecting.

UI design lives in [ui/](ui/README.md): `ui_theme.h` owns colors, `ui_view.c` layout/text and `ui_model.c` tabs/freshness. [Board pins](boards/waveshare_touch_1_9/board_pins.h) stay separate. Work is on `codex/waveshare-coordinator-ui`, with one host installation/data directory. Keys/device state stay ignored; Git branches do not preserve runtime pairing.

Existing architecture, upstream comparison, board profiles and host semantics live in [Zigbee Coordinator](../../docs/Firmware/ESP32-C6%20Coordinator/Overview.md). This README owns operational commands. ZBOSS support remains [experimental in Zigbee2MQTT](https://www.zigbee2mqtt.io/guide/adapters/zboss.html).

[Hardware status](../../docs/Firmware/ESP32-C6%20Coordinator/Board%20Profiles.md#hardware-status) records the ordered touch development kit separately from the existing profiles below.

[Original integration code map](../../docs/Firmware/ESP32-C6%20Coordinator/Code%20Reference.md) · [Generate searchable Doxygen reference](../../tools/documentation/README.md).

## Build in Docker

Prerequisites: Git, PowerShell 7, and a running Docker Linux engine. No native ESP-IDF or USB passthrough is required. From this directory:

```powershell
./Build-Firmware.ps1 -Profile waveshare-touch-usb
./Build-Firmware.ps1 -Profile xiao-ext-usb
./Build-Firmware.ps1 -Profile generic-usb
```

The script clones the pinned fork into ignored `source/` on a clean checkout, registers `upstream`, requires the correct revision and a clean tree, and archives committed files. Existing source checkouts with mismatched remotes/revision or uncommitted files are rejected. Each profile has its own Docker volume (`sustain-c6-PROFILE`) and `artifacts/PROFILE/` directory. Outputs include `factory.bin`, `app.bin`, effective `sdkconfig`, dependency lock, flash arguments and a hash manifest. Compilation failures invalidate the container build's success manifest. Do not run concurrent builds against this same integration directory.

- `waveshare-touch-usb`: 8 MB, onboard antenna, LCD/touch/UI enabled, XIAO RF GPIO handling excluded and UART disabled. Runtime source/patch hashes are in the manifest; no added library dependency tree.
- `xiao-ext-usb`: XIAO external antenna, 4 MB, USB NCP and UART disabled; only this profile selects the XIAO RF switch.
- `generic-usb`: comparison build, external antenna disabled, USB NCP, UART disabled. Upstream still resets GPIO3/14 as inputs. **Not qualified for the Waveshare display board.**

[profiles/](profiles/) owns overlays; [tools/build.py](tools/build.py) runs in the official image. [CI](../../.github/workflows/coordinator-build.yml) builds all three profiles after publication. It never flashes hardware and has not run remotely for this work.

For an intentional dependency update only:

```powershell
# Temporarily move the existing reviewed lock aside before resolving a different set.
./Build-Firmware.ps1 -Profile xiao-ext-usb -ResolveDependencies
# Review artifacts/xiao-ext-usb/dependencies.lock, then replace dependencies.lock.
# A resolution build cannot be flashed by Flash-Firmware.ps1.
./Build-Firmware.ps1 -Profile xiao-ext-usb
```

Normal builds require the checked-in [dependencies.lock](dependencies.lock) and reject a build that changes it. The initial resolution establishes that file. Review source/dependency/toolchain updates together and repeat hardware checks. `origin` is the fork; `upstream` is the maintained parent. Do not use upstream's version-bump helper: it can automatically stage/commit files. Controlled inputs do not alone prove bit-for-bit reproducibility.

## Identify and flash on Windows

Install the pinned local flash tools once using a real CPython 3.12 executable (replace the path):

```powershell
./Install-FlashTools.ps1 -Python C:/Path/To/Python312/python.exe
./.tools/esptool/Scripts/python.exe -m esptool --chip esp32c6 --port COM14 flash-id
./.tools/esptool/Scripts/python.exe -m esptool --chip esp32c6 --port COM14 get-security-info
```

Close Zigbee2MQTT, browser flashers and serial monitors. Initial full-image flash on Waveshare:

```powershell
./Flash-Firmware.ps1 -Profile waveshare-touch-usb -Port COM14 -ExpectedBaseMac ac:eb:e6:39:fe:ec
```

For UI edits on this already initialized board/profile, rebuild, then update only the application:

```powershell
./Build-Firmware.ps1 -Profile waveshare-touch-usb
./Flash-Firmware.ps1 -Profile waveshare-touch-usb -Port COM14 -ExpectedBaseMac ac:eb:e6:39:fe:ec -AppOnly
```

App-only validates the hashed layout/offset and writes at `0x20000`, preserving NVS/partition data. It cannot combine with `-EraseDevelopmentFlash`. Full images establish a board or changed layout/toolchain. For XIAO, select its own profile/MAC/port with antenna attached.

The script checks image/dependency/local-source hashes, MAC, chip, profile flash size and security, verifies before boot and records ignored `flash-result.json`. The explicit development-erase flag destroys state; the user's backup waiver applied to initial development bring-up. Full factory writes also overwrite runtime flash even without a whole-chip erase. No automatic backup, eFuse change or security enablement is performed.

Rollback here means rebuild/reflash a previously validated source/profile/dependency/toolchain combination. The original development firmware is not recoverable through this integration because backup was waived. Network recovery/migration is separate and remains unvalidated with the standard host; do not promise paired devices survive a factory flash.

## Run the Windows host

[host/](host/) contains the portable native Windows host prepared earlier: Node 22.23.4, Zigbee2MQTT 2.14.2 and Mosquitto 2.1.2. [Tool provenance](host/tool-versions.json) and [npm lock](host/package-lock.json) pin its dependencies. On a clean checkout, [Install-Host.ps1](host/Install-Host.ps1) needs internet and 7-Zip. No service or system Node update is required.

In separate PowerShell terminals from this directory:

```powershell
./host/Start-Broker.ps1
```

```powershell
./host/Start-Zigbee2MQTT.ps1 -Port COM14 -EnableDisplay
```

From **Git Bash**, explicitly invoke PowerShell in two terminals; Bash cannot parse `.ps1` files directly:

```bash
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ./host/Start-Broker.ps1
```

```bash
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ./host/Start-Zigbee2MQTT.ps1 -Port COM14 -EnableDisplay
```

`-EnableDisplay` installs the opt-in [host extension](host/extensions/coordinator-display.cjs) into existing ignored data. It adds no second serial client or broker. Available/stale counts come from actual Zigbee2MQTT availability publications; unreported devices remain unknown. Enable availability in host settings if those counts are needed. MQTT `sustain/zigbee/bridge/display` exposes timestamped diagnostics. The screen expires host values after 15 seconds. To return to XIAO, remove only `host/data/external_extensions/coordinator-display.cjs`, then launch without `-EnableDisplay`; keep other converters/state intact.

The launcher creates ignored `host/data/configuration.yaml` from [the host example](host/configuration.example.yaml) once and preserves it on later launches. It scopes Git discovery to prevent the npm package from mistaking this project's commit for its own compiled-package version; it restores the working directory and environment on exit. The [serial fragment](configuration.example.yaml) is only a fragment for other hosts. Native Windows uses COM ports; Linux must use the port actually visible there.

For this npm-hoisted Windows install, the launcher also supplies a missing `zigbee2mqtt/node_modules` junction to the actual host dependency root. Zigbee2MQTT's external-converter loader otherwise creates a dangling junction and fails with `EEXIST` on an in-process restart. Package source remains unchanged. The Nordic hardware test reproduced that failure and verified clean converter loading plus fresh reports after the launcher fix and a repeated restart.

Frontend: [127.0.0.1:8080](http://127.0.0.1:8080). MQTT: `127.0.0.1:1883`, base topic `sustain/zigbee`, initial channel 15. Keep permit-join closed until an intentional node test. Generated keys, device database and logs are ignored; never publish them. Ctrl+C stops each foreground process. No service/autostart is configured.

Record firmware version response, formation/resume, restart/reconnect, then a real Nordic node interview and MQTT reports. Those are different checks. A custom Nordic converter may be required. Firmware backup features described by upstream rely on extra host support and do not establish recovery support in this standard host.

## Validation status

On 2026-10-25, both Docker profiles built successfully. The XIAO external-antenna image was flashed and fully verified before boot. Zigbee2MQTT reported revision `1.5.0.79`, formed a channel-15 network, and resumed it after host stop plus coordinator reset. MQTT reported online, permit-join false and zero joined nodes; the frontend returned HTTP 200. The test host/broker were stopped afterward. [validation.json](validation.json) records versions, image hashes and test boundaries. Earlier schema, serial-dependency loading and MQTT QoS 1 loopback checks also passed.

The initial extra verification ran after startup and found changes only in NVS/OTA-selection sectors. The corrected flasher holds the application stopped until full verification completes. Native launcher testing also fixed an enclosing-Git-repository version-detection issue without editing the npm package. Build logs include upstream Kconfig notices (duplicate Zigbee symbols and an obsolete bootloader-console symbol); effective profile checks and compilation pass. Those baseline tests did not include Waveshare display or Nordic commissioning; subsequent results are recorded below. GitHub Actions is authored but has not run remotely.

Single-node Nordic commissioning/reporting passed on XIAO on 2026-10-25; [Nordic operations/results](../nordic_zigbee/README.md) owns those measurements. They do not establish pairing on Waveshare. On 2026-10-30, Waveshare build/full-flash verification, formation/resume, LCD/touch diagnostics and host expiry/reconnect passed. The user confirmed the physical screen and both tabs work. It currently has zero registered nodes, joining closed. [Waveshare validation](validation-waveshare.json) owns current hashes/scope; [validation.json](validation.json) remains the XIAO baseline.

The XIAO regression build reproduces its original factory SHA after UI integration. UI tests cover invalid payloads, freshness/wraparound, tab/tile bounds; host tests use the real serial codec and check unknown availability. No fixture values are compiled into firmware. The host/broker are left running for user inspection, without services/autostart. No Waveshare Nordic re-pairing/report, RF-range, mesh or soak test is claimed.

Docker initially failed on an inaccessible stale `Docker/run/dockerInference` socket. Only the runtime-socket directory was moved aside to `C:/Users/rayv_mini_pc/AppData/Local/Docker/run.pre-coordinator-20261025` after shutdown; VM disks/settings were preserved. The engine then started. That local recovery directory is outside this repo and must not be mistaken for a firmware backup. It may need cleanup after a Windows restart releases the stale sockets.
