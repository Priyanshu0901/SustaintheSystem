# Project

Updated: 2026-10-10

## Purpose

Project name: **Sustain the System**.

## Primary goal

**Build systems that know when they need attention. Predict failure. Prevent waste. Extend the life of the systems our world depends on.**

Develop a predictive monitoring solution for the **Sustain the System competition**, using **Nordic Semiconductor's nRF54LM20 DK and Bluetooth technology**. Detect degradation or other issues early enough to enable proactive maintenance, prevent failure and unnecessary replacement, and help equipment, infrastructure, or natural resources last longer.

The user adopted the supplied competition overview as the primary project goal on 2026-09-24. See [the supplied competition overview](Sources/Competition%20Overview.md) for the preserved source, examples, prize descriptions, and resource links. That note preserves the original unverified material. Official categories, scoring, and key dates were subsequently checked on 2026-09-25 and rechecked on 2026-10-05 in [Prize categories and judging](Research/Prize%20Categories%20and%20Judging.md); selected technical claims and remaining uncertainties are covered in [Opportunity assessment](Research/Opportunity%20Assessment.md).

## Required solution outcomes

The solution should demonstrate how Bluetooth technology can:

- Collect and share meaningful data from the physical world.
- Monitor changes and detect critical conditions.
- Alert users and recommend next steps based on actionable insights.
- Improve efficiency, resilience, or resource utilization with practical sustainability outcomes.

The selected use case should connect early detection to a concrete maintenance or intervention action and a measurable benefit. Specific metrics and thresholds remain to be defined.

## Candidate scope and technical opportunities

Product ambition, clarified **2026-10-10**: pursue a useful product whose eventual scope may exceed the contest demonstrator, with a distinctive **contest project**. Uniqueness concerns the contest, not worldwide product-category novelty; an existing commercial category is not a reason to reject a useful application. Time and engineering effort are not reasons to cap the concept at a small monitoring example. Keep the problem-first, simple useful solution direction: substantial engineering may be justified, while sensing and user burden should follow actual needs. The contest can demonstrate a meaningful part of the larger product; no larger scope, release or final concept is selected yet.

Detection must lead to action: establish **which animal, asset, component or area needs attention**, at a resolution that makes the intervention practical, with supporting evidence and uncertainty. Merely detecting an anomaly can leave the operator with the original search problem. Spatial coordinates alone also do not establish the failing part or cause. An alert may narrow the search without diagnosing everything, but the remaining inspection effort and product value must be explicit. Contest distinctiveness needs comparison with actual entries; commercial products/research inform useful scope and comparative value, without imposing global novelty.

**Compressed-air leakage is excluded from active candidates** by the user's explicit correction on 2026-10-10; it was an agent-added proposal, not a user shortlist item. [Opportunity assessment](Research/Opportunity%20Assessment.md#product-direction-and-actionable-location--2026-10-10) preserves the rejected proposal as history and records the Fluke comparison. Ideas 2–5 remain promising, without final selection. The user retains planning direction.

Idea-selection direction, 2026-10-03: the user wants an ambitious core concept that stands apart from crowded application themes and aims to outperform existing entries. Concept exploration should not begin by restricting choices to available test rigs or incremental variations. The user will handle planning during this discussion. No concept has been selected; novelty and comparative superiority are goals, not verified facts.

- **Equipment:** bearing wear, pumps, HVAC, conveyor motors, generators, solar inverters, irrigation systems, or agricultural equipment.
- **Infrastructure:** community infrastructure, building energy systems, or greenhouses.
- **Natural resources:** irrigation, animal and wildlife health, or water resources.

These are examples from the overview, not selected deliverables. The target asset and intended users remain undecided.

The overview highlights on-device machine learning and edge computing as opportunities, including anomaly detection, predictive failure modeling, sensor fusion, and maintenance recommendations. A particular model, sensor set, architecture, or accelerator is not yet selected; edge AI is not stated as a mandatory requirement in the supplied overview.

## Confirmed requirements

- Current wearable concept under investigation: **LM20B chest-worn stethoscope/body sensing for a person**, using microphone and accelerometer; dog placement is a possible extension to compare. Target body sounds rather than environmental sound recognition. Keep the current **non-clinical** scope, sensing interpretation/decision processing **on the nRF** and no constant raw stream to the phone; phone may configure/display/synchronize results. Exact vital outputs, parts, mounting and performance requirements remain open. Earlier people/animal market survey is background. Frequent local acquisition may be necessary; cadence and energy follow requirements.
- Keep canonical project documentation in this self-contained Obsidian vault, starting at [[Overview]]. Note navigation must stay within this vault.
- Follow **planning → vault → execution** for project tasks.
- Vault documentation itself is exempt from the preliminary planning and task-record requirement.

