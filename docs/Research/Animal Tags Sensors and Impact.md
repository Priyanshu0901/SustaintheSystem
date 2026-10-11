# Animal Tags, Sensors and Impact

Related: [animal-tag GNSS comparison](Animal%20Tag%20GNSS%20Comparison.md), including LG77LICMD/L89HA and alternatives.

Research checked: **2026-10-04**. [Overview](../Overview.md) · [Animal and wildlife concepts](Animal%20and%20Wildlife%20Health%20Concepts.md) · [Hardware reference](../MCU%20and%20Board%20Capabilities.md)

## What a tag means

An identification tag, a sensor logger and a radio transmitter are different functions. A passive ID tag may report only an identifier when a reader is nearby. A logger records sensor data locally. An active telemetry tag also sends data when its particular radio link is available. GPS/GNSS positioning does not itself provide an upload connection.

Species, body size, habitat, attachment site, duration and the observation sought determine appropriate equipment. There is no universal low-impact tag. This reference informs the user's concept discussion; it does not select an animal, approve a device for animal use or initiate handling.

## Sensor measurements and derived data

The measurement column describes the instrument output. Behavior and health columns require calibration, suitable mounting, validated algorithms and independent observations; they are not additional physical measurements.

| Sensor or modality | Direct data | Possible derived information | Important interpretation limit |
| --- | --- | --- | --- |
| 3-axis accelerometer | Time-stamped specific force on x/y/z, including gravity | Activity bouts; resting/moving classes; periodic head/jaw movements; selected step features | Measures tag motion, not a disease. Collar motion, fit and attachment site influence the signal. Lying/standing or rumination are inferred labels. [T1] |
| Gyroscope | Angular velocity about three axes | Turns and rotations; movement transitions; support for orientation estimates | Requires sensor fusion and stable mounting. More data does not automatically improve generalization. [T9, T13] |
| Magnetometer | Local magnetic field vector | Heading/orientation assistance | Magnetic structures and calibration matter; does not supply absolute position. An additional sensor, not part of the discussed six-axis XIAO IMU. |
| Channel Sounding / local positioning | Pairwise radio measurements and range estimates | Position, visits to mapped resources, paths and dwell periods | Position is computed using anchors. Range quality, antenna orientation and body obstruction affect it; resource proximity is not intake. See [radio reference](../Bluetooth%20and%202.4%20GHz%20Radio.md). |
| GPS/GNSS receiver | Position/time fixes and associated quality estimates | Outdoor movement tracks, range use and travel | Receiver adds hardware/energy; satellite positioning and telemetry are separate. Indoor/covered conditions can limit fixes. [T8] |
| Contact or external temperature sensor | Temperature at its actual mounting point | Surface/microclimate trends, context for activity | Ear, skin, enclosure and silicon temperatures are not interchangeable with core body temperature. Vendor ear-temperature systems use other signals as well. [T7] |
| Rumen/reticulum bolus | Internal local temperature; some designs add pH/activity | Rumen conditions and thermal proxies in appropriate ruminants | Retained internal device; not a general collar accessory. Measurement site, feeding/drinking and age/context matter. A calf pilot found poor fever detection from individual readings. [T6] |
| Microphone/contact acoustic sensor | Audio or vibration waveform | Proposed vocalization, chewing or cough-event features | Animal source attribution, wind, friction and neighboring animals matter. A MEMS microphone alone does not validate these classifications. See [hardware reference](../MCU%20and%20Board%20Capabilities.md). |
| Noseband pressure sensor | Pressure changes associated with jaw/head movement | Chew events; rumination/eating durations | Additional facial attachment. Performance varies with algorithm and behavior; drinking is harder in some validations. [T4, T5] |
| ECG / physiological logger | Cardiac electrical signal; some include temperature | Heart rate and selected cardiac interval metrics | Reliable acquisition may need contact electrodes or implantation. Heart rate is not a unique measure of stress; motion and signal quality matter. [T8] |
| Water-contact/conductivity sensor | Wet/dry or conductivity-dependent observations | Time in water under a specified detection rule | Not a drinking-volume sensor. A 2026 polar-bear study used a wet switch on collars. [T8] |

