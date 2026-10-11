# Animal and Wildlife Health Concepts

Research checked: **2026-10-04**. [Overview](../Overview.md) · [Project](../Project.md) · [Radio geometry and phone limits](../Bluetooth%20and%202.4%20GHz%20Radio.md)

For attachment options, direct measurements, inferred behavior and animal burden, see [Animal tags, sensors and impact](Animal%20Tags%20Sensors%20and%20Impact.md).

## Scope and evidence

Priority correction, 2026-10-05: find a simple useful solution by understanding a complex real-world problem and deriving the minimum evidence needed for a useful action. [Project](../Project.md) owns this current direction. The earlier agent framing of Bluetooth/data reliability as the core problem statement is superseded; [sparse herd telemetry and recovery](Sparse%20Herd%20Telemetry%20and%20Recovery.md) remains supporting engineering research. Non-intrusive here means low observation/data burden. Detailed behavior labeling, comprehensive monitoring and tagless observation are not selected requirements; each candidate below still needs a concrete problem and requirements before a solution is chosen.

These are proposed concepts for ambitious discussion, not a selected application, diagnostic system or implementation plan. The user retains planning direction. The accepted component POC uses three fixed bases and a moving main device; if received, the DK is the moving node. Final field tags, species, sensors and mounting are undecided.

The saved [contest inventory](Application%20Inventory.md) already includes titles for cattle health/estrus monitoring and wildlife protection. Those title-level observations do not establish implementation details, current participation or novelty. The opportunity considered here is a specific health observation leading to a useful intervention, rather than claiming novelty for a generic collar.

Position gives spatial context; an added IMU provides movement signals; resource and environmental sensors provide independent context. Location near food or water is not evidence of intake. A single moving node can demonstrate individual observations, while herd interactions and comparisons require additional animal tags or another observation system.

## 1. Health through resource access

**Proposed core:** explain an animal's changing routine in terms of its access to food, water, shade and rest. Distinguish plausible reasons for a change and suggest the next inspection instead of outputting a disease label.

Illustrative hypotheses:

- Reduced movement and altered resource use in one animal: inspect that animal for injury or illness.
- Repeated approach/retreat at a feeder, coinciding with other animals' occupancy: inspect competitive exclusion or crowding.
- Similar changes across animals at one trough, together with measured low water level/flow: inspect the shared water system.
- Changed shade and water use alongside environmental measurements: inspect heat exposure and resource accessibility.

Localization supplies approach paths and dwell times. An IMU and resource instrumentation help test the hypotheses. Group-level interpretation needs multiple observations; proximity alone cannot prove aggression or cause. Baselines must account for routines, weather and management changes. The intended sustainability benefit is earlier care and repair of shared resources; benefit and diagnostic lead time are unverified.

**Research basis:** a 2020 cattle study combined RFID, collar acceleration and a flow meter to observe drinking behavior and herd intake [S2]. A separate 2014 study linked sequences from electronic feeding equipment to video-observed competitive replacements [S3]. These support observable components, not the proposed integrated cause-ranking system.

## 2. Early mobility deterioration

**Proposed core:** flag persistent changes in how an animal moves between familiar resources, supported by IMU features and its own baseline. Candidates include slower journeys, changed resting patterns and altered feeding routines. Locomotion assessment would then be performed by the animal's caretaker or veterinarian.

This could apply to cattle or, after species-specific validation, larger animals in managed wildlife settings. The system could track changes after care as well as deterioration. Local position is useful for route and resource context; a collar alone must not be claimed to measure limb-level gait symmetry or to identify an injured limb.

**Research basis:** a 2018 dairy study used position plus acceleration on 19 cows over five days and found feeding-duration differences associated with lameness [S1]. It supports behavior association, not a validated pre-symptom prediction system. Collar placement and species affect transferability. Actual feed intake was not directly measured.

## 3. Wildlife waterhole health sentinel

**Proposed core:** jointly monitor the health of a water point and changes in wildlife use. Fixed bases around a local water point could observe tagged animals; a camera could cover untagged visitors. Add water level/flow, temperature and appropriate water-quality measurements as separate inputs.

Potential outputs include repeated unsuccessful water access, changing visitation schedules, crowding and a resource fault coinciding with several animals changing their behavior. A ranger could inspect the water supply or take a sample. Sensor readings and behavior do not identify contamination or infection without independent evidence.

