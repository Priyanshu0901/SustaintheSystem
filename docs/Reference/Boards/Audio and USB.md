# Nordic audio, USB and peripheral interfaces

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

| Function | Documented hardware capability |
| --- | --- |
| Serial interfaces | Seven EasyDMA serial instances; alternative functions share resources |
| SPI | One up to 32 MHz; six up to 8 MHz |
| UART | One up to 4 Mbps; six up to 1 Mbps |
| I²C / TWI | Six up to 400 kHz |
| ADC | Eight programmable-gain channels; 10-bit up to 2 MS/s; oversampled 12-bit at 125 kS/s or 14-bit at 62.5 kS/s |
| Audio inputs/interfaces | PDM; TDM configurable as I²S |
| PWM | Three four-channel units |
| Counters | Seven 32-bit timers; two quadrature decoders |
| Supervision | Two watchdogs; temperature sensor |
| Wake and timing | Global RTC in System OFF; comparator/low-power comparator wake support |
| NFC | NFC-A listening peripheral |
| Debug | SWD and trace facilities |

These are SoC resources, not a promise that every board exposes every signal. ADC rate/resolution combinations are alternatives, not 14-bit conversion at 2 MS/s. Source: [LM20A/B peripheral specifications](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html).

### Audio acquisition

Audio acquisition follow-up checked **2026-10-10**: the [LM20A/B PDM peripheral](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/pdm.html) documents one/two microphone inputs, hardware decimation to **16-bit PCM**, nominal 8/16/32/48 kHz output and EasyDMA. The [register table](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/pdm.html-topic) lists high-pass pole/disable controls; no configured cutoff/default or SDK driver exposure was established here. The [TDM peripheral](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/tdm.html) supports mono/stereo I²S, EasyDMA, 8/16/24/32-bit sample widths and controller/target clocks. These are hardware capabilities, not a built sample or a promise every microphone clock/rate/format works. Exact wiring, driver, filter and resource compatibility need verification. This targeted follow-up does not refresh the full catalog.

### USB

USBHS is a USB 2.0 **device** peripheral: documented modes include 480 Mbps high speed, 12 Mbps full speed and 1.5 Mbps low speed; up to 16 bidirectional endpoints and DMA access. OTG is not supported. These are bus rates, not guaranteed application transfer rates. A board's USB-C connector does not by itself establish which SoC signals it exposes. Source: [USBHS specification](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/usbhs.html).

[SoC resources](Nordic%20SoCs.md) · [XIAO sensors](XIAO%20Sense.md)
