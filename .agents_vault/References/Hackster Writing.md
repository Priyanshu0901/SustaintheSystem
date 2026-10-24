# Hackster writing guidance

Updated: 2026-10-25. Earlier project comparisons reviewed 2026-10-24; Alex Wulff tutorial guide reviewed 2026-10-25. Applies to [Story](../../docs/Hackster/Story.md); [Overview](../../docs/Overview.md) owns scope and evidence limits.

## Current writing instructions

Write as the engineer building the product. Use direct first-person narration: what I am using, how it works, why I chose it, and what I will measure. Use design-present for intended operation and future tense for work/tests; never invent completed hardware, training, integration or results. Keep actual uncertainty in the implementation notes and mention a technical limit in the article only where it explains a decision or test.

The article is an ongoing build read end-to-end. Do not open with an abstract or a summary of paragraphs that follow, announce the document's purpose, label the article The Concept, or close with From Concept to POC / Where This Could Lead recaps. Explain the product, operating flow, engineering choices and validation naturally through connected paragraphs. Remove repetition by giving each explanation one home; do not remove useful detail to meet a word-count target.

Use simple, product-focused headings such as Microphone Array and Audio Capture, Acoustic Anomaly Detection, Wireless Monitoring and Diagnostics, Power Management, and Testing and Validation. Avoid creator-script headings such as What I Plan to Explore, clever Seven Microphones / Two Fans titles, grant/professor-approval language, and repeated proposed / can be / to be selected wording. Say what this build will do rather than list every possible alternative. Light humour may arise from engineering constraints; do not force jokes or fabricate personal experiences.

Explain meaningful technical steps directly. For tests, say what the author will change, record and compare; do not imply that a documented abnormal condition already exists. Explain inspection/repair value without invented life-extension, power or accuracy results. Product figures show the architecture and workflow; technical evidence remains distinguishable from illustration without filling the article with disclaimers.

## File ownership and Things categories

- [Basics](../../docs/Hackster/Basics.md): project name and elevator pitch.
- [Things](../../docs/Hackster/Things.md): BOM and third-party dependencies, using the three category names below.
- [Story](../../docs/Hackster/Story.md): product narrative, mechanisms, reasoning, validation, future field application and bottom references.
- [Hardware application](../../docs/Hackster/Hardware%20Application%20Questions.md): separate form answers in text code blocks. Respect requested question ranges; Q4 is left unchanged until instructed. Do not edit biography or select declarations without authorization.

Things category rules:

| Category | Include | Keep elsewhere |
| --- | --- | --- |
| Hardware components | Boards, sensors, readers, host devices, adapters, cables and other physical parts used in the system | Fabrication tools |
| Software OSs, Apps and online services | Third-party operating systems, SDKs, applications, libraries and online services used for development or operation | Our own firmware, companion application and monitoring application: explain these in Story and deliver them as source code |
| Hand tools and fabrication machines | Soldering tools, 3D printer and equipment used to make or assemble the product | Components and cables that remain in the system |

Do not add products, software stacks, versions or purchases merely to fill categories. Record quantities and availability from user confirmation or the stated BOM source. LoRa belongs to the user-requested later field version, separate from the initial Zigbee build. Sources owns external technical references; implementation notes own unresolved integration details.

## Images and editable diagrams

Use images to carry spatial relationships, architecture and interaction sequences rather than turn every explanation into a heading or table. Place images beside the corresponding prose. Use concise Markdown captions, for example `![Signal and access paths](assets/system-flow.png)`.

Create native editable draw.io sources and matching PNG exports in docs/Hackster/assets/. Update both when a diagram changes. Keep titles and explanatory context in the surrounding caption/prose, not title banners inside the picture. Do not add illustration-only disclaimers, cautionary footers, apology text or repeated status qualifications inside figures. Functional component, state, connection and swimlane labels are part of the diagram and may remain.

The NFC/BLE maintenance sequence is one composite image containing three panels, uploaded as one image. UI data remains illustrative rather than a claimed measurement; record that status in technical context without warning banners. Preserve the user's design edits when updating native sources. Replace image briefs with actual assets as they become available; do not publish drafting instructions. Use real data for measured plots. Put manufacturer, dataset and other technical references at the bottom of Story, with image-source credits where appropriate.
## Evidence and reproduction

For results record setup, procedure, outcome and limits. Keep recording sessions and threshold selection separate from evaluation. Distinguish public-data detection, physical source association, actual NPU deployment and whole-node energy. Label simulated network data. A final tutorial needs tested versions, wiring, commands, expected output and recovery steps; add these when they exist.

## Completed-project tutorial reference

