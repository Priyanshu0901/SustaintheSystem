# Sparse herd telemetry and recovery

Checked: 2026-10-04. [Project](../Project.md) · [Overview](../Overview.md) · [Radio capabilities and geometry](../Bluetooth%20and%202.4%20GHz%20Radio.md)

## User direction and scope

Priority correction, 2026-10-05: the current goal is a simple useful solution to a complex real-world problem. First identify the required intervention and minimum evidence; then tailor the solution. Reliability/energy optimization is supporting engineering research, not the central problem statement. [Project](../Project.md) owns the current direction. This note does not select a herd telemetry product or impose 100 tags, hourly location, storage mechanisms or protocols.

Earlier clarification, 2026-10-04: "non-intrusive" means low observation/data/processing burden here, not necessarily tagless observation. The user raised efficient Bluetooth connectivity and dependable data management for roughly 100 cattle tags, with location every 30 minutes/hour as examples. The agent overpromoted those concerns to the central project question; that framing is now superseded. Minute behavior classification and comprehensive labeling are not requirements. Dog/bench testing remains available for relevant shared components.

This is a distributed telemetry/state-management problem. It becomes distributed intelligence when tags and receivers make complementary decisions about measurement, retention, communication and which uncertainties need attention. Machine learning and peer-to-peer consensus are not prerequisites. Exact protocol, cadence, failure guarantees, alert semantics and infrastructure remain unselected; no implementation or scale test has occurred.

## Workload and the actual scaling question

Illustration only: a 32-byte logical application record per tag per sample, before radio/security/storage framing, retransmission and ranging observations.

| Example cadence | Records/tag/day | Records/100 tags/day | Application bytes/day |
| --- | --- | --- | --- |
| Every 30 minutes | 48 | 4,800 | 153,600 = 150 KiB |
| Every hour | 24 | 2,400 | 76,800 = 75 KiB |

One complete herd sweep contains 3,200 application bytes under this assumption. The record format is not selected and a 32-byte record is not asserted to fit one legacy advertisement. Raw ranging/sensor data can be much larger. These figures do not establish capacity, power or CPU use.

At this sparse rate, raw application-byte volume alone is unlikely to explain high processing demand. Discovery/scanning duty, sensor acquisition, connection/ranging initiation, synchronized bursts, retries, backlog recovery, database work and energy can dominate. Measure them separately. Roughly 100 tags need not mean 100 simultaneous persistent connections; actual controller limits and the selected scheduling approach remain to verify.

Three different clocks matter: **measurement cadence**, **upload opportunity**, and **maximum useful alert latency**. An hourly measurement can be stored locally and uploaded later; hourly uploading can alternatively batch more frequent measurements. Neither guarantees hourly fresh information at the receiver. A change just after a fix remains unobserved until another successful observation, with additional delivery delay. Faster event-triggered observations are a possible design option, not a selected policy.

## Proposed responsibilities and evidence

| Role | Candidate responsibility | What must remain explicit |
| --- | --- | --- |
| Animal tag | Obtain sparse quality-checked observations, keep durable uncollected records, track battery/reset state, retry within an energy budget | Acquisition can fail; location is not current while position sensing sleeps. A reboot must not silently erase or relabel history. |
| Receiver/base/gateway | Arrange collection opportunities, persist/deduplicate records, issue durable receipts, track per-tag freshness and gaps | Receiving a packet differs from retaining its record. Ranging-anchor and collector roles can share hardware only if scheduling/support are verified. |
| Herd state view | Combine latest valid observation, its age/quality, tag status and missing-data evidence; prioritize inspection | Distinguish animal concern, tag concern and network uncertainty. Position alone is not a diagnosis. |

These are proposed responsibilities, not selected firmware or device allocation. The initial POC remains three fixed bases plus moving DK. A gateway role must not silently move the DK to a fixed base. Bluetooth ranging applies within anchor coverage; intermittent contact/storage does not create coverage across an entire farm or reserve. Outdoor GNSS and energy tradeoffs are maintained in the [GNSS reference](Animal%20Tag%20GNSS%20Comparison.md).

## Reset and loss: distinguish failure points

| Failure | Proposed behavior / guarantee boundary |
| --- | --- |
| Tag resets before committing an observation | Volatile or incomplete data may be lost. Recover previous committed records and expose reset/gap evidence; do not promise recovery of unrecorded measurements. |
| Tag resets with committed but unuploaded records | Reconstruct a durable pending queue and retry after boot. Persistence requires suitable flash/storage design and successful recovery checks. |
| Receiver gets a packet then resets before storage | Tag retains the record until an application receipt confirms the agreed durable storage point. Radio reception alone must not authorize deletion. |
| Receiver commits a record but receipt is lost | Retry can produce duplicates. Use stable record identity and deduplication so one observation contributes once to the state view. |
| Two receivers collect the same record | Merge by the same durable identity, preserve provenance and handle conflicting payloads explicitly. Two receptions are not two animal observations. |
| Internet/backhaul fails | Gateway queues records locally. Whether a tag can delete its copy after gateway persistence depends on the required survival/replication policy. Local persistence is not cloud replication. |
| Radio contact unavailable | Bound retries/backoff; retain records within capacity and expose stale state. Missing reports alone cannot establish immobility, death or illness. |
| Queue fills or flash/storage fails | Apply an explicit retention/loss policy and expose dropped sequence ranges or counts. Finite storage cannot preserve arbitrarily long outages. |

Candidate transaction pattern: **commit locally → transmit → receiver commits/deduplicates → application receipt → safely reclaim acknowledged records**. A later backend upload is another stage. The failure guarantee depends on whether one durable copy is sufficient or independent copies are required. No universal exactly-once or zero-loss claim is made.

