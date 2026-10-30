# nRF54LM20A and LM20B capabilities

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

| Name | What it is |
| --- | --- |
| MCU | Processor, memory and control peripherals |
| Wireless SoC | MCU functionality plus radio and other integrated hardware |
| nRF54LM20A / nRF54LM20B | Two wireless SoCs within Nordic's nRF54L series |
| nRF54LM20 DK | Nordic development board for these SoCs |
| XIAO nRF54LM20A Sense | Seeed board combining the LM20A with additional components and sensors |

## Processing and memory

| Capability | LM20A | LM20B |
| --- | --- | --- |
| Main CPU | 128 MHz Arm Cortex-M33 | Same |
| CPU computation | DSP instructions and floating-point unit | Same |
| Coprocessor | 128 MHz RISC-V, including SoftPeripheral support | Same |
| Internal nonvolatile memory | 2036 KB RRAM; marketed as 2 MB NVM | Same |
| RAM | 512 KB | Same |
| Dedicated Axon NPU | Absent | 128 MHz accelerator |

The B adds acceleration; it does not add RAM or a faster main CPU. CPU-based models can run without an NPU. Memory available to application data is less than the total once firmware, stacks, buffers and runtime allocations are included. Sources: [LM20B product specification](https://www.nordicsemi.com/Products/nRF54LM20B), [LM20A/B datasheet](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html).

## Electrical limits and security

LM20A/B operating envelope: **1.7–3.6 V**, **−40 to +85°C**. Package-dependent GPIO counts are **66 (CSP98), 40 (CSP61), 32 (QFN52)**. These limits apply to the chip, not automatically the assembled board. [Nordic family/package table](https://www.nordicsemi.com/Products/nRF54LM20A).

Radio current and TX figures: [RF and power](../Radio/RF%20and%20Power.md).

TrustZone isolation, cryptographic acceleration, key management, secure boot/storage facilities, tamper detection and side-channel protections are documented. Their presence does not mean project firmware has configured them. [LM20A/B security features](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html).

[Interfaces](Audio%20and%20USB.md) · [Axon NPU](Axon%20NPU.md) · [Radio modes](../Radio/Modes%20and%20Roles.md) · [Board/SDK compatibility](SDK%20Compatibility.md)
