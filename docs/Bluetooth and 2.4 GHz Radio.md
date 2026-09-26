# Bluetooth and 2.4 GHz Radio Reference

Source check: **2026-09-26**. [Overview](Overview.md) · [MCU and board capabilities](MCU%20and%20Board%20Capabilities.md)

Scope: Nordic **nRF54LM20A/B**, with Bluetooth LE concepts needed to interpret their capabilities. This is factual reference material, not a proposed project architecture. No radio firmware or hardware has been tested locally.

**Read support claims at three levels:** silicon implements a radio mode; a controller/SDK implements a protocol feature; the selected board, firmware configuration and peer must support that feature together. A Bluetooth version label does not establish every optional feature.

## 1. Radio modes in the LM20A/B

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

For chip/board distinctions, see [MCU and board capabilities](MCU%20and%20Board%20Capabilities.md). Connectivity boundaries are documented on [Nordic's LM20A page](https://www.nordicsemi.com/Products/nRF54LM20A).

## 2. Bluetooth LE channels, discovery and roles

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

## 3. Connections: data, timing and energy

### GATT data exchange

A GATT server exposes services containing characteristics; a client discovers and accesses them.

| Operation | Behaviour |
| --- | --- |
| Read | Client requests a value |
| Write request | Client sends a value and receives an ATT response |
| Write command | Client sends a value without an ATT response |
| Notification | Server sends a subscribed value without ATT confirmation |
| Indication | Server sends a subscribed value and receives ATT confirmation |

Notifications still travel over a connected link with link-layer reliability. Lack of ATT confirmation does **not** mean the radio never acknowledges or retries. Conversely, confirmation does not prove that the receiving application completed an action or saved the data. Sources: [Nordic GATT exercise](https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-4-bluetooth-le-data-exchange/topic/blefund-lesson-4-exercise-2/), [GATT API](https://nrfconnectdocs.nordicsemi.com/ncs/latest/zephyr/services/connectivity/bluetooth/api/gatt.html).

### Packet sizes and throughput

**ATT MTU and link-layer Data Length Extension are different.** A common negotiated combination is ATT MTU 247, giving 244 bytes of characteristic value in an ordinary notification, carried with a 251-byte link-layer payload. Default ATT MTU 23 gives 20 value bytes for that operation. An MTU is not a radio bit rate, and larger MTUs may require fragmentation.

Delivered throughput also depends on PHY, packets permitted per event, connection timing, retransmissions, host queues and the peer. “2 Mbps BLE” does not mean 2 Mbps of application data. Source: [Nordic communication and packet-size explanation](https://academy.nordicsemi.com/courses/designing-low-power-bluetooth-le-products/lessons/lesson-1-power-consumption-essentials/topic/bluetooth-le-communication-methods/?version=v3.4.0).

### Timing controls

| Parameter | Effect |
| --- | --- |
| Connection interval | Spacing between scheduled connection events |
| Peripheral latency | Allows the peripheral to skip eligible events when appropriate |
| Supervision timeout | Time without successful communication before the link is considered lost |
| Event duration / packet budget | Radio time available to exchange data |
| PHY | Changes airtime and reception characteristics |

Longer intervals or permitted event skipping can reduce average activity, while increasing waiting time for data. A connected peripheral need not keep its receiver continuously on. Scanning windows and intervals are a separate receive-duty-cycle control. Source: [Nordic connection timing and power](https://academy.nordicsemi.com/courses/designing-low-power-bluetooth-le-products/lessons/lesson-4-bluetooth-le-power-optimization/topic/bluetooth-le-connection-parameters-and-power-consumption/).

Do not treat the commonly quoted **7.5 ms** minimum as universal across newer features. The reviewed **nrfxlib 3.4.99 development documentation** lists Shorter Connection Intervals down to **625 µs for nRF54L**, requiring the lowest supported ACL frame space, LE 2M and a 27-byte data length. It separately lists Nordic's proprietary Low Latency Packet Mode. Neither statement establishes support in an older SDK or a phone. [Controller feature table and conditions](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/README.html).

## 4. Periodic advertising and PAwR

**Periodic advertising** lets receivers synchronize to scheduled broadcasts. **PAwR**, Periodic Advertising with Responses, adds scheduled response slots within subevents, enabling two-way exchange without maintaining an ordinary ACL connection for every responder.

PAwR scheduling defines when a receiver listens and responds; the application/profile still defines payload meaning, device assignment and delivery handling. Ordinary advertising is not a reliable acknowledged command channel just because multiple devices can hear it. PAwR and Bluetooth Mesh are different mechanisms. [Bluetooth SIG 5.4 technical overview](https://www.bluetooth.com/wp-content/uploads/2023/02/2301_5.4_Tech_Overview_FINAL.pdf).

Nordic's reviewed controller table lists both PAwR advertiser and scanner support in its **multirole** library. This is controller-level evidence; the exact LM20 board build, host configuration and release maturity remain untested locally. [SoftDevice Controller feature table](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/README.html).

## 5. Channel Sounding and other ranging terms

Channel Sounding collects round-trip-time and phase/IQ measurements between an initiator and reflector; an algorithm uses these to estimate distance. It is not simply an RSSI-to-distance conversion, nor does it automatically provide a bearing or full position.

Nordic's reviewed controller documentation supports initiator/reflector roles and up to four antenna paths/elements. Multiple antennas require suitable external RF switching, GPIO configuration and timing. That does not imply four antennas are fitted to either discussed board.

The same page explicitly lists **radio coexistence and front-end modules as unsupported for its Channel Sounding implementation**. This is a dated implementation limitation, not a statement that all Bluetooth activity is impossible alongside ranging. [Nordic Channel Sounding support and limitations](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/doc/channel_sounding.html).

The official [Ranging Requestor sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/bluetooth/channel_sounding/ras_initiator/README.html) explicitly lists both LM20 DK targets:

- nrf54lm20dk/nrf54lm20a/cpuapp
- nrf54lm20dk/nrf54lm20b/cpuapp

It connects to a reflector/responder, obtains ranging data and prints estimates. This verifies a documented software path, not measured accuracy on our boards. RSSI, Channel Sounding and Direction Finding/AoA/AoD are separate capabilities; no board-specific direction-finding claim is made here.

## 6. Bluetooth Mesh, Thread, Zigbee and Matter

| Technology | Where it fits |
| --- | --- |
| Bluetooth Mesh | Mesh messaging, addressing, models and provisioning over Bluetooth LE bearers |
| Thread | IPv6 mesh networking over IEEE 802.15.4 |
| Zigbee | Separate network/application stack using IEEE 802.15.4 |
| Matter | IP-based application/interoperability layer; can operate over Thread or Wi-Fi |
| BLE commissioning | Can carry credentials during Matter setup; not the normal Matter-over-Thread data transport |

Thread and Zigbee sharing a PHY does not make their networks interoperable. Matter is not another modulation setting. Sources: [Nordic Matter transports](https://academy.nordicsemi.com/courses/matter-fundamentals/lessons/lesson-1-matter-introduction/topic/transport-layers-in-matter/?version=v3.4.0), [Matter commissioning](https://nrfconnectdocs.nordicsemi.com/addons/ncs-matter/latest/matter/overview/commissioning.html), [LM20 protocol support](https://www.nordicsemi.com/Products/nRF54LM20A).

Bluetooth Mesh includes Relay, Proxy, Friend and Low Power roles. A Friend buffers messages for a Low Power node to retrieve; low-power operation therefore does not imply every node performs the same listening work. Mesh can use managed flooding, with directed forwarding defined in newer mesh specifications. A set of ordinary BLE connections to one central is not automatically Bluetooth Mesh. [Bluetooth SIG Mesh primer](https://www.bluetooth.com/bluetooth-mesh-networking-primer/).

## 7. Proprietary radio and Enhanced ShockBurst

**Enhanced ShockBurst (ESB)** provides Nordic packet communication with buffering, acknowledgements and automatic retransmission. It has its own addressing and packet handling; it does not expose Bluetooth GATT or interoperate directly with a phone's BLE API. [Nordic ESB library](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/libraries/others/esb.html).

The [ESB transmitter sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/esb/esb_ptx/README.html) explicitly lists both LM20 DK targets. Supported hardware mode rates do not establish the default ESB sample bitrate. Packet format, bitrate, addressing and channel settings must match at both ends.

ESB is also distinct from direct RADIO programming. Direct access exposes lower-level packet and timing controls; software must supply the required protocol behaviour. Do not assume that ESB automatically supplies BLE pairing, GATT, IP routing or application security. Detailed configuration reference: [ESB user guide](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/esb/index.html).

## 8. One radio, multiple protocol stacks

Nordic's **Multiprotocol Service Layer (MPSL)** arbitrates radio timeslots. Dynamic multiprotocol support lets stacks remain active while sharing radio time; it does not turn one transceiver into two simultaneous independent receivers.

Nordic notes that BLE activity can interrupt Thread/Zigbee processing, and extensive BLE scanning can block 802.15.4 traffic. Therefore support for two protocols individually does not establish their combined latency, throughput or reliability. [Nordic multiprotocol documentation](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/multiprotocol/index.html).

The [MPSL timeslot sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/mpsl/timeslot/README.html) lists both LM20 targets. Direct RADIO use must respect stack ownership and the applicable scheduling API when a controller is active.

## 9. RF specifications, power and range

| LM20A/B receiver mode | Typical sensitivity |
| --- | --- |
| LE 1M, short packet | −95.5 dBm |
| LE 1M, long packet | −94 dBm |
| LE 2M, short packet | −93 dBm |
| LE Coded 500 kbps | −98 dBm |
| LE Coded 125 kbps | −103 dBm |
| IEEE 802.15.4 | −101 dBm |

The source uses different test conditions: short 1M packets are at most 37 bytes with BER 10⁻³; long 1M packets are at least 128 bytes with BER 10⁻⁴. These are typical laboratory sensitivities, not range guarantees or perfectly equivalent tests. [LM20 receiver electrical specification](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/elspec.html-unique_988306023).

Nordic lists maximum TX **+8 dBm for CSP / +7 dBm for QFN**, and headline currents of **3.3 mA RX / 5.0 mA TX at 0 dBm**, at **3 V**. These are vendor operating points, not whole-board average consumption. [LM20 product specifications](https://www.nordicsemi.com/Products/nRF54LM20A).

Interpretation:

- dBm expresses absolute power; dB expresses a ratio or gain/loss.
- More-negative sensitivity means weaker signals can be decoded under the stated conditions.
- RSSI measures received strength; it is not a direct packet-error measurement or calibrated distance.
- Antenna efficiency/orientation, enclosure, obstacles, interference and receiver conditions affect usable range.
- Low average current depends on time spent transmitting, receiving, starting clocks and sleeping—not merely the TX current.

Accounting identity: average current equals total charge consumed over an interval divided by that interval. A shorter packet can reduce airtime, but retries or longer receive windows can offset that reduction. No fixed range, battery life or maximum node count is established for our project.

## 10. Security terms

**Pairing** establishes security material; **bonding** retains it for later connections. **Encryption**, peer authentication and application authorization are different properties. The chosen pairing method and device I/O capabilities affect protection against impersonation during pairing. A stored bond is not a complete application permission system. [Nordic pairing explanation](https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-5-bluetooth-le-security-fundamentals/topic/pairing-process/).

Broadcast data does not inherit a connected link's encryption merely because it uses the same chip. Likewise, a packet CRC detects errors; it is not cryptographic authentication. Protocol-specific security and key handling must be configured in firmware.

## 11. SDK support: evidence and limits

nRF Connect SDK normally combines the **Zephyr Bluetooth Host** with Nordic's **SoftDevice Controller**. The controller handles link-layer/radio timing; the host provides higher-level Bluetooth functions. Standalone Zephyr's controller is a different implementation and can have a different support matrix. [Nordic stack architecture](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/bt/bt_stack_arch.html).

| Evidence checked | What it establishes |
| --- | --- |
| LM20 RADIO datasheet | Physical modes and hardware helpers |
| Controller multirole feature table | Extended/periodic advertising, PAwR, Coded PHY, subrating, Channel Sounding and isochronous features are listed |
| LM20 Channel Sounding sample | Both A/B DK targets explicitly documented |
| LM20 ESB and MPSL samples | Both A/B DK targets explicitly documented |
| Local project | No firmware build or radio test performed |

The reviewed latest NCS/nrfxlib pages display **3.4.99**, a moving development documentation version. They are not a blanket claim of support in released NCS 3.3.0 or any locally installed SDK. Controller feature tables also do not prove that a complete LE Audio product/profile, every role combination, or a specific XIAO build works.

For a concrete implementation, the remaining facts to establish are the exact SDK release, controller library/configuration, board and silicon revisions, peer support and applicable errata. No universal connection-count limit or guaranteed feature combination is asserted.

## Official documentation map

| Need | Reference |
| --- | --- |
| Learn Bluetooth concepts | [Bluetooth SIG LE primer](https://www.bluetooth.com/bluetooth-le-primer/) |
| Inspect LM20 radio hardware | [RADIO chapter](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/radio.html?contentId=~f5ojBLyjysoQg63BKuIiA) |
| Check controller feature variants | [SoftDevice Controller](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/README.html) |
| Understand ranging limitations | [Channel Sounding implementation](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/doc/channel_sounding.html) |
| Understand proprietary packets | [ESB guide](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/esb/index.html) |
| Understand shared radio scheduling | [Multiprotocol support](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/multiprotocol/index.html) |

Maintain this note in place as supported releases and verified board details change. Internal document links are relative to preserve navigation when the project directory is renamed.

