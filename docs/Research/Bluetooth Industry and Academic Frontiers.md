# Bluetooth industry applications and academic frontiers

Research checked: **2026-10-04**. This is a selected landscape of industrial applications, recent developments and original academic work, emphasizing 2025–2026 with an older foundation where useful. It is not an exhaustive literature review or a project implementation plan.

[Overview](../Overview.md) · [Project direction](../Project.md) · [Bluetooth and radio fundamentals](../Bluetooth%20and%202.4%20GHz%20Radio.md) · [Contest landscape](Contest%20Landscape.md)

## Main finding

Bluetooth supports several different roles: transporting sensor measurements, locating objects, coordinating many devices, broadcasting audio, and—experimentally—sensing physical changes through radio propagation. Industry examples increasingly connect observations to operational decisions across a whole facility or supply chain. Academic work challenges the energy, sensing and coordination limits of the radio itself.

The sections below distinguish operator/vendor reports, adopted specifications, developer products and research demonstrations. Numerical results are attributed to their sources, not independently replicated here. Feature adoption does not establish support on our boards; see the existing radio reference for platform boundaries.

## Industry: applications and what is changing

| Application | Named evidence | Bluetooth's actual role | Evidence maturity and boundary |
| --- | --- | --- | --- |
| Supply-chain and cold-chain visibility | Wiliot/Walmart announcement, 2025-10-02 [I1]; Gen3 Pixel launch, 2026-01-08 [I2] | Battery-free tags broadcast BLE data; infrastructure and cloud software interpret goods' location and condition | Vendor reports millions of sensors and 500 Walmart locations at announcement time. 90 million pallets by end-2026 was a goal, not a verified achieved count. Gen3 is a product announcement. |
| Reusable logistics assets | Royal Mail's technology page [I3]; Wiliot's deployment account, 2025-11-20 [I4] | Tags on rolling containers replace some manual scans with automatic observation | Operator describes tagging 850,000 York containers; Wiliot later describes about 900,000 rolling cages. Different dated reports, not one reconciled current count. |
| Industrial location and process visibility | Quuppa/InPlay announcement, 2026-03-16 [I5] | Angle-of-arrival positioning of compact BLE tags using locator infrastructure | Vendors announce sub-meter positioning for thin tags. This is an ecosystem announcement, not an independently tested accuracy guarantee. |
| Lighting and HVAC coordination | Adopted HVAC Integration NLC Profile [I6]; SIG deployment discussion, 2026-08-27 [I7] | Mesh occupancy messages from lighting sensors inform compatible thermostats; lighting also responds to occupancy/daylight | Standard exists. Reported lighting savings are installation-specific; they do not establish that HVAC integration produced the same percentage savings. |
| Electronic shelf labels and distributed displays | Bluetooth ESL explanation [I8]; ST STEVAL-ESL1KCB [I9] | PAwR schedules commands and responses for many sleeping devices; labels receive display updates | Adopted architecture plus a manufacturer developer kit. Not every deployed ESL uses Bluetooth or the standardized ESL Profile. |
| Digital keys and spatial access | CCC account, 2026-06-18 [I10]; NXP Channel Sounding devices, 2026-02-04 [I11] | BLE participates in discovery/communication; Channel Sounding can add distance awareness | CCC discusses BLE, UWB and NFC together. Its UWB distance bounding must not be relabeled as BLE ranging. NXP documents MCX W72/KW47 support; no universal car deployment claim. |
| Public audio and accessibility | Fraport announcement, 2026-01-28 [I12] | Auracast broadcasts announcements to compatible receivers without individual ordinary connections to every listener | Frankfurt trial at Terminal 1 gates A16/A17, initially evaluated over two months. This source does not verify an airport-wide rollout or subsequent trial outcome. |
| Industrial condition monitoring | ABB Smart Sensor material [I13] and customer-case collection [I14] | Sensor data travels through BLE to a phone/gateway and onward to analytics | Established commercial category with customer cases. Wireless vibration/temperature monitoring is an industry baseline, not itself a novel core concept. |
| Wearable medical telemetry | Dexcom G7 manufacturer FAQ [I15] | Sensor communicates directly with a compatible Apple Watch through Bluetooth | Commercial product example. Protocol connectivity is distinct from medical measurement validity; no clinical recommendation or new performance claim is made. |

