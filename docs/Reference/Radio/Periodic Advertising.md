# Periodic advertising and PAwR

Reference check: **2026-09-26**. For evidence levels, see [SDK support](SDK%20Support.md).

## Periodic advertising and PAwR

**Periodic advertising** lets receivers synchronize to scheduled broadcasts. **PAwR**, Periodic Advertising with Responses, adds scheduled response slots within subevents, enabling two-way exchange without maintaining an ordinary ACL connection for every responder.

PAwR scheduling defines when a receiver listens and responds; the application/profile still defines payload meaning, device assignment and delivery handling. Ordinary advertising is not a reliable acknowledged command channel just because multiple devices can hear it. PAwR and Bluetooth Mesh are different mechanisms. [Bluetooth SIG 5.4 technical overview](https://www.bluetooth.com/wp-content/uploads/2023/02/2301_5.4_Tech_Overview_FINAL.pdf).

Nordic's reviewed controller table lists both PAwR advertiser and scanner support in its **multirole** library. This is controller-level evidence; the exact LM20 board build, host configuration and release maturity remain untested locally. [SoftDevice Controller feature table](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrfxlib/softdevice_controller/README.html).

[Radio topic navigation](../../Overview.md#reference-library)
