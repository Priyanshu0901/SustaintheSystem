# Mesh protocols, proprietary radio and coexistence

Reference check: **2026-09-26**. For evidence levels, see [SDK support](SDK%20Support.md).

## Bluetooth Mesh, Thread, Zigbee and Matter

| Technology | Where it fits |
| --- | --- |
| Bluetooth Mesh | Mesh messaging, addressing, models and provisioning over Bluetooth LE bearers |
| Thread | IPv6 mesh networking over IEEE 802.15.4 |
| Zigbee | Separate network/application stack using IEEE 802.15.4 |
| Matter | IP-based application/interoperability layer; can operate over Thread or Wi-Fi |
| BLE commissioning | Can carry credentials during Matter setup; not the normal Matter-over-Thread data transport |

Thread and Zigbee sharing a PHY does not make their networks interoperable. Matter is not another modulation setting. Sources: [Nordic Matter transports](https://academy.nordicsemi.com/courses/matter-fundamentals/lessons/lesson-1-matter-introduction/topic/transport-layers-in-matter/?version=v3.4.0), [Matter commissioning](https://nrfconnectdocs.nordicsemi.com/addons/ncs-matter/latest/matter/overview/commissioning.html), [LM20 protocol support](https://www.nordicsemi.com/Products/nRF54LM20A).

Bluetooth Mesh includes Relay, Proxy, Friend and Low Power roles. A Friend buffers messages for a Low Power node to retrieve; low-power operation therefore does not imply every node performs the same listening work. Mesh can use managed flooding, with directed forwarding defined in newer mesh specifications. A set of ordinary BLE connections to one central is not automatically Bluetooth Mesh. [Bluetooth SIG Mesh primer](https://www.bluetooth.com/bluetooth-mesh-networking-primer/).

## Proprietary radio and Enhanced ShockBurst

**Enhanced ShockBurst (ESB)** provides Nordic packet communication with buffering, acknowledgements and automatic retransmission. It has its own addressing and packet handling; it does not expose Bluetooth GATT or interoperate directly with a phone's BLE API. [Nordic ESB library](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/libraries/others/esb.html).

The [ESB transmitter sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/esb/esb_ptx/README.html) explicitly lists both LM20 DK targets. Supported hardware mode rates do not establish the default ESB sample bitrate. Packet format, bitrate, addressing and channel settings must match at both ends.

ESB is also distinct from direct RADIO programming. Direct access exposes lower-level packet and timing controls; software must supply the required protocol behaviour. Do not assume that ESB automatically supplies BLE pairing, GATT, IP routing or application security. Detailed configuration reference: [ESB user guide](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/esb/index.html).

## One radio, multiple protocol stacks

Nordic's **Multiprotocol Service Layer (MPSL)** arbitrates radio timeslots. Dynamic multiprotocol support lets stacks remain active while sharing radio time; it does not turn one transceiver into two simultaneous independent receivers.

Nordic notes that BLE activity can interrupt Thread/Zigbee processing, and extensive BLE scanning can block 802.15.4 traffic. Therefore support for two protocols individually does not establish their combined latency, throughput or reliability. [Nordic multiprotocol documentation](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/multiprotocol/index.html).

The [MPSL timeslot sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/mpsl/timeslot/README.html) lists both LM20 targets. Direct RADIO use must respect stack ownership and the applicable scheduling API when a controller is active.

[Radio topic navigation](../../Overview.md#reference-library)
