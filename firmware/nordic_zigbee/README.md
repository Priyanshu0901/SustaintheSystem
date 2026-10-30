# Nordic Zigbee test node

Original nRF Connect SDK/Zephyr application for the **Seeed XIAO nRF54LM20A Sense**. It sends versioned **simulated** observations to the existing ESP32-C6 coordinator. Build either a powered router or sleepy end device from the same sources. No microphone, inference, BLE or measured acoustic results are implemented.

[Architecture, report contract and existing profiles](../../docs/Firmware/Nordic%20Zigbee/Overview.md) explain the portable module boundaries. This README owns setup, build and test operations. [Coordinator operations](../esp32c6_coordinator/README.md) owns starting the broker/host and identifying its serial port.

[Code map](../../docs/Firmware/Nordic%20Zigbee/Code%20Reference.md) · [Generate searchable Doxygen reference](../../tools/documentation/README.md).

## Inputs and provenance

- [build-lock.json](build-lock.json): NCS **3.4.1**, Zigbee R23 add-on **1.4.0**, Seeed board commit and official Linux toolchain image digest. Nordic documents add-on 1.4.0 against NCS **3.4.0**. The 3.4.1 combination is a project integration that requires validation; compilation does not establish vendor certification or hardware compatibility.
- [dependencies.lock.yml](dependencies.lock.yml): frozen revisions for the 53 active SDK projects installed here, plus the SDK manifest repository, Zigbee and Seeed (56 projects). Optional inactive SDK groups are excluded. Both build entry points verify checked-out revisions and Zigbee/Seeed tracked-file cleanliness. Native builds require an otherwise unmodified SDK; they do not scan every SDK worktree for edits or reject untracked files. Docker initializes isolated locked checkouts. No shared SDK manifest/configuration is changed.
- [../xiao_source/](../xiao_source/) supplies known-working board settings: NCS 3.4.1, target `xiao_nrf54lm20a/nrf54lm20a/cpuapp`, and active-low PWM aliases red channel 0/P1.22, blue channel 1/P1.23, green channel 2/P1.24. The pinned Seeed board definition supplies these aliases; the new application disables competing GPIO LED outputs. Application and LED logic are written from scratch.
- [Nordic add-on source](https://github.com/nrfconnect/ncs-zigbee/tree/bc7863e99cbbe121d8755bc49586e977cd46d032): consulted for lifecycle, endpoint, scheduler, sleepy behavior and buffer APIs. Our application logic is original; the dependency remains unmodified. Nordic/DSR license terms apply to the dependency and its precompiled stack; [Seeed source](https://github.com/Seeed-Studio/platform-seeedboards/tree/6a313a19f06994909627250a5e082e637bd34ff6) supplies board support under its own licenses.

## Build on this Windows PC

Use PowerShell 7 from this directory. The existing Nordic toolchain and SDK are reused; the build script restores its process environment afterward. First obtain the pinned add-on (the working copy prepared in this task already has it):

```powershell
git clone https://github.com/nrfconnect/ncs-zigbee.git .deps/zigbee
git -C .deps/zigbee checkout --detach bc7863e99cbbe121d8755bc49586e977cd46d032
./Build-Native.ps1 -Board xiao -Role router
./Build-Native.ps1 -Board xiao -Role sleepy
```

Defaults match the tested reference: SDK `C:/ncs/v3.4.1`, toolchain `C:/ncs/toolchains/4f5b6ad6dd`, Seeed board root `C:/Users/rayv_mini_pc/Documents/platform-seeedboards/zephyr`. Override `-SdkRoot`, `-ToolchainRoot`, `-SeeedRoot`, `-ZigbeeRoot` for another installation with the same pinned inputs. Do not replace a shared SDK checkout to satisfy this lock; create an isolated workspace instead.

Output: `build/native-xiao-router/zephyr/zephyr.hex` and `build/native-xiao-sleepy/zephyr/zephyr.hex`, with `.elf`, `.config` and `zephyr.dts` alongside. Always use a pristine build when switching board or role; the script enforces this. Build directories are separate.

For a future DK, select its actual A/B variant:

```powershell
./Build-Native.ps1 -Board dk-a -Role sleepy
./Build-Native.ps1 -Board dk-b -Role sleepy
```

DK profiles use logs and no XIAO LED pins. These commands prepare development images; verify the actual DK revision and Nordic compatibility matrix before flashing.

## Pinned Docker build

Git, PowerShell 7 and Docker's Linux engine are required. The official amd64 NCS 3.4.1 toolchain image is pinned by digest; the first run downloads about 1.7 GB of compressed image layers plus the locked SDK sources. Dependencies/build intermediates stay in the named `sustain-nordic-sdk-341` volume; source is mounted read-only. Run one build at a time against this volume.

```powershell
./Build-Docker.ps1 -Board xiao -Role router
./Build-Docker.ps1 -Board xiao -Role sleepy
```

Outputs go to `artifacts/BOARD-ROLE/`, including firmware, effective config/device tree and SHA-256 manifest. A failed rebuild invalidates the prior success manifest. Treat outputs without a fresh manifest as incomplete. Pinning enables repeatable inputs; byte-identical native/Linux binaries are not claimed. Docker execution status is recorded in [validation.json](validation.json); an authored script alone is not a completed container build.

For isolated native SDK setup, use an NCS 3.4.1 toolchain terminal and a new directory: copy `dependencies.lock.yml` to `manifest/west.yml`, run `west init -l manifest`, then `west update --narrow --fetch-opt=--depth=1`. Supply that workspace to the native script, with `-SeeedRoot WORKSPACE/seeed/zephyr -ZigbeeRoot WORKSPACE/zigbee`. Do not run this over the existing shared NCS workspace.

## Programming and logs

These are **direct SWD development images starting at address 0**. The XIAO SED image has been programmed and independently read back on 2026-10-25. Images replace the Seeed bootloader layout: application 1992 KiB, Zephyr settings 8 KiB, Zigbee NVRAM 32 KiB, product configuration 4 KiB. No MCUboot/USB DFU or OTA update path is provided. The native build must select `CONFIG_USE_DT_CODE_PARTITION=y` so the linker respects the application boundary. Keep this layout stable while testing network persistence.

The tested XIAO is Seeed USB VID/PID `2886:0068`, CMSIS-DAP serial **EA5899D5**, UART **COM11**. Its FICR part is `0x054bc20a`, variant `AAB0` (hardware version code B), device ID words `0x0f67bf8a`/`0xfefe7a17`. **COM6 is a separate Nordic device.** Confirm probe/port assignments after reconnecting.

Use the explicit-probe native flasher from this directory:

```powershell
./Flash-Native.ps1 -ProbeSerial EA5899D5 -Role sleepy
```

The script checks artifact hashes/board/role, selects the named CMSIS-DAP probe, reads the LM20A part ID, disables Seeed's automatic erase/recover handler, loads the image with the board's RRAM write-buffer commit, then compares a complete application readback before resetting. It leaves the processor halted if verification fails. It does not erase Zigbee state or program UICR. OpenOCD 0.12.0 at `C:/msys64/ucrt64/bin/openocd.exe` is the tested host tool. Initial `verify_image` attempted a target SRAM CRC helper that faulted; its fallback verified the image, and independent full readback also matched. The script avoids that helper.

J-Link/nrfutil are registered by the board but untested here. Do not use recover/mass erase casually: it destroys existing firmware and network state. To change an already paired board between router and SED, remove it from Zigbee2MQTT and deliberately clear that board's Zigbee network storage with the identified probe before pairing again. This app has no factory-reset button or runtime role switch.

Logs use the board's UART20 (P1.11 TX, P1.10 RX), bridged by the onboard debug adapter to **COM11**, 115200 baud. The application itself does not implement USB CDC. Dim blue means unjoined; dim green means joined. Enqueue success means accepted for scheduling; `Report APS completion status=0` is a transport completion, not proof of MQTT publication. Correlate with host logs and decoded MQTT messages. Zephyr log timestamps can retain a GRTC-derived offset across debug resets; compare report `uptime` and wall-clock capture timestamps for recovery measurements.

## Install and check the decoder

The existing host pins Zigbee2MQTT 2.14.2 and zigbee-herdsman-converters 26.115.1. From this directory:

```powershell
../esp32c6_coordinator/host/.runtime/node-v22.23.4-win-x64/node.exe tests/converter.test.cjs
# With the host stopped, prepare its existing ignored data directory:
New-Item -ItemType Directory -Force ../esp32c6_coordinator/host/data/external_converters
Copy-Item host/sustain-node.cjs ../esp32c6_coordinator/host/data/external_converters/sustain-node.cjs
```

In the existing local `host/data/configuration.yaml`, merge `enable_external_js: true` under its existing `advanced:` mapping, preserving the channel, keys and other settings. [External converters](https://www.zigbee2mqtt.io/advanced/more/external_converters.html) load from the data directory; restart Zigbee2MQTT and verify the converter appears in `sustain/zigbee/bridge/converters`. The hardware test installed this converter and enabled it in the existing local host configuration; these steps are for a fresh host setup.

## Bench sequence: two routers and two SEDs

1. Label boards R1, R2, E1, E2. Record probe/board identity, actual chip revision, role, firmware hash and resulting Zigbee IEEE address. Build R1/R2 with `router`; E1/E2 with `sleepy`. USB power is fine for every board. SED means receiver-off-when-idle; PWM, console and board peripherals make these unsuitable for power measurements.
2. Start the broker and coordinator host with the existing coordinator guide. Preserve the formed channel-15 network. In the frontend enable joining briefly, power one router, and wait for its completed interview/model `SustainNode-v1`. Rename it R1, then close joining. Repeat for R2 and then E1/E2; start routers first.
3. Observe each device topic under `sustain/zigbee/FRIENDLY_NAME`. Expect approximately one newly generated record every 10 seconds: `simulated: true`, role, sequence, uptime, score and direction. Every tenth generated record carries the simulated anomaly flag. Reboot resets sequence/uptime. Check the *new report timestamps*, because Zigbee2MQTT may republish cached state at startup.
4. Collect a 10-minute baseline per board: timestamps, sequence/uptime, role, host link quality and TX completion errors. Loss/coalescing is possible: this is latest-value telemetry with no offline queue. Sequence gaps do not alone prove radio loss; compare generated/enqueue/TX logs. Duplicate MQTT messages and rebooted counters are separate cases.
5. Reset one node without erasing it. It should resume/rejoin without reopening permit-join; verify new reports. Restart Zigbee2MQTT and the coordinator while keeping keys/database; verify recovery and fresh reports. Record actual delay, rather than assigning a guaranteed timeout.
6. Move powered routers and E1/E2 into the intended topology. Check parent and route information with the frontend network map and, where available, packet capture. Power down one router and measure recovery, then restore it. Receiving all four devices beside the coordinator does not establish routed operation; placement/map/LQI alone is weaker evidence than packet capture. SEDs never relay for other nodes.
7. Close permit-join; retain logs/results in ignored build/output locations and a concise measured validation record. Mark separate pass/fail for interview, reports, persistence, routed forwarding and parent loss. Later test microphone capture, BLE coexistence and energy in separate stages.

Example decoded test data (not a measured observation):

```json
{"schema_version":1,"simulated":true,"anomaly":true,"sequence":10,"uptime":90,"anomaly_score":0.9,"direction":150,"node_role":"sleepy_end_device"}
```

If joining fails, check permit-join, actual SoC revision, antenna attachment, all-channel scan, stored network identity and coordinator logs. If interview succeeds but reports do not decode, check converter loading, cluster `0xfc00`, attribute `0x0000`, endpoint 10 → coordinator HA endpoint 1, schema version and log completion status. This private cluster/endpoint contract is for this bench host; other ecosystems may require a different destination or binding strategy.

## Validation

[validation.json](validation.json) records build checks and the **2026-10-25 single-node hardware test**:

- XIAO **sleepy** image: 292796 B flash / 51192 B RAM; programming plus full binary readback passed. Router image compiles but has not been tested on hardware.
- Channel-15 interview/configuration passed as `SustainNode-v1`, EndDevice, IEEE **0xf4ce36177afefe8a**. Decoder is installed in the local host.
- **61 distinct reports** across 10 minutes 24.775 seconds, including deliberate resets/restarts. All payload fields matched the simulated producer; all 61 UART TX completions reported status 0. No sequence gaps within the three boot episodes. Two cached host-startup republishes were excluded.
- Initial uninterrupted baseline: 32 reports over 310.407 seconds, median interval 10.011 seconds. Reflash/reset and a separate node-only debug reset retained pairing with joining closed; network rejoin followed application startup in 9.138 and 9.156 seconds respectively.
- First host restart exposed an external-converter `EEXIST` junction defect in the npm layout. The [native host launcher](../esp32c6_coordinator/host/Start-Zigbee2MQTT.ps1) now supplies the missing package-local dependency junction. Repeating the restart loaded the converter cleanly and produced a fresh report 8.638 seconds after the request. Upstream package/firmware source was not patched.

The board remains running. The local broker and Zigbee2MQTT remain running for inspection at [127.0.0.1:8080](http://127.0.0.1:8080); topic `sustain/zigbee/0xf4ce36177afefe8a`. Joining is closed. UART/MQTT capture processes have stopped, freeing COM11; no service/autostart was installed. These are single-hop, USB-powered development checks. Physical power cycling, router/routed topology, parent loss, RF range, long-duration soak, power, Docker/DK builds, acoustic and BLE coexistence remain untested.

Capture helpers retain timestamped JSONL only in ignored `build/`. From this directory, clear previous stop markers, then run the UART and MQTT commands in separate terminals:

```powershell
Remove-Item -LiteralPath build/uart.stop,build/mqtt.stop -ErrorAction SilentlyContinue
C:/ncs/toolchains/4f5b6ad6dd/opt/bin/python.exe tools/serial_monitor.py --port COM11 --output build/uart-hardware.jsonl --stop-file build/uart.stop --seconds 1200
../esp32c6_coordinator/host/.runtime/node-v22.23.4-win-x64/node.exe tools/mqtt_monitor.cjs build/mqtt-hardware.jsonl build/mqtt.stop 1200
```

Create the corresponding stop file to end a capture, or let its duration expire. Helpers append to existing logs; choose fresh output names for a separate experiment. `tools/summarize_hardware.py build/mqtt-hardware.jsonl --output build/hardware-summary.json` validates simulated fields and summarizes per-boot continuity, treating repeated sequence/uptime as cached duplicates. It is a bench-data checker, not a general packet-loss measurement.
