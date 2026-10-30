# RF sensitivity, power and range

Reference check: **2026-09-26**. For evidence levels, see [SDK support](SDK%20Support.md).

## RF specifications, power and range

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

[Radio topic navigation](../../Overview.md#reference-library)
