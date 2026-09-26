# Project

Updated: 2026-09-26

## Purpose

Project name: **Sustain the System**.

## Primary goal

**Build systems that know when they need attention. Predict failure. Prevent waste. Extend the life of the systems our world depends on.**

Develop a predictive monitoring solution for the **Sustain the System competition**, using **Nordic Semiconductor's nRF54LM20 DK and Bluetooth technology**. Detect degradation or other issues early enough to enable proactive maintenance, prevent failure and unnecessary replacement, and help equipment, infrastructure, or natural resources last longer.

The user adopted the supplied competition overview as the primary project goal on 2026-09-24. See [the supplied competition overview](Sources/Competition%20Overview.md) for the preserved source, examples, prize descriptions, and resource links. That note preserves the original unverified material. Official categories, scoring, and key dates were subsequently checked on 2026-09-25 in [Prize categories and judging](Research/Prize%20Categories%20and%20Judging.md); selected technical claims and remaining uncertainties are covered in [Opportunity assessment](Research/Opportunity%20Assessment.md).

## Required solution outcomes

The solution should demonstrate how Bluetooth technology can:

- Collect and share meaningful data from the physical world.
- Monitor changes and detect critical conditions.
- Alert users and recommend next steps based on actionable insights.
- Improve efficiency, resilience, or resource utilization with practical sustainability outcomes.

The selected use case should connect early detection to a concrete maintenance or intervention action and a measurable benefit. Specific metrics and thresholds remain to be defined.

## Candidate scope and technical opportunities

- **Equipment:** bearing wear, pumps, HVAC, conveyor motors, generators, solar inverters, irrigation systems, or agricultural equipment.
- **Infrastructure:** community infrastructure, building energy systems, or greenhouses.
- **Natural resources:** irrigation, animal and wildlife health, or water resources.

These are examples from the overview, not selected deliverables. The target asset and intended users remain undecided.

The overview highlights on-device machine learning and edge computing as opportunities, including anomaly detection, predictive failure modeling, sensor fusion, and maintenance recommendations. A particular model, sensor set, architecture, or accelerator is not yet selected; edge AI is not stated as a mandatory requirement in the supplied overview.

## Confirmed requirements

- Keep canonical project documentation in this self-contained Obsidian vault, starting at [[Overview]]. Note navigation must stay within this vault.
- Follow **planning → vault → execution** for project tasks.
- Vault documentation itself is exempt from the preliminary planning and task-record requirement.

## Current state

The [MCU and board capability reference](MCU%20and%20Board%20Capabilities.md) records manufacturer-verified specifications for nRF54LM20A/B, related family members, and the two discussed development boards. It separates silicon, board and software capabilities; physical hardware and firmware remain unverified.

The primary goal and expected solution outcomes are documented from the user-supplied competition overview. The use case and implementation scope remain to be selected.

The [application landscape research](Research/Contest%20Landscape.md) and [single current inventory](Research/Application%20Inventory.md) cover 47 hardware applications, last checked 2026-09-26. They include inferred themes/category fit and evidence priorities for a candidate adaptive monitoring network. New equipment and network-oriented headings reinforce the need for measured differentiation; no use case or architecture has been finalized. Future refreshes update these notes in place, with brief change history rather than separate dated additions.

## Open questions

- Which asset or resource will be monitored, for which users, and for which degradation or failure mode?
- What sensors, data, and baseline will establish that early detection is useful?
- What intervention should an alert trigger, and how will its sustainability benefit be measured?
- What deliverables, budget, hardware availability, and constraints define the first milestone?
- Key deadlines, build/documentation requirements, and judging criteria are now summarized in [Prize categories and judging](Research/Prize%20Categories%20and%20Judging.md). Confirm entrant-specific eligibility and recheck official requirements before applying or submitting.

Capture answers here when provided; they do not block maintaining the vault.