## Current state

Latest clarification, 2026-10-10: user specifies **LM20B** and a **person's chest-worn stethoscope/body-sensing wearable**, with microphone and accelerometer; asks about human market and dog placement. [PDM/I2S and body-sensing assessment](Research/Opportunity%20Assessment.md#lm20b-chest-body-sensing-pdm-versus-i2s--2026-10-10) distinguishes interface, acoustic/contact coupling, heart/respiration feasibility, quality/motion limits, sold handheld instruments/patches and wearable research. Dog acoustic placement is a bounded extension, not selected deployment. The broad market ranking below is preceding exploration, not a directive to choose worker/environmental sensing. LM20B target and person are explicit; exact board receipt, sensors, outputs and product value remain unknown. No prototype or physiological/clinical performance verified.

Latest wearable-market review, 2026-10-10: [canonical comparison](Research/Opportunity%20Assessment.md#non-clinical-wearables-and-local-nrf-processing--2026-10-10) covers sound-awareness, work movement/exposure, cattle management and working-animal workload against commercial/research baselines. Useful local processing is plausible; no model, energy result, buyer demand or contest uniqueness verified. This supersedes clinical-project and live-audio/phone-processing emphasis as current exploration. Older stethoscope feasibility remains history. People/animals review does not select a household-pet convenience project, change the moving-DK direction or authorize wear/trials.

Stethoscope feasibility follow-up, 2026-10-10: user clarified **XIAO nRF54LM20A Sense**. [Canonical analysis](Research/Opportunity%20Assessment.md#phone-connected-stethoscope-feasibility--2026-10-10) assesses body-sound recording plus a phone app as a potentially useful direct BLE instrument, without requiring distributed computing. Acoustic coupling and exact microphone response remain unverified; vendor record-then-upload code is not a live phone or validated diagnostic system. Human/veterinary target, product advantage, sustainability outcome and meaningful contest-DK role remain open. No new concept, architecture or experiment selected.

Technical follow-up, 2026-10-10: the [array/camera and Bluetooth analysis](Research/Opportunity%20Assessment.md#array-camera-and-bluetooth-contribution--2026-10-10) distinguishes source direction/location, actual DK acquisition/compute limits, raw-data bandwidth and whole-system power. The user is testing whether Bluetooth contributes real product value when sensing/processing is heavy; low radio power alone is insufficient evidence of low total power. This is a thought experiment, not selection of an acoustic product or reversal of the compressed-air rejection. Hardware/algorithm feasibility, energy fractions and network contribution remain unmeasured.

Current interest, 2026-10-10: the user finds ideas **2–5** promising: individual-animal inspection priorities, shared water-loss accounting, stored-grain hotspots and irrigation delivery verification. The [DK feature-utilization comparison](Research/Opportunity%20Assessment.md#development-kit-utilization-for-ideas-25--2026-10-10) assesses the actual development kit against ordinary boards and finished instruments, separating useful, conditional and unused features and external sensing needs. This supersedes the emphasis of the earlier agent shortlist, not the six proposals or the problem-first direction. No final concept or hardware allocation selected; moving-DK component POC remains unchanged. Verified specifications are in [Board capabilities](MCU%20and%20Board%20Capabilities.md#nordic-nrf54lm20-dk); comparative advantages are hypotheses, not measured superiority.

The [dated problem-first opportunity review](Research/Opportunity%20Assessment.md), checked 2026-10-05, originally proposed six applications: cattle water-point reliability, individual inspection priorities, shared water-loss accounting, stored-grain hotspots, irrigation delivery verification and compressed-air loss triage. These remain dated history, with compressed-air loss explicitly rejected on 2026-10-10. The earlier agent shortlist of water points, shared water losses and stored grain is superseded as current emphasis by the user's interest in ideas 2–5. Evidence/action limits remain useful; none establishes uniqueness or product superiority.

Current goal, clarified 2026-10-05: understand a complex real-world problem and determine the smallest useful solution its requirements demand. Animal health/care at scale is an area of interest; other previously researched problems remain available for discussion. Identify a concrete harmful situation, who needs to act, what decision they must make, and the minimum evidence and timeliness needed to support that action. Tailor sensing, cadence, connectivity and data handling to those requirements. A tag is not expected to detect every aspect of an animal; avoid unnecessary sensing, battery use and solution complexity. No final problem statement, solution or requirements have been selected. The user retains planning direction.

The agent's earlier central question about reliable herd information at minimum energy/communication cost is superseded as the problem statement. Bluetooth efficiency and reset-resilient data management remain supporting engineering concerns where the selected solution needs them. The [sparse herd telemetry and recovery reference](Research/Sparse%20Herd%20Telemetry%20and%20Recovery.md) retains load arithmetic, failure boundaries and validation options; roughly 100 tags and 30-minute/hourly observations are exploratory examples, not fixed requirements. In this discussion "non-intrusive" means low observation/data burden, not necessarily no wearable.

Application direction, clarified 2026-10-04: the user wants to address the difficulty of individual care across large cattle herds or wildlife populations. The household-pet convenience direction suggested by the agent is superseded. The user has regular access to a dog and no farm access; the dog is a possible test surrogate for shared sensing/learning components, not a substitute for cattle behavior or health validation. Species, final concept and architecture remain open.

The [animal concept reference](Research/Animal%20and%20Wildlife%20Health%20Concepts.md#cattle-target-dog-surrogate-and-reduced-label-learning) retains candidate problems, transferable tests, learning/data references and attachment alternatives. Use only the observations needed by the eventual problem; detailed classification and tagging alternatives are not requirements. The prior tagless interpretation of "non-intrusive" was the agent's interpretation, now superseded. The [pet market comparison](Research/Pet%20Products%20and%20Household%20Validation.md) remains competitor history. No model, wearable, animal trial or implementation selected or validated.

The [GNSS module comparison](Research/Animal%20Tag%20GNSS%20Comparison.md), checked 2026-10-04, evaluates LG77LICMD/L89HA and alternatives, with conditional prototype/custom-board recommendations and antenna, ordering and power uncertainties. No GNSS hardware or animal deployment selected.

Animal and wildlife health is now being explored at the user's request. The [concept reference](Research/Animal%20and%20Wildlife%20Health%20Concepts.md) records five proposals involving resource access, mobility, waterholes, rehabilitation and sickness behavior, with original research and inference limits. No species, application or biological model has been selected.

The [animal tag/sensor/impact reference](Research/Animal%20Tags%20Sensors%20and%20Impact.md), checked 2026-10-04, distinguishes sensor measurements from behavior/health interpretation, compares attachment methods and documents capture and continuing animal burden. No tag assembly or animal deployment has been selected or validated.

Localization POC direction, confirmed by the user on 2026-10-04: three fixed bases and a moving main device for an initial 2D demonstration. If received, the DK will be the moving main device. Later custom base PCBs are intended to reduce cost; general 3D would need a fourth fixed base or an independent spatial constraint. This is a component-level direction, not selection of the final sustainability use case. Device counts, geometry, phone limits and unverified support are maintained in the [radio reference](Bluetooth%20and%202.4%20GHz%20Radio.md).

The [Bluetooth industry and academic research](Research/Bluetooth%20Industry%20and%20Academic%20Frontiers.md), checked 2026-10-04, expands concept exploration with current industrial applications and original papers. It distinguishes commercial/standards evidence from prototype results and does not select an idea or establish local platform support.

The [MCU and board capability reference](MCU%20and%20Board%20Capabilities.md) records manufacturer-verified specifications for nRF54LM20A/B, related family members, and the two discussed development boards. It separates silicon, board and software capabilities; physical hardware and firmware remain unverified.

The primary goal and expected solution outcomes are documented from the user-supplied competition overview. The use case and implementation scope remain to be selected.

The [application landscape research](Research/Contest%20Landscape.md) and [single inventory](Research/Application%20Inventory.md) were completely refreshed on 2026-10-05: 69 current listing IDs across five pages, matching the badge, and 70 historical total. Four new IDs were added; BuildingDNA 20417 is unlisted in this snapshot, reason unknown. Titles are verified, not implementations or winners. No use case or architecture is finalized. Future refreshes update these notes in place.

## Open questions

- Which asset or resource will be monitored, for which users, and for which degradation or failure mode?
- What sensors, data, and baseline will establish that early detection is useful?
- What intervention should an alert trigger, and how will its sustainability benefit be measured?
- What deliverables, budget, hardware availability, and constraints define the first milestone?
- Key deadlines, build/documentation requirements, and judging criteria are now summarized in [Prize categories and judging](Research/Prize%20Categories%20and%20Judging.md). Confirm entrant-specific eligibility and recheck official requirements before applying or submitting.

Capture answers here when provided; they do not block maintaining the vault.
