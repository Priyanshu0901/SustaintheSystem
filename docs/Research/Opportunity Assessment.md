# Opportunity assessment

Application landscape refreshed on 2026-09-26: see the [current 47-entry inventory](Application%20Inventory.md) and [current positioning analysis](Contest%20Landscape.md). Technical research below retains its original research date; the latest headings reinforce the need to demonstrate adaptive network behavior rather than claim uniqueness from an architecture label.

Research date: 2026-09-25. [Landscape](Contest%20Landscape.md) · [Inventory](Application%20Inventory.md) · [Awards](Prize%20Categories%20and%20Judging.md).

## Working hypothesis

Evaluate a distributed network that computes local health features and increases sensing/reporting effort when evidence warrants it, with a central on-device model interpreting trends and requesting diagnostics. This is a **candidate under evaluation**, not an implemented architecture or finalized scope in [Project](../Project.md).

Proposed testable claim: **at a defined early-warning quality and latency, adaptive operation reduces total monitoring energy compared with a sensible fixed-rate design**. Fewer bytes alone are insufficient, and detecting an existing fault is not the same as predicting a future failure.

## Closest visible overlaps

These are watchpoints, not entrant rankings. Competitor claims below come only from their headings.

| Application | Visible overlap | Unknowns and implication |
| --- | --- | --- |
| [Operating-point-aware pump diagnostics](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20407) | State-aware Edge AI | Model and validation unknown; test normal speed/load changes separately from degradation |
| [Industrial acoustic/vibration stethoscope](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20414) | Multimodal condition sensing | Fusion/network/power unknown; acoustic DSP alone is not a distinctive claim |
| [Shop Sentinel](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20360) | Edge-AI machine tags | Topology and duty cycles unknown; demonstrate the contribution of feedback between gateway and nodes |
| [AgriGuard EQ](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20357) | Maintenance mesh | Protocol and power unknown; a multi-node label alone is insufficient |
| [FridgeSeal Sentinel](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20368) | Seal/compressor prediction | Zero-sensor claim unexplained; seals are not an unclaimed topic |
| [Self Diagnosing Nephelometer](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20375) | Fouling prediction and sensor fusion | Ground truth unknown; distinguish process degradation from sensor contamination |
| [ResinGuard](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20371) | Process/consumable condition | Replacement policy unknown; show an actual intervention and avoided waste |
| [Condition-Aware Physical AI](https://www.hackster.io/contests/sustain-the-system/hardware_applications/20359) | Broad condition-aware framing | Asset and behavior unknown; use a specific measured claim |

## Broader primary-source research

### Bluetooth: measure both ends

Bluetooth SIG explains that ordinary advertising has timing randomness, while periodic advertising allows synchronized reception. PAwR adds scheduled bidirectional responses. [Bluetooth LE primer](https://www.bluetooth.com/bluetooth-le-primer/).

**Our engineering inference:** fewer transmitted packets can still require expensive scanning or receive windows. Measure node and gateway energy, define how a sleeping node receives an escalation command, and measure command delay. Compare against an efficient fixed-rate connected or scheduled design as well as raw streaming. PAwR is an option to investigate, not verified support on the intended hardware/SDK.

### Edge AI: demonstrate benefit beyond DSP

Nordic documents Axon in the nRF54LM20B for neural-network acceleration. [Nordic Axon overview](https://www.nordicsemi.com/Products/Technologies/Edge-AI/Axon-NPU). Its official low-power sample lists the DK target `nrf54lm20dk/nrf54lm20b/cpuapp`. [Nordic sample](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/samples/axon/axon_low_power/README.html).

This establishes a documented development path, not our model's compatibility, workload savings, available stock, or the user's exact board revision. Verify conversion, operators, memory, and hardware before committing. Compare simple feature thresholds with a small model; measure NPU execution where feasible. The award story becomes stronger if ML improves early warnings under changing conditions, rather than merely running successfully.

### Compressed-air leakage

DOE identifies hoses, joints, fittings, and valves as common leak locations and describes ultrasonic detection. Its historical tip sheet reports that leaks can waste 20–30% of compressor output; that is contextual evidence, not an estimate of our future savings. [DOE leak tip sheet](https://www1.eere.energy.gov/manufacturing/tech_assistance/pdfs/compressed_air3.pdf).

**Candidate interpretation:** monitor progression at several leak points and request diagnostics before prolonged waste. No heading explicitly names compressed-air leak evolution, but HoseGuard could overlap. First verify acoustic bandwidth, background-noise tolerance, and correlation with measured leak flow. Do not assume an onboard microphone supports ultrasound. Sound level alone does not establish saved electricity; quantify leak flow and actual compressor behavior.

### Filters and membranes

DuPont describes fouling-related changes in normalized flow, salt passage, and pressure drop, and explains that temperature and operating changes can affect output without proving fouling. [FilmTec technical manual, cleaning requirements](https://www.dupont.com/content/dam/water/amer/us/en/water/public/documents/en/RO-NF-FilmTec-Manual-45-D01504-en.pdf).

**Candidate interpretation:** a filtration rig could tie trends to inspection or replacement. Appropriate pressure/flow/process sensors are likely needed; IMU/microphone features are not established as sufficient. Do not transfer membrane-specific thresholds to another filter. The nephelometer and ResinGuard headings make this an adjacent area, not an empty niche. Simulated restriction would validate restriction detection, not automatically long-term membrane-life prediction.

### Valves and pipes

DOE's maintenance guide describes ultrasonic analysis for component wear, fluid leaks, and steam-trap failures. [DOE maintenance guide](https://www.energy.gov/femp/articles/operations-and-maintenance-best-practices-guide-achieving-operational-efficiency).

**Candidate interpretation:** choose one mechanism—such as valve seat leakage or delayed actuation. Valve stiction, hose leakage, and pipe-wall damage require different evidence. Match process measurements or suitable acoustic sensing to the actual failure; “pipe health” is too broad for an initial validation claim.

## Conditional shortlist

This is our recommendation for discovery, not a uniqueness claim, purchase instruction, or prediction of winning chances.

| Candidate | Possible award story | Intervention and useful metric | Selection gate |
| --- | --- | --- | --- |
| Progressive compressed-air leakage | Equipment + Bluetooth; AI if validated | Repair leaking part; leak flow and energy consequence | Controlled rig and sensor bandwidth |
| Filter restriction/membrane fouling | Equipment/infrastructure/resource, depending on site | Inspect/clean/replace; pressure, flow, consumable waste | Process sensors, normalization, realistic progression |
| Specific valve degradation | Equipment + Bluetooth | Service valve; leakage or actuation trend | Repeatable ground truth and operating-state separation |
| Refrigeration seal degradation | Equipment/infrastructure | Repair seal; comparable runtime/energy | FridgeSeal overlap and ambient/load confounding |
| Water-loss/pipe monitoring | Infrastructure/resource | Locate/repair leak; measured water loss | Narrow failure mode and separate effects of usage |

**Recommendation:** first assess access to a compressed-air, valve, or filtration setup and reliable progression measurements. Choose the strongest ground truth and maintenance intervention, not simply the least similar heading. Keep adaptive network behavior as the experimental contribution.

## Proposed evidence package

These are future experiments. No hardware measurements were performed during this research.

1. **Controlled conditions:** healthy, progressively degraded, and severe states plus normal operating variations. Separate independent runs/assets between training and testing; random windows from the same capture can produce misleading evaluation.
2. **Fair baselines:** raw transfer, fixed-rate feature reporting, and adaptive reporting on the same task. Compare simple thresholds with ML. Set acceptable detection quality and latency before optimizing energy.
3. **Energy:** node joules/hour, gateway energy, airtime/bytes, diagnostic frequency, and processing/inference cost. Include sensing, scanning, listening, and development-board overhead.
4. **Detection:** false alarms per node-day, missed events, detection delay, and robustness to operating changes. A forecasting claim needs a defined future event and lead-time evaluation.
5. **Network:** command receipt, missed-packet recovery, escalation, and return to normal. Prevent repeated switching between modes with a justified policy.
6. **Maintenance:** show what changed, uncertainty, and the next action. Measure resource loss before/after intervention; distinguish measured from projected savings.
7. **Decision metric:** define a correct decision episode before computing energy per useful health decision. Also report energy per time and error rates so repeated alerts cannot inflate the denominator.

Three nodes displaying healthy/developing/severe states can communicate the system clearly. Scripted states alone do not validate prediction or generalization; pair the demonstration with repeatable measurements and a reproducible build guide.
