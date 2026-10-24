# Acoustic Fault Detection and Localization

A non-contact monitor that detects changes in machinery sounds and estimates which nearby machine needs inspection.

The monitoring node uses the Nordic nRF54LM20 DK and a Sipeed R6+1 microphone array for local audio processing and Edge AI anomaly detection. Zigbee carries compact observations to a local coordinator; NFC-assisted Bluetooth access provides diagnostic history during maintenance. A later field version will use LoRa reporting.

The first demonstration will compare one microphone with the array using two simultaneous fans or motors, measuring detection, false alarms, direction estimates and power. An acoustic anomaly indicates a reason to inspect equipment; it does not establish a specific fault or remaining life.

Status: design and documentation in progress. Hardware integration and measured performance remain unverified.

## Project documentation

- [[Hackster/Story|Project story]] — design, operation and validation approach.
- [[Hackster/Basics|Title and pitch]].
- [[Hackster/Things|Bill of materials]] — selections pending.
- [[Hackster/Hardware Application Questions|Hardware application]] — draft answers.

## Technical references

- [[Hardware/MCU and Board Capabilities|MCU and board capabilities]].
- [[Hardware/Bluetooth and 2.4 GHz Radio|Bluetooth and radio reference]].
- [[Sources/References|Sources]] — manufacturer documents, datasets and contest rules.