The ambitious value is understanding the water point as an animal-resource system. The ecological outcome is not automatically "add more water": aggregation can also affect competition and exposure. A 2025 Kruger study found species-dependent changes in waterhole use and temporal overlap during drought [S4]. It supports monitoring ecological context, not a validated Bluetooth health detector or proof of disease transmission.

This is local coverage at an interaction point, not continuous tracking across a reserve. Sensor silence could mean the animal left coverage, a failed device or another cause; it cannot alone establish harm.

## 4. Recovery monitoring in wildlife rehabilitation

**Proposed core:** create a longitudinal record of a recovering animal's voluntary activity, movement between resources, feeding opportunities and rest within a rehabilitation enclosure. Highlight progress, plateaus and regression to help carers evaluate treatment and determine what to observe next.

Use species-specific functional tasks and observations rather than a universal activity score. Increased activity can be agitation, while rest can be normal recovery. The system would inform professional assessment; it would not automatically declare an animal fit for release. For terrestrial animals this can use the initial local 2D geometry. Flight or climbing studies need additional spatial sensing rather than assuming three anchors provide unrestricted 3D.

**Research basis:** a 2007 study followed one rehabilitated monk seal using satellite telemetry and measured changes in diving behavior [S6]. That is evidence that functional behavior can be followed over recovery-related periods. A terrestrial enclosure system and prerelease decision support are our proposals; this marine, postrelease case does not validate them. Bluetooth is not proposed for underwater ranging.

## 5. Wildlife sickness-behavior sentinel

**Proposed core:** selected tagged animals learn a time-of-day activity baseline locally and store persistent deviations. A receiver at a water point, den entrance or managed enclosure collects summaries when the animal comes within range. Unusual behavior prompts targeted observation or sampling.

This concept differs from continuous localization: the IMU may keep observing outside anchor coverage, but Bluetooth data delivery and precise local positioning are available only where a link works. Immediate remote alerts over a wide reserve would require another communication path. The current single-node POC would not validate population surveillance.

**Research basis:** a 2023 experiment instrumented 12 wild boars and reported reduced activity during African swine fever infection, supporting sickness-behavior sensing [S5]. Reduced activity is nonspecific; the proposal must not report a particular pathogen from motion alone or transfer this result unchanged to other species. No disease exposure experiments are proposed.

## Comparative judgment

| Concept | Main insight sought | Role of local positioning | Main unresolved inference |
| --- | --- | --- | --- |
| Resource access | Individual concern versus shared-resource/access problem | Approaches, departures and resource occupancy | Whether the proposed cause is correct |
| Mobility deterioration | Persistent change in functional movement | Routes, travel time and behavior context | Whether the change indicates injury/illness |
| Waterhole sentinel | Resource condition alongside animal response | Local access patterns at one site | Cause of changed visitation and ecological outcome |
| Rehabilitation | Recovery trajectory and regression | Enclosure use and functional journeys | Relation to clinical recovery and release readiness |
| Sickness sentinel | Departure from an individual's routine | Context and data collection at interaction points | Distinguishing sickness from normal disturbances |

Current conceptual preference: **resource access** offers an ambitious integrated livestock direction, and **waterhole sentinel** offers the strongest direct animal-resource conservation story. Rehabilitation is a distinct managed-wildlife option. These are judgments from the examined mechanisms, not user selections or established competition superiority.

## Cattle target, dog surrogate and reduced-label learning

User direction, 2026-10-04: address individual monitoring and care at herd/wildlife scale. Dog access is a surrogate testing opportunity; no farm access currently. This supersedes the agent's household-pet product recommendation. The final species and health/resource question remain user-led and unselected.

### What can transfer from a dog demonstration?

| Component | Dog/bench evidence can establish | Cattle/wildlife evidence still needed |
| --- | --- | --- |
| Radio, timestamps, storage and data collection | Synchronization, packet loss, reconnection, local logging, representative power measurements | Herd-scale simultaneous traffic, field propagation, infrastructure coverage and deployment runtime |
| Position and mapped resource events | Accuracy/uncertainty at tested heights, approach/departure/dwell logic against measured paths/video | Cow body occlusion, animal density, antenna/mounting, larger pens/pasture and resource geometry |
| Resource sensors and event association | Mass/flow sensing, spill/refill handling and pairing observations with independently identified visits | Multiple users, resource losses/refill dynamics and attribution of actual individual intake |
| Unlabeled motion features and individual baseline software | Whether the pipeline represents recurring dog activity, tracks changes and handles missing data | Whether learned features and baselines work on cattle data and separate management/weather changes |
| Automatic label generation and review tools | Agreement with independently annotated dog events, label coverage and synchronization | Cattle-specific event interpretation and audits on real cattle observations |
| Disease and species-specific behaviors | No cattle claim from dog testing | Rumination, grazing, estrus, lameness thresholds, disease lead time and clinical outcomes require appropriate species data |