User-selected priority reference for the final write-up: [Alex Wulff — How to Create a High-Quality Project Tutorial](https://www.hackster.io/AlexWulff/how-to-create-a-high-quality-project-tutorial-e25feb), published 2017-08-09. Full article read in the browser on 2026-10-25 after the web reader failed. Revisit when the build is complete and before preparing the final Hackster tutorial.

Useful guidance, paraphrased:

- Share legible schematics and editable design files, plus a complete, accurately linked Things list.
- Photograph construction steps with good lighting and consistent viewpoints; capture enough detail to explain assembly. Choose a clear, representative cover.
- Publish readable code with explanatory comments, named configuration values and consistent formatting; provide complete files rather than isolated snippets.
- Organize instructions with useful headings, short paragraphs and photos beside the relevant steps. Proofread, explain terminology and expand acronyms.
- Make the title and pitch explain the project, then check publication details and meaningful topic tags.

Application to this project: collect wiring, array placement, assembly photos, software versions and experiment records during development. After completion, replace image briefs with real assets, finish Things, document acquisition/model deployment and commissioning, show measured results and BLE retrieval, and check that another person can reproduce the tested system.

This supplements the user's narrative preferences; it does not impose numbered headings or a word limit. Recheck current publishing controls and image requirements rather than treating 2017 interface advice as current. The author's optional extra-parts/photography purchases are not project requirements. Recording this reference does not authorize publication or imply a completed build.

## Research provenance

Five award-bearing examples were checked against the [Smarter Sustainable World](https://www.hackster.io/contests/smarter-sustainable-world) and [Make it Matter](https://www.hackster.io/contests/makeitmatter) winner listings. Their useful pattern: concrete problem → user interaction → architecture choices → reproducible evidence and limitations. This is editorial interpretation, not proof that prose caused an award. No hardware, repository or embedded-media testing was performed.

- [Water tap controller](https://www.hackster.io/ElijahMaluleke/smart-water-tap-leakage-controller-iot-project-c41a30): use case and construction evidence.
- [Soilless farm](https://www.hackster.io/Tai-Min/vertical-self-regulating-soilless-farm-a86d25): subsystem roles and limitations.
- [Power monitoring/fault detector](https://www.hackster.io/487038/power-monitoring-and-fault-detector-device-349054): acquisition-to-indication path and interface obstacle.
- [Matter home](https://www.hackster.io/jens-ajisai/gently-communicating-home-using-matter-ca97fb): interaction and software constraints.
- [Smart Power Monitor](https://www.hackster.io/matvs/smart-power-monitor-6f2f84): quick start with expected behaviour.

The user-selected [gym-class Story](https://www.hackster.io/kevin-loeffler/the-30-gym-class-live-heart-rate-zones-for-every-student-6a97db#story) supplies the problem-to-workflow progression. [Tree acoustics logs](https://hackaday.io/project/193030/logs?sort=oldest) connect setup pictures with constraints; [FATCAT sound-engine log](https://hackaday.io/project/161396-fatcat-altoids-tin-mod-tracker/log/154421-the-sound-engine) was available only as indexed text. Neither supplies measurements for our project.

Contest facts stay in [[References/Prize Categories and Judging]]. Earlier outlines, shortening targets, tentative openings, draft counts and conflicting revision advice are retired in [[References/History|Git history]].

## Current live-page review — 2026-10-25

Reviewed [the public project](https://www.hackster.io/priyanshu-roy/acoustic-fault-detection-and-localization-0ce81d) in the browser after the web reader failed. Expanded Story and inspected the cover, system diagram and maintenance mockup. Title/pitch match Basics; page is Work in progress. The live starter BOM includes one nRF54LM20 DK, one R6+1 array, four XIAO nRF54LM20A Sense boards, an ACS ACR1552U, one XIAO ESP32C6 and two low-voltage fans/motors, plus software/tools. This is page content, not confirmation of hardware receipt or compatibility. Local Things has since been reconciled with the starting BOM and corrected category ownership.

Actionable findings:

- At the time of review, three internal image briefs were publicly visible: array/timing illustration, future spectrograms and test layout. Replace or omit these from published Story; keep drafting instructions locally.
- NFC role clarified by the user: ACR1552U connects to Android in the final product and reads the nRF tag for BLE commissioning information. Android then connects over BLE. Keep Story and figures consistent; no bench-only restriction or blanket claim about Indian phones. Manufacturer Android support is a platform reference, not proof of this app/device integration.
- XIAO role clarified by the user: available nRF54LM20A Sense boards supply microphone-based POC nodes and a cheaper single-microphone baseline while the DKs arrive. Do not assume these are repeaters. A-variant CPU performance and B-variant NPU deployment are separate measurements.
- Fix component-note spelling: coordinator and commissioning. Add direct dataset references and source/photo attribution where appropriate.
- Diagram and maintenance mockup render; the maintenance image is a UI design, with status retained in technical context rather than figure disclaimers. Paragraph breaks display correctly once Story is expanded. No need to require finished-build results for this work-in-progress page review.

No external edits, forms, agreements or uploads performed. Public URL is now known; Q4 remains unchanged because the user previously asked to leave it.
