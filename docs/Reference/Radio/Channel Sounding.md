# Bluetooth Channel Sounding and ranging

Reference check: **2026-09-26**. For evidence levels, see [SDK support](SDK%20Support.md).

## Channel Sounding and other ranging terms

Channel Sounding collects round-trip-time and phase/IQ measurements between an initiator and reflector; an algorithm uses these to estimate distance. It is not simply an RSSI-to-distance conversion, nor does it automatically provide a bearing or full position.

Nordic's reviewed controller documentation supports initiator/reflector roles and up to four antenna paths/elements. Multiple antennas require suitable external RF switching, GPIO configuration and timing. That does not imply four antennas are fitted to either discussed board.

The same page explicitly lists **radio coexistence and front-end modules as unsupported for its Channel Sounding implementation**. This is a dated implementation limitation, not a statement that all Bluetooth activity is impossible alongside ranging. [Nordic Channel Sounding support and limitations](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/doc/channel_sounding.html).

The official [Ranging Requestor sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/bluetooth/channel_sounding/ras_initiator/README.html) explicitly lists both LM20 DK targets:

- nrf54lm20dk/nrf54lm20a/cpuapp
- nrf54lm20dk/nrf54lm20b/cpuapp

It connects to a reflector/responder, obtains ranging data and prints estimates. This verifies a documented software path, not measured accuracy on our boards. RSSI, Channel Sounding and Direction Finding/AoA/AoD are separate capabilities; no board-specific direction-finding claim is made here.

[Radio topic navigation](../../Overview.md#reference-library)
