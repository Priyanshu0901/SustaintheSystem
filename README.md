# Sustain the System

### Every machine has a sound. A change is a reason to listen.

**Acoustic fault detection and localization, built around local intelligence.**

A fan starts rattling. A motor develops an unfamiliar hum. In a room full of running equipment, noticing the change is only half the problem—finding where it came from is the other half.

Sustain the System is an in-development, non-contact monitoring system that aims to detect changes in machinery sounds and estimate the direction of the source. The goal is to give a technician a useful starting point for inspection.

[Explore the project on Hackster](https://www.hackster.io/priyanshu-roy/acoustic-fault-detection-and-localization-0ce81d) · [Open the documentation](docs/Overview.md)

## From sound to an inspection

The planned monitoring node combines a **Nordic nRF54LM20 DK** with a **Sipeed R6+1 microphone array**. Local audio processing and Edge AI look for departures from normal operation; spatial analysis estimates which direction deserves attention.

```text
Machinery sound → Microphone array → Local anomaly + direction analysis
                                              │
                                      Compact Zigbee reports
                                              ↓
                                    Coordinator → Local computer
```

For diagnostics, an Android companion app will use an **ACS ACR1552U USB NFC reader** to read BLE commissioning details from the node's tag, then retrieve event history over an authenticated Bluetooth connection.

The design brings together three useful ideas:

- **Listen without contact.** Observe machinery acoustically, without attaching a sensor to each machine.
- **Process at the node.** Send compact observations over Zigbee while keeping audio analysis local.
- **Bring history to the technician.** Use nearby Bluetooth access to investigate recorded events.

An acoustic anomaly is a prompt to inspect equipment; it does not establish a specific fault or predict remaining life.

## On the bench today

**The radio foundation is working. Acoustic performance remains to be demonstrated.**

The XIAO ESP32-C6 USB coordinator and one XIAO nRF54LM20A Zigbee sleepy end device have passed build, programming and live simulated-report checks. The single-node test also verified retained pairing across resets and resumed reporting after a host restart.

Microphone integration, acoustic inference, routed mesh operation and the Android diagnostic workflow remain unverified. The planned acoustic demonstration will compare one microphone with the array using two simultaneous fans or motors, measuring detection, false alarms, direction estimates and power.

## Explore the build

| Start here | What you'll find |
| --- | --- |
| [Project documentation](docs/Overview.md) | The canonical documentation entry, firmware guides and searchable topic references |
| [Firmware architecture](docs/Firmware/Nordic%20Zigbee/Architecture.md) | Written node components, report handoff and ZBOSS callbacks; links to coordinator architecture |
| [Hackster project](https://www.hackster.io/priyanshu-roy/acoustic-fault-detection-and-localization-0ce81d) | The public build story and bill of materials |
| [Nordic node firmware](firmware/nordic_zigbee/README.md) | Board and role profiles, pinned build inputs, programming and validation |
| [ESP32-C6 coordinator](firmware/esp32c6_coordinator/README.md) | Upstream provenance, pinned builds and local host setup |

For a hands-on start, follow the relevant firmware guide. Each integration records its own prerequisites, commands and tested hardware scope.
