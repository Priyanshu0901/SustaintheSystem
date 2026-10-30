# Zigbee observation report protocol v1

HA profile `0x0104`, device ID `0xfff2`, endpoint **10**, private cluster **0xfc00**, attribute **0x0000**, ZCL octet-string type **0x41**. Basic manufacturer is `Sustain`; model is `SustainNode-v1`. The node explicitly sends periodic global Attribute Reports to coordinator address `0x0000`, HA endpoint **1**. It does not require binding/configure-reporting and does not expose runtime report-interval writes. Do not infer interoperability with arbitrary hubs from this bench contract.

The octet string carries 16 bytes, preceded on the wire by its ZCL length byte `0x10`. Multibyte integers are little-endian. No C struct is transmitted by casting or packing.

| Record byte offset | Type | Meaning |
| --- | --- | --- |
| 0 | u8 | Schema version, 1 |
| 1 | bitmap8 | Bit 0 simulated; bit 1 anomaly; other bits zero |
| 2–5 | u32 | Generated sequence, restarts on reboot; wraps after 2³² values |
| 6–9 | u32 | Uptime seconds, wraps after 2³² seconds |
| 10–11 | u16 | Score in thousandths, 0–1000; not a calibrated probability |
| 12–13 | u16 | Direction degrees, 0–359; synthetic in this build |
| 14 | u8 | 1 router; 2 sleepy end device |
| 15 | u8 | Reserved, zero |

Host decoding rejects unsupported versions, incorrect lengths, reserved bits, invalid scores/directions and unknown roles. The `simulated` flag remains true for all test records. A real inference producer must clear it deliberately, supply defined measurement semantics and retain validation evidence. Do not describe synthetic scores or rotating directions as detection/localization results. Do not put microphone frames, logs or secrets into this compact radio record.

[Firmware architecture](Architecture.md) · [Code reference](Code%20Reference.md)