### Supply chains: item condition and system behavior

Wiliot's Gen3 description distinguishes RF energizing from BLE data transmission: its architecture uses 2.4 GHz and sub-1 GHz for expanded energizing/read operation, while broadcasting encrypted BLE signals. Battery-free does not mean infrastructure-free or that weak ambient RF always suffices. Reusable asset tracking, automated receiving and shipment verification are operational uses, not simply dashboards. [I2]

**Interpretation for concept exploration:** physical condition, location and timing together can expose resource waste that a single machine sensor cannot explain. This does not prove a causal model, remaining-life prediction or a unique contest opportunity.

### Buildings: existing sensors become shared infrastructure

The HVAC Integration NLC Profile defines a thermostat subscribing to occupancy sensors in a Bluetooth mesh. This enables reuse of the lighting sensor network rather than requiring separate occupancy sensing for every subsystem. [I6]

The SIG's August 2026 article reports an **83% total lighting energy reduction** at Woluwe Shopping Centre compared with the original installation, combining LED upgrades and controls. Treat it as a reported case result with its own baseline, not a transferable Bluetooth efficiency figure. [I7]

**Interpretation:** interoperability between subsystems can matter as much as a better individual detector. It creates room to explore interactions between energy use, occupancy and system condition without selecting a building concept here.

### Retail scheduling is useful beyond price tags

The ESL profile's addressing permits **32,640 labels** per network/access point. That is an addressing limit; it does not promise that every label updates simultaneously at arbitrary latency. PAwR offers scheduled two-way exchanges and EAD protects advertising data. The ST kit demonstrates PAwR-based display control. These mechanisms are useful inspiration for coordinated fleets, but an application still needs scheduling and delivery logic. [I8, I9]

## Recent standards and development status

| Development | Verified source/date | What it changes | What not to assume |
| --- | --- | --- | --- |
| Channel Sounding | NXP's 2026-02-04 developer explanation [I11] | Phase/time measurements enable distance-aware interactions beyond RSSI proximity | Distance alone does not provide direction, a 3D map or sensing of material damage. |
| Shorter Connection Intervals | Core 6.2 announcement, 2025-11-04 [S1] | Specification minimum can reach 375 microseconds | This is not guaranteed end-to-end application latency, nor universal controller/phone support. |
| Core 6.3 | Specification date **2026-05-05** [S2]; SIG technical overview [S3] | Includes Inline PCT Transfer and PHY-specific RTT accuracy reporting for Channel Sounding | Inline transfer requires supporting hardware/features; it is not merely an application algorithm update. |
| High Data Throughput | SIG article, 2026-07-28 [S4] | Describes an upcoming PHY up to 7.5 Mbps with new modulation, coding and packet handling | Article describes planned adoption in a later-2026 release. Not verified as adopted/shipping by this review; no LM20 support claim. Bit rate is not application goodput. |
| Additional spectrum | SIG roadmap talk, 2026-06-04 [S5] | Work toward 5/6 GHz operation and expanded audio capabilities | Roadmap activity is not a capability of existing 2.4 GHz boards. |

Existing [radio documentation](../Bluetooth%20and%202.4%20GHz%20Radio.md) remains authoritative for the previously checked hardware/SDK distinctions. This research does not upgrade or validate its SDK support matrix.

## Academic research: selected original papers

### 1. Device-free occupancy sensing from BLE signals

**Iannizzotto, Lo Bello and Nucita — Improving BLE-Based Passive Human Sensing with Deep Learning**, *Sensors* 23, 2581, **2023**. A convolutional network classifies radio-signal variations to detect occupants without requiring them to wear tags, including cases without direct obstruction of the transmitter–receiver line of sight. [A1]