An animal can carry more than one device, but every component and attachment adds burden. For example, the reviewed polar-bear research separated collar GPS/activity/water-contact measurements from implanted ECG/temperature logging; these were not all obtained from an ordinary Bluetooth collar [T8].

## Attachment types and intrusiveness

These are qualitative comparisons, not formal risk ratings or fitting instructions. Capture and handling are separate from the ongoing burden of wearing the tag. RSPCA's wildlife research guidance emphasizes both [T3].

| Attachment | Common use/context | Useful observations | Physical/handling burden |
| --- | --- | --- | --- |
| Neck collar | Managed cattle; species-appropriate larger terrestrial wildlife | Head/neck activity, local position/GPS; external temperature | External, but restraint/capture may be needed; rubbing, pressure, snagging, shifting mass and changing body size require consideration. |
| Ear tag | Livestock; selected wildlife applications | Identification; head movement/activity; some ear-temperature systems | New attachment can pierce tissue. Added mass, leverage and snagging matter; existing ID-tag compatibility cannot be assumed. |
| Leg-mounted unit | Particularly managed livestock locomotion work | Steps, limb motion, standing/lying patterns | Local pressure/abrasion and impact exposure. Limb-specific information cannot be assumed from a neck collar instead. |
| Harness/backpack or glued/taped external logger | Birds and selected other species | Position, motion and environmental logging | Capture plus attachment-specific effects on movement, drag, grooming and entanglement. Temporary adhesion is not automatically harmless. Bird-specific guidance: [T2]. |
| Passive PIT/RFID tag, sometimes implanted | Identification of mammals, fish and other animals; readers at gateways | ID and time/location of detection by a reader | Very small devices can still require penetration/implantation and handling. Standard PIT tags have no battery and do not continuously broadcast coordinates. [T10] |
| Rumen bolus | Suitable ruminants | Internal temperature and device-specific rumen data | Orally administered retained device; species/age suitability and professional administration matter. Battery/device servicing is constrained. [T6] |
| Implanted physiological logger | Specialist wildlife/veterinary research | Internal/subcutaneous temperature, ECG and derived heart rate | Surgical/procedural burden, wound and infection considerations; retrieval may require another capture. [T8] |

Collar, ear, leg, halter and internal-sensor systems exist in cattle research, but their output quality varies by indicator and context [T1]. A mature commercial product is not evidence that a new enclosure or mounting method is suitable.

## Species and habitat change the answer

- **Cattle and other managed large mammals:** existing handling routines and established wearable systems can make monitoring more practical. An externally attached sensor is still a burden, and the final collar/ear assembly matters more than bare-chip dimensions. Do not transfer cow behavior labels unchanged to goats, sheep or deer.
- **Free-ranging terrestrial wildlife:** fitting and removal may require capture. An end-of-deployment release/removal strategy, seasonal body changes and possible effects on dependent young are important. RSPCA guidance warns against assuming external devices are benign [T3].
- **Birds:** flight, feather condition, drag, attachment and molt can dominate effects. BTO's backpack guidance gives a less-than-3% body-mass rule of thumb for the device plus attachments, while explicitly noting that even 3% may be too much for large species [T2]. This is not a universal safety threshold or permission for other taxa.
- **Aquatic animals:** a device may influence swimming through drag even when light. In a ten-year king-penguin study, flipper bands affected survival, reproduction and foraging [T11]. This is attachment/species-specific evidence, not proof that all tags have those effects. The current local Bluetooth POC does not establish an underwater telemetry system.
- **Small mammals, bats and insects:** reducing the entire package and capture burden is especially important; a development board is not a species-ready tag. Nest/hive sensors, cameras or gateway observations may answer the question without a wearable.

Do not use a generic 3% or 5% calculation as the sole criterion. Total mass includes battery, straps, enclosure, mounting hardware and antenna. Shape, placement, attachment mechanics, duration and species biology can matter independently of mass. Clinical suitability and field fitting should be assessed by professionals familiar with the species; this research is not a handling protocol.

## Useful evidence about intrusion and data quality

