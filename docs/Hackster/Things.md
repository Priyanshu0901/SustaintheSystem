---
status: draft
updated: 2026-10-25
---

# Things

[Hackster Things editor](https://www.hackster.io/projects/0ce81d/edit#things) · [[Hackster/Basics|Basics]] · [[Hackster/Story|Story]]

Bill of materials for the acoustic monitoring node, microphone-based POC nodes and Android maintenance interface. Quantities for the boards, array, reader and fans follow the current Hackster list.

## Hardware components

| Component | Quantity | Role in the project |
| --- | ---: | --- |
| Nordic nRF54LM20 DK | 1 | Main monitoring node, targeting the nRF54LM20B for Axon NPU inference and multichannel acoustic processing |
| Sipeed MAIX R6+1 microphone array | 1 | Seven-channel acoustic capture for beamforming and source-direction estimation |
| Seeed Studio XIAO nRF54LM20A Sense | 4 | Microphone/CPU POC and Zigbee firmware testing while the DKs arrive; two temporarily serve as powered routers during mesh tests |
| ACS ACR1552U USB NFC reader | 1 | USB-connected reader for the Android maintenance interface; reads the nRF tag’s identity and BLE commissioning details |
| Seeed Studio XIAO ESP32C6 | 1 | Zigbee coordinator; connects the monitoring network to the local computer over USB |
| Low-voltage DC fans or motors | 2 | Simultaneous-source test setup for normal and abnormal operating conditions |
| Android device | 1 | Hosts the companion application, reads tag data through the ACS reader and connects to the monitoring node over BLE |
| Monitoring computer | 1 | Hosts the coordinator connection, report storage and local monitoring application |
| USB host / OTG adapter for Android | 1 set | Connects the ACS reader to the Android device |
| USB cables | As required | Programming, power and the coordinator-to-computer connection |

The XIAO Sense boards are already available. The nRF54LM20 DK is awaiting arrival. The ACS reader remains part of the final Android maintenance interface.

Additional ordered hardware: [Waveshare ESP32-C6-LCD-1.9 touch development kit](../Firmware/ESP32-C6%20Coordinator/Board%20Profiles.md#hardware-status). Its status is kept with the coordinator board documentation.

## Software OSs, Apps and online services

| Item | Purpose |
| --- | --- |
| nRF Connect SDK | Nordic development framework for firmware, peripheral drivers and wireless stacks |
| Nordic Zigbee R23 Add-on | Existing Zigbee test firmware and router/end-device configurations; [Nordic firmware guide](../Firmware/Nordic%20Zigbee/Overview.md) links pinned inputs and measured validation |
| Zephyr RTOS | Embedded operating system and task scheduling |
| Nordic Edge AI Add-on | Axon model compilation, inference driver and integration tools |
| Android | Operating system for the maintenance device |
| draw.io / diagrams.net | Editable system diagrams, processing flows and interface mockups |

## Hand tools and fabrication machines

| Item | Quantity | Purpose |
| --- | ---: | --- |
| 3D printer | 1 | Enclosure and test-mount fabrication |
| Soldering iron | 1 | Wiring and assembly |

LoRa hardware belongs to the later field version and is separate from this starting BOM. Manufacturer documentation is collected in [[Sources/References|Sources]].