Dog motion is valid dog data; it should not be relabeled as cattle data. Keep separate data provenance/species, attachment site, axes, sampling and reference labels. A system can execute the same logic on both while requiring different parameters/models. A one-dog demo cannot test herd exclusion/competition or validate species-specific physiology. The moving DK is a carried/surrogate radio prototype; wearable fit remains a separate issue.

### Four distinct ways to reduce annotation

1. **Self-supervised representation learning:** learn structure from unlabeled acceleration, for example by reconstructing masked portions or matching two appropriate views of a signal. This can initialize a model with fewer labeled examples. It does not attach names such as rumination or disease to patterns by itself. Fine-tuning and evaluation still need species-appropriate evidence. BEBE supports label-efficient learning [S12]. A 2025 study pre-trained on cattle/sheep/horse data and then evaluated a labeled goat task; its unlabeled corpus was simulated by removing existing labels [S8]. These are useful precedents, not zero-label cattle validation or a ready-to-run MCU model.
2. **Individual baseline/anomaly detection:** learn recurring activity/resource patterns and report persistent deviations without naming every activity. This can start without behavior annotations, but an initial baseline is not proof of health. Weather, management, growth, fitting changes, data dropout and disease can all change signals. Keep observation quality separate from activity; avoid automatically adapting a persistent deterioration into the new normal. Useful-alert thresholds and health significance still need independent validation.
3. **Independent event labels / weak supervision:** fixed stations provide provisional labels from measured physical events. A feeder scale plus confidently identified single-animal use can support a consumption event. Water-contact/flow/mass measurements plus verified source attribution can support a drinking event. Gate detection supplies passage, not eating or drinking. Individual electronic-bin measurements have been evaluated in cattle [S14]; RFID/flow/collar combinations also exist [S2]. Our proposal is to use calibrated event records to supervise portions of another stream, with confidence and unknown states. It is an engineering inference, not a published validation of our learning system.
4. **Teacher predictions and selective review:** a camera/model or rules can propose labels for sensor windows; sample uncertain cases and a random selection of apparently confident cases for human audit. Camera predictions are model-generated labels, not unquestioned ground truth. Pseudo-labeling the same model's predictions can reinforce mistakes. Training and validating against the same label generator measures agreement with itself, not true behavior accuracy.

The learning method and physical intrusiveness are separate choices. Self-supervised learning does not require an implant, but does not make a collar harmless. Automated video labeling can reduce labor without requiring body-mounted sensors; identity, occlusion and audit remain important.

### A concrete automatic-label example

Proposed station record: a known animal is the sole verified user during an interval; independent resource measurements show a stable net change consistent with use; no refill, spill, sensor fault or competing visitor is observed. Associate that interval with the animal's motion stream and store the **evidence and confidence** alongside the provisional label. Leave ambiguous time windows unlabeled. Food/water station events label only those behaviors at those stations; they do not automatically label pasture grazing, rumination or all-day health.

If two animals share a trough, aggregate water loss cannot be assigned to one merely because its tag is nearest. Flow from an automatic refill valve is not instantaneous drinking, and a load-cell change may be spillage or a pushed bowl. Correct timing, water balance, station design and identity matter. A drinking-duration label is different from measured volume. Even multi-camera cattle research reports false drinking detections when animals stand near troughs [S11].

Useful data fields: source ID/species, timestamp/interval, sensor placement, measured event, generating rule/version, confidence, ambiguity/rejection reason and audit result. Do not merge uncertain pseudo-labels with audited observations as if equally reliable. Clinical outcomes remain a distinct source of labels.

### Existing cattle data allows work without owning cattle

