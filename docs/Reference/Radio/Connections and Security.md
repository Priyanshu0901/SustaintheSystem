# Bluetooth GATT, connection timing and security

Reference check: **2026-09-26**. For evidence levels, see [SDK support](SDK%20Support.md).

## Connections: data, timing and energy

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

## Security terms

**Pairing** establishes security material; **bonding** retains it for later connections. **Encryption**, peer authentication and application authorization are different properties. The chosen pairing method and device I/O capabilities affect protection against impersonation during pairing. A stored bond is not a complete application permission system. [Nordic pairing explanation](https://academy.nordicsemi.com/courses/bluetooth-low-energy-fundamentals/lessons/lesson-5-bluetooth-le-security-fundamentals/topic/pairing-process/).

Broadcast data does not inherit a connected link's encryption merely because it uses the same chip. Likewise, a packet CRC detects errors; it is not cryptographic authentication. Protocol-specific security and key handling must be configured in firmware.

[Radio topic navigation](../../Overview.md#reference-library)