This is older foundational work, despite a 2026 arXiv upload. It supports radio-based presence sensing, not arbitrary material inspection, human identification or a general damage detector. A camera-free measurement system is not automatically privacy-preserving.

### 2. Coarse material and obstruction sensing with commodity radios

**Tripathi, Tiwari and Kadam — Low-Cost Device-Free Material and Obstruction Identification in IoT Systems Using Commodity Bluetooth Low Energy RSSI**, preprint **2026-07-31**, under review. Two nRF5340 kits use calibrated RSSI statistics and a threshold classifier. Human body, glass and wood are correctly identified in reported unknown trials; paper and brass map incorrectly to trained classes. [A2]

The manuscript acknowledges limited sessions and incompletely recorded geometry/material conditions. This is a coarse laboratory demonstration; it does not establish robust material identification, moisture measurement or structural-failure prediction.

### 3. IMU-assisted ranging correction

**Bapat and Nagaraj — IMU-Aided Correction of Orientation-Induced Ranging Error in Bluetooth Channel Sounding on Commercial Hardware**, preprint **2026-09-01**. EFR32xG24 kits combine ranging and IMU features across nine distances and nine orientations. Random Forest correction reports **74.6% MAE reduction** when withholding an orientation, but **16.1%** when withholding a distance. [A3]

The reflector remains fixed in one indoor corridor. Unseen-room and moving-device generalization are unproven. The useful research theme is contextual correction of radio measurements; the result is not a universal accuracy multiplier.

### 4. Coordinated connectionless ranging networks

**Schex, Cremer and Dettmar — Connectionless Bluetooth Channel Sounding via PAwR for Scalable and Energy-Efficient Ranging**, preprint first submitted **2026-05-16**, v3 **2026-09-23**. An nRF54L15 prototype combines LE CS Test with PAwR scheduling and changing device pairings. For one partner at roughly 1-second updates, measured steady-state active charge falls by up to **25%** against real-time reporting or **48%** against on-demand reporting baselines. [A4]

Its **14,080-device** result is a capacity projection at approximately **40-second** updates, not a deployment. Test mode bypasses normal CS security establishment; security extensions remain future work. Peer coordination, inference and trust are separate problems.

### 5. BLE-compatible backscatter connections

**Dong et al. — PassiveBLE: Towards Fully Commodity-Compatible BLE Backscatter**, **2025** manuscript, listing ACM MobiCom 2025. A tag modulates an externally supplied RF signal; modified excitation firmware handles much of the coding and connection scheduling. Authors report over **99.9% connection-establishment success** and up to **974 kbps goodput**. [A5]

The often-highlighted **9.9 microwatts** is simulated ASIC uplink power, not whole-system measured consumption. The discrete prototype has additional synchronization/standby costs and requires an excitation source. An ordinary BLE development board does not become a backscatter tag through software alone.

### 6. Energy-adaptive battery-free wearables

**Chen and Rodriguez Villegas — An energy-aware, self-adaptive, battery-free smart wristband for long-term health monitoring**, *IEEE Access*, **2025-07-30**. Solar harvesting, a supercapacitor, local processing and adaptive sampling share one energy budget. Authors report sustainable **50 Hz** sampling with **1.45 hours/day at 1,000 lux**, or **200 Hz** with **4.74 hours/day**, under evaluated conditions. [A6]

The contribution is adaptation to available energy and sensing needs, with BLE transmitting processed information. These illumination and workload conditions are essential; the results do not imply indefinite operation in darkness.

### 7. Authentication tied to wireless power infrastructure

**Djidjekh, Bernyer and Takacs — Secure Polarization-Shift Backscatter Identification Applied to Battery-Free BLE Sensors Powered by Wireless Power Transfer**, arXiv upload **2026-09-24**, listing *IEEE WPTCE, July 2026*. An external RF-switch/antenna module backscatters an AES-128-encrypted identifier on the power wave before BLE data exchange. Authors demonstrate coexistence with BLE advertising. [A7]