| Public source checked | What is available | Limits and potential use |
| --- | --- | --- |
| [AcTBeCalf](https://zenodo.org/records/13259482) [S9] | 30 pre-weaned calves, 25 Hz collar acceleration, 27.4 h behavior-aligned data, 23-behavior ethogram; calf and segment IDs in CSV; downloadable data/code listed | A real calf behavior source for model/replay work. Thirteen-week attachment period does not mean thirteen weeks of released labeled data. Calves differ from adult grazing/dairy cows; behavior labels are not clinical disease labels. License detail was not resolved by the rendered record; do not infer it from a repository default. |
| [Japanese Black Beef Cow dataset](https://zenodo.org/records/5849025) [S10] | Six cows, 25 Hz neck acceleration; video-derived behavior labels and explicit unlabeled intervals; released ZIP listed | Small, one-day, highly imbalanced dataset: some behaviors appear in very few animals. Useful for adult-cow exploration and withheld-label experiments; not broad herd validation. Record states CC BY-NC-ND 4.0, so check reuse terms for the intended deliverable before execution. |

No dataset was downloaded, inspected at row level or evaluated. These releases demonstrate availability of real cattle sensor/behavior records, not automatic alignment to our hardware. They do not provide the matched live position/resource/clinical streams needed to validate the full concept. Do not fabricate synchronized multimodal evidence by pairing unrelated records.

For a label-efficient benchmark, split by animal/session first, then expose only a controlled fraction of training labels. Compare supervised-only, self-supervised initialization and weak-label variants on an untouched independently labeled test set. Report label/audit effort, accepted-label precision and coverage, per-class performance, unknown/rejected fraction, nuisance alerts and energy. Any adaptation using unlabeled test-animal data should be reported separately from evaluation on completely unseen animals. Test cohorts/seasons where available: a 2026 cattle preprint shows that good within-year fusion results can deteriorate markedly across years [S13]. A public-data model running on the MCU establishes inference feasibility on those recordings, not on-animal field accuracy.

### Less intrusive observation options

- **No new body-mounted device:** cameras and fixed resource sensors at troughs, feeders or managed enclosure entrances. Identity could use visual recognition or an existing compatible ID tag. No new tag is not the same as no pre-existing tag. A 2026 cattle camera study associates identity with behavior [S11], but this is a trained vision system with limited scene coverage. A dog demo tests workflow/timing, not cattle visual identification. Fixed electronics must also avoid obstructing ordinary resource access.
- **Minimal external tag:** an appropriate compact identity/motion/radio assembly, with richer sensors at fixed stations. This can extend observations beyond the camera view; mounting and total burden remain subject to species-specific assessment in the [impact reference](Animal%20Tags%20Sensors%20and%20Impact.md). It is a proposal, not an approved tag.
- **Station-only wildlife observation:** low-contact monitoring at naturally used points can avoid capture, but only observes visitors. Missing detection may mean missed coverage/identity, not illness or death. Large territories and rare visitors limit individualized baselines and immediate alerts.

Bluetooth Channel Sounding ranges between participating radios. Removing animal-carried radios means that it no longer supplies direct per-animal position. A fully tagless concept would use other sensing for animals and Bluetooth for station data; this is an architectural alternative, not a silent change to the accepted moving-DK localization POC.

### Proposed research focus

Agent suggestion, not final selection: **learn individual routines with fewer manual labels, use independent resource observations to verify selected events, and rank animals/resources that need human inspection while admitting unknown evidence**. This preserves the scale-of-care problem. Self-supervision, personalized baselines and sensor fusion already exist; distinctiveness would need to come from demonstrated label savings, robustness, reliable attribution and useful intervention prioritization.

Separate three evidence tracks: live dog/bench tests for common components; real recorded cattle data for cattle behavior/model research; later cattle/wildlife observations for field and clinical claims. Synthetic bench faults or replay can test alert plumbing and resource-fault logic, but cannot establish illness detection or herd care outcomes. No implementation, animal procedure or clinical experiment has been initiated.

## POC and research boundaries

Market history and dog/bench testing examples remain in [pet products and household validation](Pet%20Products%20and%20Household%20Validation.md); its household-product recommendation is superseded by the user's cattle/wildlife target and dog-surrogate clarification above.

- Three fixed bases plus moving DK demonstrate a local constrained-2D component. Precision, coverage, motion handling and local firmware are untested; a fourth base or independent constraint is required for general 3D.
- An IMU or environmental sensor is an added component; do not imply the DK alone contains every needed sensor. Custom base boards remain a later user intention with unverified cost/performance.
- The DK is a development platform. Final animal use requires an appropriate tag and species-specific mounting; demonstration of a carried DK or surrogate trajectory does not establish animal-health performance.
- The CMF Phone 1 is a possible BLE dashboard, not a verified Channel Sounding node. See the canonical radio reference.
- Food/water dwell does not measure consumption; enclosure ambient temperature does not measure core body temperature. Proximity does not establish physical contact, aggression or pathogen transmission.
- Biological validation needs independent observations and clinically assessed labels, separate from positioning or sensor benchmarks. No such validation has been performed.

## Primary references

| ID | Source | Evidence used and limit |
| --- | --- | --- |
| S1 | [Position and acceleration for lameness-associated feeding behavior, Journal of Dairy Science, 2018](https://www.sciencedirect.com/science/article/pii/S0022030218303734) | Small observational dairy sample; behavior association rather than validated early prediction. Publisher abstract/index accessible in search; direct page retrieval blocked. |
| S2 | [Sensor-based monitoring of cattle drinking behavior and water intake, 2020](https://www.sciencedirect.com/science/article/pii/S0168169919314875) | Combined RFID, acceleration and flow measurements; supports separating visit/drinking inference from measured herd intake. Abstract accessible in search; direct retrieval failed. |
| S3 | [Automatic detection of social competition using an electronic feeding system, 2014](https://pubmed.ncbi.nlm.nih.gov/24630661/) | Competitive replacement estimates evaluated against video; not validation of Bluetooth proximity as aggression detection. Abstract accessible in search; direct retrieval challenged. |
| S4 | [Effect of drought on wildlife activity at artificial waterholes, Biological Conservation, 2025](https://eprints.gla.ac.uk/365874/) | Original study metadata/abstract in author's university repository; camera-trap ecological context, not tag health prediction. |
| S5 | [Accelerometer-based detection of African swine fever infection in wild boar, 2023](https://pubmed.ncbi.nlm.nih.gov/37644835/) | Original study abstract; species/disease-specific experiment, not general disease diagnosis. |
| S6 | [Diving development and behavior of a rehabilitated monk seal, 2007](https://research-portal.st-andrews.ac.uk/en/publications/diving-development-and-behavior-of-a-rehabilitated-mediterranean-/) | Original study abstract in university repository; one animal, satellite telemetry and postrelease marine behavior. |
| S7 | [EweAcT behavior-aligned accelerometer dataset, 2026 preprint](https://arxiv.org/abs/2608.09943) | Describes labeled sheep activity data across conditions; potential behavior-classification research resource, not disease labels or an LM20 model. |
| S8 | [Cross-species self-supervised animal activity learning, Scientific Reports, 2025](https://www.nature.com/articles/s41598-025-27736-w) | Original study: unlabeled pre-training followed by species-specific labeled fine-tuning; target task goats, simulated unlabeled corpus. Not zero-label cattle validation. Methods/source excerpts opened; no reproduced results. |
| S9 | [AcTBeCalf author dataset record](https://zenodo.org/records/13259482) | Opened description, schema and listed downloadable files; calf behavior data, not disease-ground-truth or local hardware results. No download or record-specific license verification. |
| S10 | [Japanese Black Beef Cow author dataset record](https://zenodo.org/records/5849025) | Opened collection, annotation, class distribution, files and explicit license; limited animals/day and rare labels. No download. |
| S11 | [Multi-view cattle identity and behavior recognition, PLOS One, 2026](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0340999) | Original camera study; identity/behavior association and false-drinking/occlusion limits. Does not validate untrained automatic labels or tagless Bluetooth ranging. |
| S12 | [BEBE benchmark, Movement Ecology, 2024](https://pmc.ncbi.nlm.nih.gov/articles/PMC11654173/) | Original benchmark supports label-efficient representation learning; not cattle clinical validation. |
| S13 | [Cattle fusion under temporal distribution shift, 2026 preprint](https://arxiv.org/abs/2606.24986) | Original preprint distinguishes within-year and unseen-cohort/year performance. No reproduced/local result. |
| S14 | [Individual feed/water-bin validation, Journal of Dairy Science, 2007](https://www.sciencedirect.com/science/article/pii/S0022030207720489) | Original abstract available in primary search; direct retrieval 403. Supports independent station measurements, not our proposed automatic-label training pipeline. |

EweAcT describes 79 hours of behavior-aligned acceleration from 120 ewes, including grazing, rumination, rest and movement [S7]. It may help explore behavioral features, but species, collar mechanics and sampling must be reconciled before transfer. It is not evidence that the proposed health concepts work.