Record identity must survive resets: for example a stable tag identity plus a safely managed generation/sequence. Resetting a RAM counter to zero can collide with old records. Acknowledgment checkpoints and sequence allocation also need power-failure consistency; do not erase an unacknowledged gap because later records arrived. Clock handling matters: retain acquisition time, reception time and time-quality information; a timer reset does not provide a trustworthy new UTC timestamp. Uncommitted final writes and destructive storage corruption remain loss boundaries.

Bluetooth connected link-layer ordering/retransmission is described by the [SIG LE primer](https://www.bluetooth.com/bluetooth-le-primer/). Those link acknowledgments establish reception at that layer, not application flash persistence or survival of a later receiver reset. Application receipts and durable record identities are therefore separate design concerns.

## Storage and radio references, not protocol commitments

- [Zephyr NVS](https://docs.zephyrproject.org/latest/services/storage/nvs/nvs.html) writes data before completion metadata and ignores entries missing valid metadata during initialization. Metadata CRC is distinct from optional data CRC. It is an ID/value system; configure retention/history deliberately rather than assuming an unbounded telemetry journal. Flash wear and partial/incomplete writes still need actual-device testing.
- [Zephyr FCB](https://docs.zephyrproject.org/latest/services/storage/fcb/fcb.html) supplies FIFO flash entries, checksum-based completion and explicit rotation. It is a candidate mechanism for queued records, not an automatic end-to-end recovery solution. Storage layout, power behavior, recovery and acknowledgment state remain application work. No local SDK module/configuration has been checked.
- The [Bluetooth SIG PAwR overview](https://www.bluetooth.com/bluetooth-resources/new-bluetooth-technology-periodic-advertising-with-response-will-bring-digital-benefits-to-physical-stores/) describes scheduled low-power responses for many devices. PAwR is a candidate to investigate alongside bounded connection-based collection or advertising with a separate retrieval/receipt path. Synchronization, rejoining, roaming, supported controller features and shared ranging timing matter; periodic train timing is not simply a 30-minute application sample interval. Plain repeated broadcast does not supply application custody/durability by itself.
- A [2026 connectionless CS/PAwR preprint](https://arxiv.org/abs/2605.17094), revised 2026-09-23, uses LE CS Test commands and application coordination on an nRF54L15 POC. Its compact payload/scheduling results are relevant research leads, not standard connectionless production Channel Sounding or evidence of LM20 compatibility/100-cattle field operation. Abstract checked; full implementation not reproduced. The large capacity projections are not measured herds and are not adopted as project claims.

At sparse cadence, compare measurement/acquisition, wake/discovery, radio transfer, storage and receiver costs independently. Minimize wakeups and avoid retry storms rather than assuming payload compression solves all energy use. Authenticate identity/receipts as appropriate; transport reliability does not authenticate an animal record. These points do not select security or network protocols.

## What "wrong" can mean with sparse observations

Maintain separate observations and interpretations:

- A tag reports low battery, a reset, storage overflow or failed position acquisition: device/service concern.
- Many tags become stale at one receiver: possible shared communication/infrastructure concern; cause still needs inspection.
- A valid animal position is outside an expected area or coarse movement changes persist: candidate animal-location/routine concern.
- A tag has not been heard from: uncertain state, with last valid observation age and coverage information. Do not silently extrapolate a fresh location.

Unchanged location can be normal rest, poor positional resolution or stale data. One hourly fix cannot prove no movement between fixes. Sparse coarse checks are useful for some exceptions but cannot establish minute feeding/gait events or a health condition. Alert thresholds, expected routines, contact windows and urgency remain to define with the user. A cheap wake/event source could increase observation rate when warranted, with energy measured separately.

## Testability without 100 cattle

The systems focus can be developed without a cattle behavior dataset:

- **Real few-node bench:** known record histories, physical resets/power interruptions at transaction boundaries, lost receipts, duplicate/out-of-order delivery, gateway outage and queue exhaustion. Check the reconstructed history, committed-record loss, duplicate effects and explicit gap reporting.
- **Moving surrogate/dog access:** changing contact conditions, representative antenna/body effects, timestamps and occasional position collection. The DK remains carried/surrogate; an actual animal tag still needs suitable assembly. No dog or board experiment has been performed.
- **100 logical producers:** simulate identities, schedules, reset generations, backlog bursts and faults to test ingestion/state/recovery behavior. Report this as software/application-load emulation. It cannot establish 100 physical radios' contention, field coverage or battery performance.
- **Later physical scale/field validation:** radio collisions, scanning schedules, herd bodies/roaming, anchor coverage and actual energy/latency need real radio/field evidence or a separately validated radio model.

Candidate metrics: energy per fresh durably collected observation; observation age distribution; useful delivery rate; recovery time; loss of committed records under a stated fault model; duplicates before/after deduplication; false/missed/stale alerts; receiver duty and backlog/storage occupancy. Compare with a clearly stated simpler baseline, such as periodic reporting with only volatile queues. Zero observed loss in a finite experiment is not a universal guarantee. Robust storage and scheduled BLE are existing techniques; distinctiveness needs measured integration benefits, not a claim to have invented them.

## Status and next discussion

Supporting research recorded; arithmetic and documentation checked. No radio, reset, queue, model, animal, firmware or scale test. Current user-led discussion starts with a concrete real-world problem, required action and minimum timely evidence, as recorded in [Project](../Project.md). Only then derive relevant engineering requirements from the options here. A concrete implementation request requires a separate saved execution plan.