- A collar validation study used a 922.4 g complete system, including a positioning weight, and fitted it at least 14 days before observations [T12]. This shows that electronics mass and habituation are part of real study design. It does not establish that this weight or acclimation period is suitable for our project.
- BTO's 2019 evidence review found attachment-related effects and substantial gaps in reporting of animal effects [T14]. "No effect reported" is different from measured absence of an effect.
- The 2026 polar-bear paper instrumented eight animals and retrieved five physiological loggers the next year; the recovered devices supplied usable ECG-derived data. Its abstract summarizes five animals, so retain the distinction between instrumented and recovered/analyzed cases [T8]. Findings in this specialist deployment cannot establish general implant safety.
- A 2026 cattle posture preprint reports poorer cross-year performance than within-year performance when fusing collar, bolus and environmental data [T9]. More sensors or excellent random-split accuracy do not establish reliable transfer to unseen animals or seasons.

## What data our proposed system could produce

**Proposed records, not implemented:** animal/tag ID; timestamp; attachment location; raw acceleration/gyro values; sensor temperature with its actual meaning; anchor IDs, distances and quality; computed position and uncertainty; time spent in mapped zones; behavior label/confidence; battery and device status. Keep sensor faults and out-of-coverage periods distinct from inactivity or mortality.

The useful hierarchy is raw measurements → validated behavior estimates → changes relative to that animal's appropriate baseline → a suggested inspection. A lower activity score does not directly measure pain, infection or stress. Changes immediately after fitting may reflect capture or the equipment itself.

An illustrative data-volume calculation: three 16-bit acceleration channels at 50 samples/s produce 300 bytes/s, or **25.92 MB/day** in decimal units, before timestamps/headers. Adding three equally sampled 16-bit gyro channels doubles this to **51.84 MB/day**. These are arithmetic examples, not a selected rate or measured battery/storage behavior. Event summaries and intermittent raw windows can reduce storage and radio traffic; retaining representative raw data helps investigate a classification error.

