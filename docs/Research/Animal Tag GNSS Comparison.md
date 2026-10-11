# Animal tag GNSS comparison

Checked: 2026-10-04. Species, habitat, fix cadence, runtime and assembled mass remain unspecified. Recommendations are engineering judgments, not field validation or a purchase decision. See [animal-tag impact](Animal%20Tags%20Sensors%20and%20Impact.md).

## Recommendation and identity

Between the requested parts, LG77L is the stronger compact-tag candidate. For a new design, shortlist **MAX-M10S for a prototype**, **MIA-M10Q for a very small custom PCB**, and **LC76G(PA/PB) for low-power Quectel alternatives**. Whole-tag energy per successful fix and antenna arrangement ultimately determine suitability.

These are GNSS modules, not bare chipsets. Mikroe explicitly identifies LG77LICMD and links the Quectel LG77L family datasheet. Its Galileo description points to C, but the reviewed official documents do not decode the full ordering suffix: C mapping is provisional. L89HA is L89(HA), associated with L89 R2.0. Verify exact ordering code/firmware before PCB design. [Mikroe](https://www.mikroe.com/gnss-13-click), [HA/HB differences](https://quectel.com/content/uploads/2024/02/Quectel_L89HAL89HB_Difference_Introduction_V1.0.pdf).

## Requested modules

| Property | LG77L family / LG77LICMD candidate | L89HA / L89 R2.0 |
| --- | --- | --- |
| Size | 7 × 7 × 2 mm | 26.4 × 18.4 × 6.8 mm |
| Mass | Approx. 0.2 g, antenna excluded | Approx. 8.2 g, integrated antennas included |
| Signals | Single-band GPS L1, GLONASS L1 or BeiDou B1I; Galileo E1 on C | Default GPS L1, Galileo E1, QZSS L1, NavIC L5 |
| Antenna | Separate GNSS antenna needed | Integrated patch/chip antennas; external option |
| Typical tracking | C: 23 mA at 3.3 V = 75.9 mW; A: 24 mA = 79.2 mW | 32 mA at 3.3 V = 105.6 mW |
| Backup | Family headline 6 µA | 51 µA |
| Unaided cold start | C: <25 s under specified conditions | 35 s under specified conditions |
| Power/logic | Main 2.8–4.3 V; separate I/O 1.8 or 2.8 V | Main 3.1–4.3 V; typical I/O 3.0 V |
| Fit | Compact custom receiver; RF integration required | Antenna-equipped demonstration; substantial wearable mass/height |

Sources: [LG77L specification V1.3](https://www.quectel.com/content/uploads/sites/3/2022/06/Quectel_LG77L_GNSS_Specification_V1.3.pdf), [L89 R2.0 specification V1.2](https://www.quectel.com/content/uploads/2024/03/Quectel_L89_R2.0_GNSS_Specification_V1.2-1.pdf). Constellations/test setups differ, so these are not equal-antenna field results; LG77L auxiliary supply currents also need checking.

**L89HA's dual-band label does not mean GPS L1+L5:** its listed L5 signal is NavIC, while GPS is L1. NavIC may matter for an India-focused application, but it does not establish better canopy performance. LG77L hardware documentation makes I2C firmware-dependent (versions ending SC); UART is the initial interface candidate. GLP sacrifices high-accuracy positioning and can return to continuous mode in challenging conditions. [LG77L hardware design V1.2](https://quectel.com/content/uploads/2024/02/Quectel_LG77L_Series_Hardware_Design_V1.2.pdf).

## Alternatives

| Candidate | Published size / power evidence | Conditional fit |
| --- | --- | --- |
| **MAX-M10S** | Approx. 10.1 × 9.7 × 2.5 mm, 0.5 g; calculated default tracking 35.4 mW at 3 V or 27.36 mW at 1.8 V | Prototype candidate: module, integrated LNA/filter, external antenna, UART/I2C |
| **MIA-M10Q** | 4.5 × 4.5 mm footprint, approx. 0.07 g; calculated default tracking 38.7 mW at 3 V or 31.14 mW at 1.8 V | Compact custom PCB: SiP with TCXO/LNA/filter, dense assembly, external antenna; smaller does not mean lower power than MAX |
| **LC76G(PA)** | 10.1 × 9.7 × 2.4 mm, approx. 0.5 g; 32.01 mW continuous tracking at 3.3 V in V1.3 | Low-power Quectel candidate in a 3.3 V domain |
| **LC76G(PB)** | Same size; 28.08 mW continuous tracking at 1.8 V in V1.3 | Candidate where 1.8 V supply/I/O fits; regulation/level conversion may add overhead |

u-blox calculations add VCC and V_IO currents in default GPS+Galileo+BeiDou B1I, 1 Hz, specified signal level/internal LNA setting. Headline 25 mW is not guaranteed for every configuration. Both M10 sheets list 28 µA hardware-backup current under their specified conditions. Sources: [MAX-M10S R08](https://content.u-blox.com/sites/default/files/MAX-M10S_DataSheet_UBX-20035208.pdf), [MIA-M10Q R08](https://content.u-blox.com/sites/default/files/documents/MIA-M10Q_DataSheet_UBX-22015849.pdf), [MIA product page](https://www.u-blox.com/en/product/mia-m10-series).

LC76G V1.3 lists PA/PB adaptive low-power Mode 1 as 12.54/9.72 mW and Mode 2 as 24.75/20.34 mW. These are not unrestricted continuous tracking. **AB is about 108.57 mW tracking**, so exact suffix matters. Earlier V1.2/brochure values differ; confirm ordered revision, firmware and mode rather than mixing figures. [V1.3 hardware design](https://www.quectel.com/content/uploads/2023/05/Quectel_LC76G_Series_Hardware_Design_V1.3.pdf), [earlier V1.2](https://quectel.com/content/uploads/2024/02/Quectel_LC76G_Series_Hardware_Design_V1.2.pdf).

**Sony CXD5610GF** is an actual IC: 3.66 × 3.15 mm XFBGA, published 9 mW simultaneous L1/L5 reception, 6 mW L1, embedded nonvolatile memory and low-voltage supplies. Attractive future custom design, but IC power excludes a finished tag. Procurement, full design documentation, SDK, support circuitry and assembly remain unverified; not the default POC choice. [Sony brief](https://www.sony-semicon.com/files/62/pdf/p-23_CXD5610_1015.pdf).

**Semtech LR1110** combines GNSS scanning, LoRa and Wi-Fi scanning; a cloud solver computes position. It is not a standalone NMEA receiver replacement. Potential for sparse wildlife reports depends on transport, assistance and backend energy/cost. Current Semtech guidance names partner solvers including AWS/Traxmate; do not assume an unchanged legacy service. [LR1110](https://www.semtech.com/products/wireless-rf/lora-edge/lr1110), [current cloud guidance](https://www.semtech.com/cloud-geolocation).

## Animal-tag implications and limits

### Sleep, bursts and cycling

**Exact MAX-M10S-00B follow-up, checked 2026-10-04:** supported. Datasheet R08 identifies ordering code MAX-M10S-00B and type MAX-M10S-00B-01 with ROM SPG 5.10, and lists on/off and cyclic-tracking power-save modes. Hardware backup removes VCC/V_IO while retaining V_BCKP; published current is 28 µA at 3.3 V. Fully removing backup too loses retained clock/orbit/configuration data. In hardware backup, PIOs must not be driven. At 1.8 V I/O with backup supplied, turn V_IO off 100 ms before VCC, or request software standby first; 3 V supplies may switch off together. Startup supply must accommodate up to 100 mA inrush. [Official datasheet](https://content.u-blox.com/sites/default/files/MAX-M10S_DataSheet_UBX-20035208.pdf).

The integration manual distinguishes short-interval cyclic tracking (0.5–10 s) from on/off operation for longer periods, including minutes/hours/days. Retaining backup enables faster restart when stored time/orbit data remain useful; hot start is not guaranteed. A proposed sparse animal-tag design would use timed or motion-triggered host control with retained backup. Verify actual type/firmware if a seller only supplies the ordering code; breakout regulators, LEDs and antenna circuits may dominate idle current. [Official integration manual](https://content.u-blox.com/sites/default/files/MAX-M10S_IntegrationManual_UBX-20053088.pdf). No circuit or firmware implementation selected.

Follow-up checked 2026-10-04: yes, continuous tracking is not required. LG77L supports periodic standby/backup cycling and GLP; its backup domain retains RTC/RAM, while satellite searching stops in sleep. Keeping backup power is different from removing every supply. Retained data may support faster restart, but long gaps, stale orbit data and weak signals prevent guaranteeing a short burst. [LG77L operating modes](https://quectel.com/content/uploads/2024/02/Quectel_LG77L_Series_Hardware_Design_V1.2.pdf).

L89HA supports backup/wake operation with published 51 µA backup current, so a host-controlled burst schedule is possible; HA/HB entry/exit sequences differ. Cycling does not remove its physical size/mass. [HA/HB backup modes](https://quectel.com/content/uploads/2024/02/Quectel_L89HAL89HB_Difference_Introduction_V1.0.pdf). MAX-M10S also has cyclic tracking and hardware-backup operation, with 28 µA specified backup current at 3.3 V. [MAX-M10S power modes](https://content.u-blox.com/sites/default/files/MAX-M10S_DataSheet_UBX-20035208.pdf).

Proposed animal-tag policy, not a selected requirement: maintain low-power motion monitoring, request occasional location while resting, increase cadence on movement or an anomaly, acquire a quality-checked position, store it and return GNSS to backup. Use a maximum acquisition duration and record unsuccessful attempts rather than searching indefinitely. Full power removal may save idle energy but lose retained state; the winner depends on gap length and acquisition energy. No live GNSS location is available during sleep; motion sensing cannot replace an absolute location fix.

Illustration: a 32 mW receiver active for 30 s every 10 min contributes 1.6 mW average before backup/other loads, twenty times below continuous operation. At 120 s per attempt, that contribution is 6.4 mW. These are assumptions, not runtime predictions. Sparse-fix selection must compare total energy per accepted fix, not rank solely by continuous current or backup current. No schedule or hardware validated.

Design inference: antenna orientation, body proximity, enclosure and foliage can outweigh footprint differences. Compare complete assemblies: LG77L/M10/LC76G exclude the GNSS antenna, L89 includes antennas. A smaller receiver does not establish a safe attachment or automatically permit a smaller battery. See [animal-impact guidance](Animal%20Tags%20Sensors%20and%20Impact.md).

Compare energy per accepted fix: acquisition + tracking/quality checks + sleep + MCU/storage/radio/regulator/antenna energy. Illustration only: 32 mW for 30 s every 5 min averages 3.2 mW before other loads; 120 s averages 12.8 mW. Neither is a measured schedule. Stale orbit data, failed fixes and hot/cold/assisted starts prevent deriving battery life from tracking current alone.

GNSS gives outdoor absolute coordinates, not an upload connection or health diagnosis. UART/logic/power control and firmware integration with the Nordic host remain work to validate. Bluetooth local ranging is separate; moving-DK/three-base POC direction is unchanged. No purchase, integration, hardware/animal tests or species selection.

Open discriminators: species/attachment, canopy, fix interval, offline operation, runtime and total mass. None of the reviewed sheets establishes a canopy winner. Remaining selection evidence is successful-fix rate, error distribution and energy with the actual antenna/mounting conditions.
