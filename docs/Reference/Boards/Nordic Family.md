# Related Nordic nRF54 family members

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

NVM figures below are Nordic's rounded product labels. All listed nRF54L parts have a 128 MHz Cortex-M33; peripheral and security sets differ.

| Part | NVM / RAM | Distinction |
| --- | --- | --- |
| nRF54LM20A | 2 MB / 512 KB | Extended peripherals, USBHS |
| nRF54LM20B | 2 MB / 512 KB | LM20 capability plus Axon |
| nRF54L15 | 1.5 MB / 256 KB | Five serial instances, PDM/I²S; no LM20 USBHS/NPU |
| nRF54L10 | 1 MB / 192 KB | Smaller-memory L variant |
| nRF54L05 | 0.5 MB / 96 KB | Smaller-memory L variant |

Source: [Nordic L15 family comparison](https://www.nordicsemi.com/Products/nRF54L15).

| Additional variant | NVM / RAM | Distinction |
| --- | --- | --- |
| [nRF54LC10A](https://www.nordicsemi.com/Products/nRF54LC10A) | 1 MB / 192 KB | Three serial instances; compact packages; shipping hibernation specified below 50 nA |
| [nRF54LV10A](https://www.nordicsemi.com/Products/nRF54LV10A) | 1 MB / 192 KB | Low-voltage supply and GPIO: 1.2–1.7 V |
| [nRF54LS05A](https://www.nordicsemi.com/Products/nRF54LS05A) | 0.5 MB / 64 KB | Entry-level Bluetooth/proprietary-radio part; baseline security |
| [nRF54LS05B](https://www.nordicsemi.com/Products/nRF54LS05B) | 0.5 MB / 96 KB | More RAM than LS05A; baseline security |

Do not transfer the LM20 TrustZone, NPU, coprocessor or protocol list to every variant. Pin compatibility is package-specific and sometimes qualified with limitations. Announced capability does not establish distributor stock. [Nordic comparison and package table](https://www.nordicsemi.com/Products/nRF54LM20A).

**nRF54H20 is a separate branch**, not an LM20 revision: multiple Cortex-M33 processors up to 320 MHz, RISC-V coprocessors, 2 MB NVM, 1 MB RAM, high-speed USB and CAN FD. Its features must not be attributed to LM20A/B. [Nordic H20 specification](https://www.nordicsemi.com/Products/nRF54H20).

[Project SoC reference](Nordic%20SoCs.md)
