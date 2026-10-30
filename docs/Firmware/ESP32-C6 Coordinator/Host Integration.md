# Coordinator and Windows host responsibilities

This note covers host process order, persisted state and converter restarts. [Firmware architecture](Architecture.md) owns component boundaries and the USB/report path.

## Startup and state

The local broker starts before Zigbee2MQTT. Preserve the existing network configuration, keys and device database during routine restarts. Runtime state stays ignored. Joining opens only for an intentional device test.

Treat serial connection, firmware version response, network formation/resume, node interview and fresh decoded reports as separate checks. A successful join alone does not establish useful telemetry. Host startup may republish cached state; inspect sequence, uptime and new timestamps.

## Converter restart behavior

The npm-hoisted Windows host needs a package-local dependency junction for external converter reloads. The project launcher supplies it without patching package source. Its exact behavior and the measured restart result belong to the operations guides.

## Canonical operations

The Waveshare launcher option installs the opt-in [display extension](Display%20UI.md) into the existing ignored host data. It uses the same Zigbee2MQTT process, installation and serialized adapter queue. Availability values are forwarded from actual host publications; missing availability stays unknown. Runtime keys/databases are not branch-versioned. Switching a board does not establish pairing migration.

[Host installation, addresses, commands and recovery limits](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/esp32c6_coordinator#run-the-windows-host). [Nordic decoder installation and tests](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/nordic_zigbee#install-and-check-the-decoder).

[Code reference](Code%20Reference.md)
