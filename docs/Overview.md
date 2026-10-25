# Acoustic Fault Detection and Localization

A non-contact monitor that detects changes in machinery sounds and estimates which nearby machine needs inspection.

The monitoring node uses the Nordic nRF54LM20 DK and a Sipeed R6+1 microphone array for local audio processing and Edge AI anomaly detection. Zigbee carries compact observations to a local coordinator. An Android device with a USB NFC reader reads BLE commissioning details from the node tag, then retrieves diagnostic history over Bluetooth. A later field version will use LoRa reporting.

The first demonstration will compare one microphone with the array using two simultaneous fans or motors, measuring detection, false alarms, direction estimates and power. An acoustic anomaly indicates a reason to inspect equipment; it does not establish a specific fault or remaining life.

Status: design and documentation in progress. Hardware integration and measured performance remain unverified.

## Project documentation

- [Hackster project](https://www.hackster.io/priyanshu-roy/acoustic-fault-detection-and-localization-0ce81d) — project article and bill of materials.

## Technical references

- [[Hardware/MCU and Board Capabilities|MCU and board capabilities]].
- [[Hardware/Bluetooth and 2.4 GHz Radio|Bluetooth and radio reference]].
- [[Sources/References|Sources]] — manufacturer documents, datasets and contest rules.
