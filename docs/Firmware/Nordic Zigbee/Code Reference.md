# Nordic code reference

The generated Doxygen reference indexes original C interfaces, internal functions and Python helpers. Function arguments, errors, ownership and simulator behavior are documented beside declarations in the C headers.

## Code map

Paths below are relative to [the integration source](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/nordic_zigbee). Published files may lag the local checkout.

| Source | Responsibility |
| --- | --- |
| `include/observation.h`, `src/observation.c` | Portable snapshot and validated byte encoder |
| `src/simulated_source.c` | Deterministic single-producer test data |
| `include/zigbee_transport.h`, `src/zigbee_transport.c` | Thread handoff, endpoint registration and ZBOSS callback/buffer lifecycle |
| `include/board_status.h`, `src/board_status.c` | Profile-selected PWM indicator or no-op |
| `src/main.c` | Startup and periodic composition |
| `host/sustain-node.cjs` | `decodeObservation` validates received bytes; the converter registers the private cluster and exposes decoded values |
| `tools/verify_inputs.py` | Locked repository revision and integration cleanliness checks |
| `tools/check_build.py` | Effective role, partitions, resource bounds and artifact hashes |
| `tools/build_container.py` | Isolated pinned SDK checkout and container build execution |
| `tools/serial_monitor.py`, `tools/mqtt_monitor.cjs` | Bounded timestamped bench capture with stop markers |
| `tools/summarize_hardware.py` | Simulated-field validation, cached-duplicate exclusion and per-boot summaries |
| `Build-Native.ps1`, `Build-Docker.ps1`, `Flash-Native.ps1` | Build orchestration and explicit-probe verified programming |

## Generate and browse

The [generator guide](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/tools/documentation) owns installation, generation commands, output locations and parser coverage. Doxygen generates searchable C/Python documentation from source; the CommonJS and PowerShell entries above are maintained code maps.

[Architecture](https://github.com/Priyanshu0901/SustaintheSystem/blob/master/docs/Firmware/Nordic%20Zigbee/Architecture.md) owns concurrency and data flow; [report protocol](https://github.com/Priyanshu0901/SustaintheSystem/blob/master/docs/Firmware/Nordic%20Zigbee/Report%20Protocol.md) owns wire fields. [Operations](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/nordic_zigbee) owns commands, prerequisites and actual validation.
