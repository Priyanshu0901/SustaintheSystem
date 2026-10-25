# XIAO ESP32-C6 coordinator integration

USB ZBOSS NCP firmware from [our fork](https://github.com/Priyanshu0901/esp-coordinator), with [tostmann](https://github.com/tostmann/esp-coordinator) as maintained upstream. [upstream.lock.json](upstream.lock.json) pins revision `b292868d1ae5426dc048ee7749d7a710e1ccd789` / version `1.5.79`, ESP-IDF 5.5.1 by Docker digest, target and source timestamp. The source checkout stays clean; profiles configure upstream without a firmware-core patch.

The connected development XIAO was identified as **COM12**, ESP32-C6FH4 rev0.1, 4 MB flash, base MAC `e4:b3:23:b4:a4:10`. COM6 is a Nordic device. Secure boot/encryption are disabled. The user explicitly waived backup for this board. Do not assume COM12 remains assigned after reconnecting.

Full design, upstream comparison, host semantics and Waveshare migration guidance live in [Zigbee Coordinator](../../docs/Hardware/Zigbee%20Coordinator.md). This README owns operational commands. ZBOSS support remains [experimental in Zigbee2MQTT](https://www.zigbee2mqtt.io/guide/adapters/zboss.html).

## Build in Docker

Prerequisites: Git, PowerShell 7, and a running Docker Linux engine. No native ESP-IDF or USB passthrough is required. From this directory:

```powershell
./Build-Firmware.ps1 -Profile xiao-ext-usb
./Build-Firmware.ps1 -Profile generic-usb
```

The script clones the pinned fork into ignored `source/` on a clean checkout, registers `upstream`, requires the correct revision and a clean tree, and archives committed files. Existing source checkouts with mismatched remotes/revision or uncommitted files are rejected. Each profile has its own Docker volume (`sustain-c6-PROFILE`) and `artifacts/PROFILE/` directory. Outputs include `factory.bin`, `app.bin`, effective `sdkconfig`, dependency lock, flash arguments and a hash manifest. Compilation failures invalidate the container build's success manifest. Do not run concurrent builds against this same integration directory.

- `xiao-ext-usb`: XIAO external antenna enabled through upstream GPIO3/14 handling, 4 MB flash, USB NCP; parallel NCP UART disabled.
- `generic-usb`: comparison build, external antenna disabled, USB NCP, UART disabled. Upstream still resets GPIO3/14 as inputs. **Not qualified for the Waveshare display board.**

[profiles/](profiles/) owns board overlays; [tools/build.py](tools/build.py) runs in the official image. [CI workflow](../../.github/workflows/coordinator-build.yml) builds both profiles on Linux using this same entry point and uploads firmware artifacts; it runs after these project files are committed/pushed, or through manual dispatch once published. It never flashes hardware. Local validation does not imply a GitHub Actions run.

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
./.tools/esptool/Scripts/python.exe -m esptool --chip esp32c6 --port COM12 flash-id
./.tools/esptool/Scripts/python.exe -m esptool --chip esp32c6 --port COM12 get-security-info
```

Close Zigbee2MQTT, browser flashers and serial monitors. With the external antenna attached, flash the identified development board:

```powershell
./Flash-Firmware.ps1 -Port COM12 -ExpectedBaseMac e4:b3:23:b4:a4:10 -EraseDevelopmentFlash
```

The script checks artifact hashes, reviewed dependencies, board MAC, ESP32-C6 identity, 4 MB flash and security state. The explicit erase option destroys existing firmware/network state; it is appropriate for this user-designated disposable development board. Without it, writes still replace firmware and portions of flash. There is no automatic backup. It writes the merged factory image at `0x0`, verifies flash, and saves ignored `flash-result.json`. It does not alter eFuses or enable security. Treat a flashing failure as incomplete bring-up.

Rollback here means rebuild/reflash a previously validated source/profile/dependency/toolchain combination. The original development firmware is not recoverable through this integration because backup was waived. Network recovery/migration is separate and remains unvalidated with the standard host; do not promise paired devices survive a factory flash.

## Run the Windows host

[host/](host/) contains the portable native Windows host prepared earlier: Node 22.23.4, Zigbee2MQTT 2.14.2 and Mosquitto 2.1.2. [Tool provenance](host/tool-versions.json) and [npm lock](host/package-lock.json) pin its dependencies. On a clean checkout, [Install-Host.ps1](host/Install-Host.ps1) needs internet and 7-Zip. No service or system Node update is required.

In separate PowerShell terminals from this directory:

```powershell
./host/Start-Broker.ps1
```

```powershell
./host/Start-Zigbee2MQTT.ps1 -Port COM12
```

The launcher creates ignored `host/data/configuration.yaml` from [the host example](host/configuration.example.yaml) once and preserves it on later launches. It scopes Git discovery to prevent the npm package from mistaking this project's commit for its own compiled-package version; it restores the working directory and environment on exit. The [serial fragment](configuration.example.yaml) is only a fragment for other hosts. Native Windows uses COM ports; Linux must use the port actually visible there.

For this npm-hoisted Windows install, the launcher also supplies a missing `zigbee2mqtt/node_modules` junction to the actual host dependency root. Zigbee2MQTT's external-converter loader otherwise creates a dangling junction and fails with `EEXIST` on an in-process restart. Package source remains unchanged. The Nordic hardware test reproduced that failure and verified clean converter loading plus fresh reports after the launcher fix and a repeated restart.

Frontend: [127.0.0.1:8080](http://127.0.0.1:8080). MQTT: `127.0.0.1:1883`, base topic `sustain/zigbee`, initial channel 15. Keep permit-join closed until an intentional node test. Generated keys, device database and logs are ignored; never publish them. Ctrl+C stops each foreground process. No service/autostart is configured.

Record firmware version response, formation/resume, restart/reconnect, then a real Nordic node interview and MQTT reports. Those are different checks. A custom Nordic converter may be required. Firmware backup features described by upstream rely on extra host support and do not establish recovery support in this standard host.

## Validation status

On 2026-10-25, both Docker profiles built successfully. The XIAO external-antenna image was flashed and fully verified before boot. Zigbee2MQTT reported revision `1.5.0.79`, formed a channel-15 network, and resumed it after host stop plus coordinator reset. MQTT reported online, permit-join false and zero joined nodes; the frontend returned HTTP 200. The test host/broker were stopped afterward. [validation.json](validation.json) records versions, image hashes and test boundaries. Earlier schema, serial-dependency loading and MQTT QoS 1 loopback checks also passed.

The initial extra verification ran after startup and found changes only in NVS/OTA-selection sectors. The corrected flasher holds the application stopped until full verification completes. Native launcher testing also fixed an enclosing-Git-repository version-detection issue without editing the npm package. Build logs include upstream Kconfig notices (duplicate Zigbee symbols and an obsolete bootloader-console symbol); effective profile checks and compilation pass. No Waveshare display, Nordic commissioning, routed mesh, RF-range, backup/restore or long-duration stability result is implied. GitHub Actions is authored but has not run remotely.

Subsequent single-node Nordic commissioning and simulated MQTT reporting passed on 2026-10-25; [Nordic operations/results](../nordic_zigbee/README.md) owns those measurements and the host restart correction. The original coordinator-only record above remains the formation/resume baseline. Current local test host/broker are running with one paired XIAO SED and joining closed; no Windows service/autostart was added.

Docker initially failed on an inaccessible stale `Docker/run/dockerInference` socket. Only the runtime-socket directory was moved aside to `C:/Users/rayv_mini_pc/AppData/Local/Docker/run.pre-coordinator-20261025` after shutdown; VM disks/settings were preserved. The engine then started. That local recovery directory is outside this repo and must not be mistaken for a firmware backup. It may need cleanup after a Windows restart releases the stale sockets.
