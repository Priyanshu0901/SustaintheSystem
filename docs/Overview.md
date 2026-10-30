# Acoustic Fault Detection and Localization

A non-contact monitor that detects changes in machinery sounds and estimates which nearby machine needs inspection.

The implemented transport stage is documented in the [Nordic firmware architecture](Firmware/Nordic%20Zigbee/Architecture.md) and [coordinator architecture](Firmware/ESP32-C6%20Coordinator/Architecture.md), each with an editable draw.io diagram.

Status: the USB coordinator and one XIAO Zigbee sleepy node passed build, programming and live simulated-report checks. Sensing integration, routed mesh and acoustic performance remain unverified.

## Project documentation

- [Hackster project](https://www.hackster.io/priyanshu-roy/acoustic-fault-detection-and-localization-0ce81d) — project article and bill of materials.

## Firmware guides

- [Nordic Zigbee node](Firmware/Nordic%20Zigbee/Overview.md) — runtime architecture, report protocol, implemented profiles and code reference.
- [ESP32-C6 coordinator](Firmware/ESP32-C6%20Coordinator/Overview.md) — existing NCP integration, build design, board profiles and host wrappers.

## Reference library

| Boards and processing | Radio and protocols |
| --- | --- |
| [nRF54LM20A/B SoCs](Reference/Boards/Nordic%20SoCs.md) | [Modes and BLE roles](Reference/Radio/Modes%20and%20Roles.md) |
| [Audio, USB and peripherals](Reference/Boards/Audio%20and%20USB.md) | [GATT, timing and security](Reference/Radio/Connections%20and%20Security.md) |
| [Axon NPU and DSP](Reference/Boards/Axon%20NPU.md) | [Periodic advertising and PAwR](Reference/Radio/Periodic%20Advertising.md) |
| [Nordic DK](Reference/Boards/Nordic%20DK.md) | [Channel Sounding](Reference/Radio/Channel%20Sounding.md) |
| [XIAO Sense sensors and board](Reference/Boards/XIAO%20Sense.md) | [Networks and coexistence](Reference/Radio/Networks%20and%20Coexistence.md) |
| [Related nRF54 parts](Reference/Boards/Nordic%20Family.md) | [RF, power and range](Reference/Radio/RF%20and%20Power.md) |
| [Board SDK and pin questions](Reference/Boards/SDK%20Compatibility.md) | [Radio SDK support](Reference/Radio/SDK%20Support.md) |

[Sources](Sources/References.md) collects manufacturer documents, datasets and contest rules. Topic notes retain citations beside their claims.
