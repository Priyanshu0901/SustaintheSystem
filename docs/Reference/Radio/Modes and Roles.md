# LM20 radio modes and Bluetooth LE roles

Reference check: **2026-09-26**. For evidence levels, see [SDK support](SDK%20Support.md).

## Radio modes in the LM20A/B

| Radio mode | Nominal rate | Meaning |
| --- | --- | --- |
| Bluetooth LE 1M | 1 Mbps | Uncoded Bluetooth LE PHY |
| Bluetooth LE 2M | 2 Mbps | Faster uncoded PHY |
| Bluetooth LE Coded, S=2 | 500 kbps information rate | Forward error correction; more airtime than uncoded operation |
| Bluetooth LE Coded, S=8 | 125 kbps information rate | Stronger coding; still more airtime |
| IEEE 802.15.4 | 250 kbps | Radio foundation for Thread/Zigbee |
| Nordic proprietary | 1, 2 or 4 Mbps | Non-Bluetooth packet modes |

These are radio rates, not delivered application throughput. **The proprietary 4 Mbps mode is not a 4 Mbps BLE PHY.** Source: [LM20A/B RADIO chapter](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/radio.html?contentId=~f5ojBLyjysoQg63BKuIiA). Coding-rate interpretation: [Bluetooth architecture specification](https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Core-54/out/en/architecture%2C-mixing%2C-and-conventions/architecture.html).

RADIO includes EasyDMA packet transfer, address matching, packet assembly/disassembly, CRC generation/checking, RSSI measurement and interframe timing facilities. Those hardware blocks do not by themselves implement a complete network stack. [Nordic RADIO peripheral](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/radio.html?contentId=~f5ojBLyjysoQg63BKuIiA).

### Terms that are easy to confuse

- **2.4 GHz:** frequency band, not a protocol.
- **PHY:** physical-layer modulation, coding and radio transmission format.
- **BLE / Bluetooth LE:** protocol stack using supported LE PHYs.
- **GATT:** application-facing service/characteristic data model, not another radio.
- **Mesh:** network organization/protocol; several incompatible technologies use this word.
- **NFC:** separate interface; not part of the 2.4 GHz radio.
- **Wi-Fi:** not native to LM20A/B; Nordic lists external nRF70 companion support.

For chip/board distinctions, see [SoC and board capabilities](../Boards/Nordic%20SoCs.md). Connectivity boundaries are documented on [Nordic's LM20A page](https://www.nordicsemi.com/Products/nRF54LM20A).

Nordic also lists Aliro at product level. Protocol listings do not establish a tested SDK/role combination. Bluetooth Classic, cellular, GNSS, LoRa and native Wi-Fi are not established capabilities of this SoC reference; Wi-Fi requires an external companion.

## Bluetooth LE channels, discovery and roles

Ordinary BLE uses 40 channels, with centers spaced 2 MHz apart from 2402 to 2480 MHz. Three primary advertising channels are numbered 37, 38 and 39; the other 37 serve data and secondary advertising. Connections hop across a channel map; adaptive channel selection can avoid poor channels.

| Role | Function |
| --- | --- |
| Broadcaster | Sends advertisements without establishing connections |
| Observer | Scans advertisements |
| Peripheral | Advertises for connection and participates in the resulting link |
| Central | Initiates and schedules connections |

A device may support multiple roles if its controller configuration and resources allow it. Central/peripheral roles are independent of GATT client/server roles.

| Communication form | What changes |
| --- | --- |
| Legacy advertising | Small broadcasts on primary channels; LE 1M; up to 31 bytes advertising data |
| Extended advertising | Can move data to secondary channels and chain packets; larger data sets |
| Passive scanning | Listens only |
| Active scanning | Requests a scan response from a scannable advertiser |
| Connected exchange | Scheduled bidirectional packet exchange between peers |

Advertising and scan-response capacity includes protocol data structures; it is not all available for arbitrary application values. Source: [Bluetooth SIG LE primer](https://www.bluetooth.com/bluetooth-le-primer/).

[Radio topic navigation](../../Overview.md#reference-library)