The [BEBE research benchmark](https://arxiv.org/abs/2305.10740) describes nine behavior-labeled animal sensor datasets with linked data/code [T13]. It offers examples of the measurements-to-labels distinction. It is not a disease dataset or a ready-to-use LM20 model. No dataset was downloaded or analyzed here. Sheep activity data is also referenced in the [concept note](Animal%20and%20Wildlife%20Health%20Concepts.md).

## Lower-burden sensing options

| Fixed observation | Data obtainable | Remaining limitation |
| --- | --- | --- |
| Existing passive ID + gateway/feeder reader | Individual visit times at monitored points | Detection at a reader, not continuous movement or physiology |
| RGB camera at a resource or enclosure | Recorded behavior; potential identity/pose estimates | Occlusion, lighting and validated identification; use of a camera does not guarantee identity continuity |
| Thermal camera | Surface-temperature patterns | Wind, sunlight, viewing conditions and site affect readings; do not equate to a core-temperature thermometer [T15] |
| Feeder load cell / trough flow and level | Resource mass/flow/level changes | Need synchronization and individual attribution; shared use/refill complicate intake estimates |
| Stationary environmental sensors | Water-point/shelter temperature, humidity and resource condition | Environmental exposure and physiology are different observations |

A hybrid is a promising design inference: keep cameras, environmental and resource sensors at fixed bases; let a compact removable animal tag contribute only the motion and identity/local-position information that cannot be obtained otherwise. This can reduce what the animal must carry, but minimum burden and equivalent data quality remain unverified.

## Relation to the current POC

The DK remains the moving main node with three fixed bases. It can demonstrate the spatial/data pipeline on a carried or mechanical surrogate without claiming an animal trial. The DK is a development board, not an established animal wearable.

The documented XIAO nRF54LM20A **Sense** has an LSM6DS3TR-C accelerometer/gyro and a PDM microphone [T16]. Those are additions to that board, not proof that the moving DK has them. The DK's documented features do not include the same Sense sensor package [T17]; motion/audio measurement on the moving DK needs appropriate added hardware. On-chip temperature is not an animal body-temperature measurement. Full hardware facts remain canonical in the [hardware reference](../MCU%20and%20Board%20Capabilities.md).

For the explored terrestrial concepts, motion plus local positioning is a useful lower-burden candidate; retain most environmental/resource sensing at fixed nodes. This is an engineering judgment, not a selected final sensor set. No assembly weight, tag fit, radio/behavior performance, battery life or animal impact has been measured.

## Sources and evidence limits

| ID | Reference | Type / use |
| --- | --- | --- |
| T1 | [Wearable wireless biosensor technology for monitoring cattle, 2021](https://pmc.ncbi.nlm.nih.gov/articles/PMC8532812/) | Published research review; attachment/output taxonomy and validation variation |
| T2 | [BTO Special Methods, chapter 8](https://www.bto.org/get-involved/volunteer/projects/bird-ringing-scheme/taking-part/resources-ringers/hub/guidance-materials/ringing-guidance/chapter-8) | Bird-specific attachment guidance checked 2026-10-04; not local law or generic safety rule |
| T3 | [RSPCA/APHA wild animals in research report, 2015](https://www.rspca.org.uk/webContent/staticImages/Downloads/2015WildAnimalsInResearch.pdf) | Professional welfare guidance; capture/handling and continuing burden |
| T4 | [Noseband pressure sensor specification/validation, Agroscope, 2017](https://ira.agroscope.ch/en-US/publication/36542) | Original study metadata/abstract; jaw-motion observation |
| T5 | [RumiWatch converter validation, 2020](https://pubmed.ncbi.nlm.nih.gov/32011911/) | Original study abstract; algorithm- and behavior-specific limits |
| T6 | [Rumen bolus pyrexia pilot in calves, 2016](https://www.sciencedirect.com/science/article/pii/S0022030216306956) | Original study abstract; distinguishes local temperature accuracy from clinical detection |
| T7 | [CowManager nutrition module](https://www.cowmanager.com/cow-management/modules/nutrition/) | Manufacturer description of ear temperature and derived behavior; not independent product validation |
| T8 | [Pulse of the Arctic, Animal Biotelemetry, 2026](https://link.springer.com/article/10.1186/s40317-026-00459-z) | Original specialist implantation/GPS study; read methods/results as well as abstract |
| T9 | [Cattle posture under animal/time distribution shift, 2026 preprint](https://arxiv.org/abs/2606.24986) | Original preprint; does not establish performance of our models |
| T10 | [USGS PIT interrogation study report](https://pubs.usgs.gov/publication/70041091) | Authoritative primary report on passive identification/reader monitoring |
| T11 | [King-penguin flipper bands, Nature, 2011](https://www.nature.com/articles/nature09630) | Original long-term study; qualitative effects only here, avoiding numerical results affected by a correction |
| T12 | [Commercial collar eating/rumination validation, 2021](https://pmc.ncbi.nlm.nih.gov/articles/PMC8532760/) | Original study; complete collar mass and study acclimation |
| T13 | [BEBE benchmark, paper first posted 2023, revised 2024](https://arxiv.org/abs/2305.10740) | Original research/data benchmark; behavioral rather than clinical labels |
| T14 | [BTO tracking effects evidence review, 2019](https://www.bto.org/our-work/science/publications/papers/effects-tracking-devices-individual-birds-review) | Author organization summary of research review; absence of reporting is not absence of harm |
| T15 | [Environmental effects on cattle infrared eye temperatures, 2014](https://www.sciencedirect.com/science/article/pii/S0034528813003664) | Original study abstract; wind, sunlight and measurement-condition effects |
| T16 | [Seeed Sense onboard sensors](https://wiki.seeedstudio.com/xiao_nrf54lm20a_with_onboard/) | Official board documentation checked 2026-10-04 |
| T17 | [Nordic nRF54LM20 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF54LM20-DK) | Official DK feature description checked 2026-10-04 |

Some NCBI/publisher direct retrievals were challenged or failed. Limited claims use primary abstracts available through search, accessible author/institutional descriptions or opened official pages. No exhaustive animal-welfare review, product comparison, physical hardware inspection, dataset evaluation or animal experiment was performed.
