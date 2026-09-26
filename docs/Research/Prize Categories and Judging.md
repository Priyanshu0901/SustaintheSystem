# Prize categories and judging

Checked: 2026-09-25. [Research overview](Contest%20Landscape.md) · [Application inventory](Application%20Inventory.md).

## Official award structure

The [contest overview](https://www.hackster.io/contests/sustain-the-system) lists six awards: one overall prize of $3,000 and five specialist prizes of $1,000 each, totaling $8,000. The following descriptions are concise paraphrases of that page.

| Category | Official emphasis |
| --- | --- |
| Overall: Sustain the System | Best overall solution to the contest challenge |
| Edge AI | Innovative on-device intelligence, including anomaly detection, prediction, fusion, and recommendations |
| Equipment Monitoring | Machine and industrial equipment condition, including motion, temperature, sound, and operating behavior |
| Infrastructure | Predictive intelligence added to an existing asset; examples span buildings, water, waste, and renewable energy |
| Natural Resource | Early detection protecting resources, agriculture, habitats, or animals |
| Bluetooth | Valuable use of Bluetooth, including notifications, Channel Sounding, Mesh, or demonstrated low-energy operation |

The overview inconsistently labels the equipment prize card “Best Industrial Solution” and the infrastructure card “Best Infrastructure Project.” These appear under the corresponding category headings; they are not extra awards. Source: [official prize section](https://www.hackster.io/contests/sustain-the-system#prizes).

## Official scoring and dates

The [rules](https://www.hackster.io/contests/sustain-the-system/rules) assign 100 points:

| Scoring area, paraphrased | Points |
| --- | ---: |
| Reproducible project documentation | 20 |
| Demonstrated use of the required DK | 10 |
| End-to-end operation and stability | 10 |
| Real-world impact and theme relevance | 20 |
| Technical depth | 10 |
| User experience and design | 10 |
| Creativity | 20 |

The build must use the nRF54LM20 DK and address sustainability, predictive maintenance, and Bluetooth. English project documentation must include a name, description, cover, BOM, instructions, images, and relevant design/code files. Hardware applications close **2026-10-25, 11:59 PM Pacific**; completed projects close **2027-04-23, 5:00 PM Pacific**. The rules limit awards to one per person/team. Recheck the official page before applying or submitting; this note is not a complete eligibility checklist. [Official rules](https://www.hackster.io/contests/sustain-the-system/rules).

The FAQ distinguishes hardware recipients, due **2026-11-20**, from contest winners, due **2027-05-21**. No winners are established by the application headings in this September snapshot. [Official FAQ](https://www.hackster.io/contests/sustain-the-system/faq).

## Research interpretation: what would support a strong entry

The following is our analysis, not an additional official rubric or forecast.

| Target | Evidence to build | Weak claim to avoid |
| --- | --- | --- |
| Overall | Real asset, credible degradation progression, useful intervention, measured benefit, reproducible working demonstration | An impressive network without a clear maintenance problem |
| Edge AI | Held-out evaluation across operating conditions; comparison with simple DSP/threshold baselines; measured inference cost and value | Merely running a neural network or showing an uncalibrated health percentage |
| Equipment | A specific failure mode, repeatable sensor mounting, and a maintenance decision validated on the asset | Calling every vibration change a fault |
| Infrastructure | A practical retrofit and evidence that an operator can act on the output | Assuming any network automatically qualifies as infrastructure |
| Natural Resource | A measurable resource outcome and a credible intervention within the deployment context | Equating any environmental measurement with resource protection |
| Bluetooth | Measured energy, delivery reliability, command latency, and diagnostic behavior under the same health task | Treating BLE connectivity or a protocol name as sufficient novelty |

Documentation, impact, and creativity together account for 60 of the 100 published points. That argues for a complete, understandable maintenance story alongside technical performance. The published rubric does not establish separate category-specific point weights or provide winning probabilities.

Our current adaptive-network direction has a plausible **Bluetooth-first** story and a conditional **Edge AI** story. This is a recommendation for evidence planning, not a category commitment. The physical asset will determine whether equipment, infrastructure, or natural-resource positioning is convincing. Broad eligibility across categories should not be described as an expectation of multiple prizes.