This is an external power/security architecture, not a replacement for BLE's complete security model. Encrypted identification alone must not be interpreted as proof of resistance to every replay or impersonation attack.

### 8. BLE in ingestible electronics

**Zhou et al. — Reevaluating Bluetooth Low Energy for Ingestible Electronics**, preprint first submitted **2026-01-27**, v2 **2026-09-08**. Phantom/in-vivo experiments compare BLE and selected sub-GHz alternatives. The authors report stronger 2.4 GHz tissue attenuation, but FEM-assisted BLE at **20 dBm** is more energy-efficient for evaluated sensing workloads below **100 kbps**. [A8]

This is specialized research hardware, not evidence that stock LM20 boards work inside the body or that all sub-GHz designs perform worse. It illustrates how antenna, front-end, protocol and processing choices jointly determine viability.

### 9. Miniaturized crystal-free sensor motes

**Hippe et al. — A Digital Twin Platform Enabling Monolithic Crystal-Free Bluetooth Low Energy Single-Chip Sensor Motes**, preprint **2026-05-22**. SDR/FPGA and chip-front-end configurations validate novel BLE device architectures before fabrication; demonstrated systems communicate with commodity BLE devices. [A9]

Here the digital twin is a wireless-chip validation environment, not an operational factory twin. The work concerns sensing-network hardware scale and fabrication risk, not a ready-to-use firmware feature.

## What these findings open up

The following are synthesis and concept prompts, not demonstrated project capabilities or chosen ideas:

| Direction | Research/industry bridge | Ambitious question |
| --- | --- | --- |
| Radio as part of the sensing instrument | Device-free occupancy, coarse material sensing, richer ranging observations | Can communication paths reveal a physical system's changing state before a conventional single sensor notices? |
| Cooperative spatial understanding | AoA assets, contextual ranging correction, changing peer-pair schedules | Can a fleet reason about its collective configuration and recognize unsafe or degrading interactions? |
| Monitoring without a battery-service burden | Commercial RF-powered tags, backscatter, harvested-energy adaptation | Can useful physical-system awareness persist across very large numbers of small assets? |
| Shared intelligence between subsystems | Lighting/HVAC occupancy exchange and logistics condition/location histories | Can a network identify and prevent waste caused by interactions between otherwise healthy components? |
| Trust in the physical observations | Secure access and power-wave identification | Can the system distinguish valid physical evidence from corrupted, substituted or misleading observations? |

These directions provide a wider search space than a generic wireless vibration detector. None establishes uniqueness, superiority over entrants or predictive-maintenance performance. In particular, occupancy sensing and material classification do not automatically transfer to crack, leak or fouling detection. User-led concept exploration remains open.

## Source register

All sources below are primary standards-body, operator/manufacturer, author manuscript or university-repository material. Accessed 2026-10-04. Vendor deployments and benefits remain attributed claims. Paper publication/upload dates are distinguished above.

