# Nordic Zigbee node

The XIAO nRF54LM20A Sense boards provide a temporary Zigbee test platform while the nRF54LM20 DKs are pending. The original NCS/Zephyr application emits **simulated** acoustic observations. Two powered routers and two sleepy end devices can exercise the [[Zigbee Coordinator|ESP32-C6 coordinator]] and host decoder before microphone and AI integration.

The [firmware operations guide](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/nordic_zigbee) owns exact setup/build/programming/test commands and dependency provenance. Local firmware may be ahead of the published repository. On 2026-10-25, one XIAO sleepy end device passed native build, full application readback, coordinator interview and decoded simulated-report checks. Reflash/reset retained its network identity with joining closed. Detailed measurements and host-recovery results live with the firmware validation record; router hardware, routed mesh, physical power cycling and power measurements remain separate tests.

## Module boundaries

| Component | Responsibility | Must remain independent of |
| --- | --- | --- |
| Observation model/encoder | Versioned units, ranges and byte layout | Zephyr, pins, radio APIs, microphone drivers |
| Simulated source | Deterministic synthetic scores/directions for link testing | Zigbee, board pin assignments |
| Zigbee transport | Endpoint/cluster, commissioning, receiver role, scheduler/buffer ownership and latest-value handoff | Audio acquisition and inference implementation |
| Board status | Optional PWM indicator through device-tree aliases | Network decisions, report generation |
| Main application | Composition, scheduling and producer-to-transport handoff | Direct peripheral register operations |

Application threads hand immutable observation snapshots to the transport API. The adapter encodes a validated record, copies it under a short spinlock and schedules radio work in ZBOSS context. No application thread calls packet/buffer APIs directly. Reports can coalesce while a send is in flight; no offline history or application acknowledgement is implemented. The next periodic sample retries after scheduler/buffer pressure. Preserve event history separately when adding the main project's storage/BLE path.

One board profile owns pin and console choices; one role profile owns router versus sleepy behavior. The XIAO profile preserves the user's tested active-low RGB PWM mapping: P1.22 red, P1.23 blue, P1.24 green. The DK profiles use logs and do not inherit those pins. Antenna changes belong in a reviewed board/RF profile, with the board schematic and actual connector/switch behavior checked first. No new antenna-switch behavior is assumed here.

## Bench report contract, version 1

HA profile `0x0104`, device ID `0xfff2`, endpoint **10**, private cluster **0xfc00**, attribute **0x0000**, ZCL octet-string type **0x41**. Basic manufacturer is `Sustain`; model is `SustainNode-v1`. The node explicitly sends periodic global Attribute Reports to coordinator address `0x0000`, HA endpoint **1**. It does not require binding/configure-reporting and does not expose runtime report-interval writes. Do not infer interoperability with arbitrary hubs from this bench contract.

The octet string carries 16 bytes, preceded on the wire by its ZCL length byte `0x10`. Multibyte integers are little-endian. No C struct is transmitted by casting or packing.

| Record byte offset | Type | Meaning |
| --- | --- | --- |
| 0 | u8 | Schema version, 1 |
| 1 | bitmap8 | Bit 0 simulated; bit 1 anomaly; other bits zero |
| 2–5 | u32 | Generated sequence, restarts on reboot; wraps after 2³² values |
| 6–9 | u32 | Uptime seconds, wraps after 2³² seconds |
| 10–11 | u16 | Score in thousandths, 0–1000; not a calibrated probability |
| 12–13 | u16 | Direction degrees, 0–359; synthetic in this build |
| 14 | u8 | 1 router; 2 sleepy end device |
| 15 | u8 | Reserved, zero |

Host decoding rejects unsupported versions, incorrect lengths, reserved bits, invalid scores/directions and unknown roles. The `simulated` flag remains true for all test records. A real inference producer must clear it deliberately, supply defined measurement semantics and retain validation evidence. Do not describe synthetic scores or rotating directions as detection/localization results. Do not put microphone frames, logs or secrets into this compact radio record.

## Moving to the DK and main application

1. Identify the actual DK/SoC A or B variant and silicon revision. Verify the SDK/add-on matrix before selecting its board target. The pinned R23 add-on 1.4.0 documents SDK 3.4.0; this project's installed-SDK build uses 3.4.1 and requires independent integration/hardware validation. [[MCU and Board Capabilities|Board capabilities]] and [[Bluetooth and 2.4 GHz Radio|radio references]] retain the broader device context.
2. Keep model/encoder and Zigbee transport source/API intact. Select the DK device-tree/role profile, compile both roles, inspect radio clock/timer and memory partitions, then perform a single-node interview/report/reboot test. GPIO and RGB mappings never move into the portable components.
3. In the main firmware, adopt these source directories as explicit CMake components (or a Zephyr library/module when a second application actually needs one). Keep only one definition of the report schema and endpoint/cluster registration. Avoid a second fork of the Zigbee transport inside the microphone app.
4. Replace the simulated producer with acquisition → preprocessing → inference → observation conversion. Keep microphone DMA/interrupt handlers free of Zigbee calls. Submit snapshots from a worker thread after validating score/direction ranges. Define real event/history persistence and backpressure policy separately; the current latest-value transport cannot preserve every anomaly event.
5. Keep router/SED selection at build time. Final monitoring nodes retain the sleepy-end-device role; router images remain powered test infrastructure. Receiver sleep does not prove low total power. Disable diagnostic PWM/logging and profile board peripheral rails before measuring energy.
6. Add BLE commissioning/history access as a separate service and validate coexistence under actual microphone/inference load. Keep the NFC-to-USB-reader/Android flow described in [[../Overview|Overview]]. Array acquisition, NPU acceleration and BLE are not supplied by this Zigbee test.
7. Freeze/review the main application's memory layout before OTA/MCUboot work. Current direct-programming test images replace the Seeed bootloader layout and reserve separate Zigbee state/product-configuration areas. Preserve those storage boundaries across routine upgrades; a board/role change or erase can require deliberate removal/recommissioning. Do not move persisted state blindly between SoCs.

## Development and review rules

Keep board support in overlays/profiles, deterministic data logic in ordinary C and ZBOSS APIs in the transport. Use explicit units/ranges, bounded storage and clear buffer ownership. Avoid blocking work inside radio callbacks. Update encoder, decoder, contract and meaningful golden/malformed-record checks together for a schema change; incompatible contracts need a new version/model strategy.

Pin SDK, add-on, board repository and Docker toolchain together; preserve upstream sources and review dependency changes as one integration change. Use pristine builds per board/role and retain effective configuration/device-tree evidence. Build success, interview success, decoded MQTT, routed forwarding, rejoin, coexistence and energy are separate acceptance results.

Primary references: [[../Sources/References|Sources]]. The [Nordic R23 setup/release documentation](https://github.com/nrfconnect/ncs-zigbee/tree/bc7863e99cbbe121d8755bc49586e977cd46d032/docs), [Seeed getting-started guide](https://wiki.seeedstudio.com/xiao_nrf54lm20a_getting_started/) and [Zigbee2MQTT external-converter guide](https://www.zigbee2mqtt.io/advanced/more/external_converters.html) support the SDK, board and host integration; they do not establish this project's measured results.
