# Nordic radio SDK support and evidence

Reference check: **2026-09-26**. Silicon modes, SDK features and a tested board/peer combination are separate evidence levels.

## SDK support: evidence and limits

nRF Connect SDK normally combines the **Zephyr Bluetooth Host** with Nordic's **SoftDevice Controller**. The controller handles link-layer/radio timing; the host provides higher-level Bluetooth functions. Standalone Zephyr's controller is a different implementation and can have a different support matrix. [Nordic stack architecture](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/bt/bt_stack_arch.html).

| Evidence checked | What it establishes |
| --- | --- |
| LM20 RADIO datasheet | Physical modes and hardware helpers |
| Controller multirole feature table | Extended/periodic advertising, PAwR, Coded PHY, subrating, Channel Sounding and isochronous features are listed |
| LM20 Channel Sounding sample | Both A/B DK targets explicitly documented |
| LM20 ESB and MPSL samples | Both A/B DK targets explicitly documented |
| Reference scope | No feature tests performed for this reference; project Zigbee validation is recorded with the integration |

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

[Radio topic navigation](../../Overview.md#reference-library)