| ID | Source |
| --- | --- |
| I1 | [Wiliot/Walmart collaboration announcement](https://www.wiliot.com/blog/wiliot-collaborates-with-walmart-to-transform-retail-supply-chain-with-ambient-iot-and-ai) |
| I2 | [Wiliot Gen3 IoT Pixel launch](https://www.wiliot.com/blog/wiliot-unveils-next-generation-iot-pixel) |
| I3 | [Royal Mail technology: Innovation in Action](https://careers.royalmailgroup.com/gb/en/technology) |
| I4 | [Wiliot's Royal Mail deployment account](https://www.wiliot.com/blog/royal-mail-and-wiliot-an-award-winning-deployment-proves-ambient-iot-has-arrived) |
| I5 | [Quuppa and InPlay positioning announcement](https://www.quuppa.com/blogs-articles/quuppa-and-inplay-enable-bluetooth-angle-of-arrival-positioning-with-the-in100-nanobeacon/) |
| I6 | [HVAC Integration NLC Profile](https://www.bluetooth.com/specifications/specs/hvac-integration-nlc-profile/) |
| I7 | [SIG: NLC and HVAC integration, 2026](https://www.bluetooth.com/blog/bluetooth-networked-lighting-control-and-hvac-integration-are-advancing-sustainability-in-commercial-spaces/) |
| I8 | [SIG: Bluetooth ESL architecture and PAwR](https://www.bluetooth.com/blog/bluetooth-esl-the-global-standard-for-the-electronic-shelf-label-market/) |
| I9 | [ST electronic shelf label developer kit](https://www.st.com/en/evaluation-tools/steval-esl1kcb.html) |
| I10 | [CCC: securing digital keys, 2026](https://carconnectivity.org/securing-digital-keys-when-your-phone-unlocks-the-car/) |
| I11 | [NXP Channel Sounding devices and spatial access](https://www.nxp.com/company/about-nxp/smarter-world-blog/BL-CONNECTIONS-BLUETOOTH-CHANNEL-SOUNDING) |
| I12 | [Fraport Auracast trial announcement](https://www.fraport.com/en/newsroom/press-releases/2026/q1/frankfurt-airport-becomes-the-first-airport-worldwide-to-use-aur.html) |
| I13 | [ABB Smart Sensor BLE data path](https://library.e.abb.com/public/c553755e8ed241ab8e703e066d85991c/BR268.pdf) |
| I14 | [ABB Smart Sensor customer cases](https://new.abb.com/motors-generators/service/advanced-services/smart-sensor/customer-cases) |
| I15 | [Dexcom G7 Bluetooth Direct to Watch FAQ](https://www.dexcom.com/en-us/m/faqs/g7/compatibility) |
| S1 | [SIG Core 6.2 release and Shorter Connection Intervals](https://www.bluetooth.com/de/blog/just-released-bluetooth-core-6-2/) |
| S2 | [Core 6.3 specification and version date](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core_v6.3/out/en/index-en.html) |
| S3 | [SIG Core 6.3 technical overview](https://www.bluetooth.com/bluetooth-core-6-3-technical-overview/) |
| S4 | [SIG HDT description and planned adoption](https://www.bluetooth.com/blog/bluetooth-le-will-soon-get-a-major-boost-in-speed-introducing-bluetooth-high-data-throughput-hdt/) |
| S5 | [SIG technology roadmap talk, 2026-06-04](https://www.bluetooth.com/bluetooth-resources/bluetooth-roadmap-building-whats-next/) |
| A1 | [Original Sensors 2023 occupancy-sensing paper](https://mdpi-res.com/d_attachment/sensors/sensors-23-02581/article_deploy/sensors-23-02581.pdf); [later arXiv upload and journal metadata](https://arxiv.org/abs/2609.02915) |
| A2 | [Material/obstruction sensing manuscript](https://arxiv.org/html/2607.29110v1) |
| A3 | [Orientation correction manuscript](https://arxiv.org/html/2609.00650v1) |
| A4 | [Connectionless ranging manuscript, v3](https://arxiv.org/html/2605.17094v3) |
| A5 | [PassiveBLE manuscript](https://arxiv.org/html/2503.11490v1) |
| A6 | [Imperial repository: energy-adaptive wristband](https://spiral.imperial.ac.uk/entities/publication/c4b05beb-42aa-4824-bbd3-9ff3bb932fa8) |
| A7 | [Power-wave identification paper and conference metadata](https://arxiv.org/abs/2609.29213) |
| A8 | [Ingestible BLE manuscript, v2](https://arxiv.org/html/2601.19241v2) |
| A9 | [Crystal-free mote validation manuscript](https://arxiv.org/abs/2605.24277) |

## Review limits

Original sources were searched and opened, with full text inspected for key ranging, backscatter and material-sensing results. Publisher access failed for selected IEEE/MDPI landing pages; accessible author manuscripts, a publisher-hosted PDF and university metadata supported the retained claims. No hardware experiments, independent field audits or exhaustive patent/novelty search were performed. Manufacturer/operator announcements establish reported use, not independent replication. Update this note in place as evidence changes.
