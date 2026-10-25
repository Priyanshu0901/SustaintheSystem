# Zigbee coordinator

The first coordinator is a Seeed XIAO ESP32-C6 with an attached external antenna, connected over USB to the Windows development PC. The PC runs Zigbee2MQTT and a local MQTT broker. The firmware is a network coprocessor (NCP): it handles the Zigbee radio/network while the host handles device definitions, reports and application state.

```text
Nordic Zigbee nodes <-> XIAO ESP32-C6 <-> USB <-> Zigbee2MQTT <-> MQTT <-> monitoring client
```

The four available XIAO nRF54LM20A Sense boards are intended for a two-router/two-sleepy-end-device trial. Monitoring nodes retain the sleepy-end-device role. Network formation on the coordinator alone does not validate routed delivery, Nordic commissioning or acoustic reports.

Validated 2026-10-25: both Docker profiles built; the XIAO external-antenna image passed write/full-image verification; Zigbee2MQTT 2.14.2 formed a channel-15 network and resumed it after host stop and coordinator reset. MQTT reported online with joining closed and zero joined nodes; the frontend returned HTTP 200. Temporary host/broker processes were stopped after testing. This establishes initial bring-up, not the later device, range or stability tests.

## Firmware parent assessment

Reviewed 2026-10-25. Keep [tostmann/esp-coordinator](https://github.com/tostmann/esp-coordinator) as the maintained upstream for this **USB ZBOSS NCP evaluation**, using [our fork](https://github.com/Priyanshu0901/esp-coordinator). The evaluated revision is `b292868d1ae5426dc048ee7749d7a710e1ccd789`, version `1.5.79`.

| Candidate | Evidence | Fit for this project |
| --- | --- | --- |
| [andryblack/esp-coordinator](https://github.com/andryblack/esp-coordinator) | Archived 2026-04-02; README identifies it as a proof of concept and points to tostmann | Preserve ancestry; do not base new work on its frozen implementation |
| [tostmann/esp-coordinator](https://github.com/tostmann/esp-coordinator) | Recent source fixes and release evidence; explicit XIAO antenna configuration; named by Zigbee2MQTT's adapter guide | Selected for the existing host protocol; validate locally before relying on it |
| Other recent forks | BragiOk/master matched selected master; aliciusschroeder/master was nine commits behind during comparison | No stronger parent identified in these comparisons; this is not an exhaustive firmware audit |
| [Espressif ESP-Zigbee-SDK](https://github.com/espressif/esp-zigbee-sdk) | Official SDK; v1.x is ZBOSS maintenance/LTS, v2.x uses Espressif's stack and is recommended upstream for new designs | Evaluate for a future standalone controller. SDK maintenance alone does not establish a compatible Zigbee2MQTT ZBOSS serial endpoint |

The chosen parent is MIT-licensed application code using separately licensed prebuilt Espressif/ZBOSS components. Preserve their notices. Source inspection found transport/reset fixes, release validation descriptions and targeted diagnostic scripts; the repository has no tracked GitHub build workflow at the reviewed revision. Maintainer test descriptions are upstream evidence, not tests performed on our hardware. The project's own CI builds both selected profiles.

[Zigbee2MQTT still labels ZBOSS experimental](https://www.zigbee2mqtt.io/guide/adapters/zboss.html), listing missing install-code support, backups and Inter-PAN; channel changes require re-pairing. The firmware's README describes extra backup capabilities through its maintainer's patched host fork. That does not establish support in the standard Zigbee2MQTT host used here. Keep the lab setup on a fixed channel and treat host parameter changes and recovery as separate validation work. Do not adopt claims about production reliability, node capacity or RF range from marketing text.

## Build and repository workflow

Use the project's [coordinator integration directory](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/esp32c6_coordinator) for the operational README, commands, profiles and scripts. Local changes may precede publication of that directory. This topic explains the design; the integration README owns exact build/flash commands.

The firmware fork is an independent checkout below the integration, ignored by the enclosing project. `origin` points to our fork and `upstream` points to tostmann. The integration lock records the exact firmware revision, version, source timestamp, Docker image digest and target. A clean checkout is required; local uncommitted firmware edits cannot silently enter a build. No submodule or embedded Git entry is added to the enclosing project.

Docker supplies ESP-IDF and its compiler/Python dependencies. The build extracts a `git archive` of the pinned revision into a profile-specific Docker volume, then applies configuration overlays and the reviewed component lock. Windows does not need a native ESP-IDF installation. Source is mounted read-only, while outputs and compiler caches have explicit locations. A Linux CI runner uses the same PowerShell entry point and image. Pinning dependencies makes rebuilding controlled; byte-for-byte reproducibility is a separate test and is not assumed.

Each successful build exports a merged factory image, application image, effective `sdkconfig`, component lock, flash arguments and a manifest with SHA-256 hashes. A failed container build removes its success manifest, preventing the development flasher from using that build. A dependency-resolution build is marked unsuitable for flashing until its generated lock is reviewed and a normal locked build passes.

Source updates are deliberate: compare maintained upstream changes, sync the fork when authorized, update the source/image/component locks together when required, then rebuild and repeat hardware tests. Avoid upstream's `scripts/bump_build.sh`; that helper can stage and automatically commit local changes. The integration calls `idf.py` directly and retains the upstream version, with profile/revision/hashes identifying our build.

## Board isolation

| Profile | Antenna / pins | Validation boundary |
| --- | --- | --- |
| `xiao-ext-usb` | Upstream external-antenna switch enabled: GPIO3 low, GPIO14 high; 4 MB flash | XIAO only; requires an attached antenna |
| `generic-usb` | External switch disabled; upstream still configures GPIO3 and GPIO14 as inputs | Comparison build only; not an approved display-board image |

Both profiles reserve USB for binary NCP traffic and disable the parallel UART1 NCP endpoint. This avoids claiming GPIO22/23 unnecessarily. Upstream's generic RF initialization still touches GPIO3/14, so changing a profile name is insufficient to qualify another board. No project patch to the Zigbee protocol core is currently needed.

The [Seeed guide](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/) documents the antenna switch. Configuration validation can establish that the external route was compiled in; it cannot establish connector quality, antenna gain or measured range.

## First use on Windows

Identify the device through USB inventory and an ESP32-C6 chip query before selecting its COM port. Port numbers can change, so the flasher checks the expected base MAC as well as chip, flash size and security state. Close serial monitors, browser flashers and Zigbee2MQTT before flashing.

Flashing runs through a workspace-local esptool environment on Windows. Docker is used for compilation; no USB passthrough or WSL USB attachment is required. A merged factory image is written at offset zero. An explicit development-erase option clears existing flash/network state; the user waived a full-flash backup for the current development XIAO. Flash verification establishes written bytes, not radio/network operation. No eFuse or permanent hardware-security changes are part of this setup.

Start the local broker, then Zigbee2MQTT with the identified COM port, using the integration README. MQTT listens on `127.0.0.1:1883`, the frontend on `127.0.0.1:8080`, and the base topic is `sustain/zigbee`. The initial channel is 15. Network keys, device databases and logs stay in ignored runtime data; do not publish them or copy them into documentation. Joining must stay closed until an intentional node test.

After startup, distinguish these checks: serial connection, firmware version response, network formation/resume, host restart/reconnect, then a real node's interview and decoded MQTT reports. A [custom device converter](https://www.zigbee2mqtt.io/advanced/support-new-devices/01_support_new_devices.html) may be necessary for the Nordic firmware. A successful join alone does not establish useful telemetry.

## Migration to the Waveshare touch display

The proposed replacement is the [Waveshare ESP32-C6-Touch-LCD-1.9](https://docs.waveshare.com/ESP32-C6-LCD-1.9), a 170×320 touch-display board. The manufacturer lists separate touch/non-touch variants. The family uses 8 MB flash, an ST7789V2 display, optional CST816 touch, and other peripherals. Confirm the actual SKU, revision and schematic before assigning pins or selecting a binary. The touch variant's external-antenna route requires a resistor change according to the manufacturer; it is not the XIAO's software RF switch.

Implement migration as a distinct board profile plus optional board/display component. First run USB NCP with the display disabled; then add LCD, then touch, testing NCP latency and stability at every stage. Disable XIAO-specific GPIO handling entirely for that board, review UART/USB/SPI/I2C pin conflicts, and explicitly select flash size/partition layout. Preserve the NCP protocol core; isolate any unavoidable board hook in a small reviewable patch that can be reapplied to upstream updates. Do not reuse an old Zigbee NVRAM blob across boards/stack versions without a validated migration path.

Display membership should come from the host device registry: joined-node count excluding the coordinator, separately measured available/stale nodes, network channel and host-connection state. A neighbor table is not a mesh-wide membership list. The binary USB NCP stream must remain free of log text or ad-hoc UI messages. Before implementing the panel, choose a separate host-to-display channel or a host/firmware protocol extension with explicit compatibility tests. Wi-Fi on the same C6 adds radio-coexistence work and is not enabled for this USB baseline.

Neither LCD/touch operation nor node/radio range or long-duration stability is established by the current profile design. See [[Sources/References|primary sources]] and [[../Overview|project status]].
