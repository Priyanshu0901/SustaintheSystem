# Sources

Board and radio notes retain their source-review dates and dated follow-ups. Vendor documentation establishes capabilities; measured project results belong to each firmware integration's validation record.

## Hardware and software

- [Nordic nRF54LM20 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF54LM20-DK) — development-kit documentation.
- [nRF54LM20A/B product specification](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html) — silicon and peripheral specifications.
- [Axon model compiler](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/axon_compiler/Axon_npu_tflite_compiler.html) — model conversion and support checks.
- [Sipeed R6+1 datasheet, v1.0](https://dl.sipeed.com/MAIX/HDK/Sipeed-R6%2B1_MicArray/Specifications/Sipeed%20R6%2B1%20Microphone%20Array%20Datasheet%20V1.0.pdf) — manufacturer array reference; confirm the actual board revision before wiring.
- [Sipeed R6+1 schematic, 11.16](https://dl.sipeed.com/fileList/MAIX/HDK/Sipeed-R6%2B1_MicArray/Sipeed-R6%2B1_MicArray_11.16/Sipeed-R6%2B1_MicArray_11.16%28Schematic%29.pdf) — seven microphone devices with shared clocks and four data lines. Checked 2026-10-25; the generic v1.0 datasheet contains inconsistent microphone-count text.
- [Nordic TDM overview](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/tdm.html-concept_tdm_overview) and [pin configuration](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/tdm.html-concept_uyv_try_vr) — serial audio SDIN/SDOUT and clock signals. Checked 2026-10-25; these do not establish direct acquisition of the array's four data lines.
- [Nordic multiprotocol documentation](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/protocols/multiprotocol/index.html) — radio scheduling and coexistence.
- [Nordic Zigbee R23 architectures](https://nrfconnectdocs.nordicsemi.com/addons/zigbee-r23/latest/architectures.html) and [router template](https://nrfconnectdocs.nordicsemi.com/addons/ncs-zigbee/latest/samples/template/README.html) — single-chip end-device/router roles and nRF54LM20 DK targets. Checked 2026-10-25; these do not establish a tested XIAO firmware build.

- [ACS ACR1552U technical specification](https://www.acs.com.hk/download-manual/13248/TSP-ACR1552U-1.04.pdf) — USB NFC reader; manufacturer lists Android support. Actual USB host and companion-app integration still needs validation.
- [XIAO nRF54LM20A Sense board documentation](https://github.com/Seeed-Studio/wiki-documents/blob/docusaurus-version/sites/en/docs/Sensor/SeeedStudio_XIAO/SeeedStudio_XIAO_nRF54LM20A-Sense/XIAO_nRF54LM20A-Sense_Getting_Started.md) — onboard digital microphone and peripherals.
- [Nordic NFC pairing sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/bluetooth/peripheral_nfc_pairing/README.html) — NFC tag-side Bluetooth handover example; external ACS/Android integration is application work.

- [Axon inference integration](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/integrations/axon.html) — compiled-model input/output contract, channel-first buffers, completion and NPU idle management.
- [Axon supported operators](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/axon_compiler/supported_operators.html) — model compatibility and quantization guidance.
- [Axon low-power sample](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/samples/axon/axon_low_power/README.html) — duty-cycled inference and profiling with reference mel frames. Its sample current is not a whole-node acoustic measurement.

The [[Reference/Boards/Nordic SoCs|board reference]] and [[Reference/Radio/Modes and Roles|radio reference]] cite additional sources beside individual technical claims.

## Zigbee coordinator

Checked 2026-10-25; these sources describe upstream capabilities and constraints. The coordinator topic records actual project validation separately.

- [tostmann/esp-coordinator](https://github.com/tostmann/esp-coordinator) and [XIAO antenna build option](https://github.com/tostmann/esp-coordinator/blob/master/main/Kconfig.projbuild) — maintained ZBOSS NCP firmware and board-specific RF switching.
- [Coordinator web flasher](https://install.busware.de/zboss/) — browser-based firmware installation and separate recovery tooling.
- [Zigbee2MQTT ZBOSS adapter](https://www.zigbee2mqtt.io/guide/adapters/zboss.html) — experimental status and host feature limitations; [custom device support](https://www.zigbee2mqtt.io/advanced/support-new-devices/01_support_new_devices.html) — device definitions/converters for application data.
- [Seeed XIAO ESP32-C6 guide](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/) and [Espressif ESP32-C6-DevKitC-1 guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html) — board interfaces and antenna references.
- [Official ESP-Zigbee-SDK](https://github.com/espressif/esp-zigbee-sdk) — custom application alternative; pin its stack generation and ESP-IDF compatibility separately from NCP firmware.
- [Pinned coordinator source](https://github.com/tostmann/esp-coordinator/tree/b292868d1ae5426dc048ee7749d7a710e1ccd789), [project fork](https://github.com/Priyanshu0901/esp-coordinator) and [archived original](https://github.com/andryblack/esp-coordinator) — parent selection and immutable source provenance; [[Firmware/ESP32-C6 Coordinator/Overview|coordinator design and usage]].
- [ESP-IDF Docker image guide](https://docs.espressif.com/projects/esp-idf/en/v5.5.1/esp32c6/api-guides/tools/idf-docker-image.html) — official toolchain container for automated builds.
- [Waveshare ESP32-C6-LCD-1.9 family guide](https://docs.waveshare.com/ESP32-C6-LCD-1.9), [touch-board schematic](https://files.waveshare.com/wiki/ESP32-C6-LCD-1.9/ESP32-C6-Touch-LCD-1.9-Schematic.pdf) and [pinned ESP-IDF display/touch example](https://github.com/waveshareteam/ESP32-C6-LCD-1.9/tree/1fed15e32977de11afe1f710a0f5e915bfcc35a5/02_Example/ESP-IDF/07_LVGL_Test) — pin/control/portrait-offset references checked 2026-10-30. [Implemented profile](../Firmware/ESP32-C6%20Coordinator/Board%20Profiles.md) and [display interface](../Firmware/ESP32-C6%20Coordinator/Display%20UI.md) describe project code.
- [Zigbee2MQTT on Windows](https://www.zigbee2mqtt.io/guide/installation/05_windows.html), [MQTT settings](https://www.zigbee2mqtt.io/guide/configuration/mqtt.html) and [official Mosquitto downloads](https://mosquitto.org/download/) — native PC host installation and broker configuration. Local host preparation does not establish coordinator interoperability.

## Nordic Zigbee test firmware

- [Zigbee R23 v1.4.0 source and documentation](https://github.com/nrfconnect/ncs-zigbee/tree/bc7863e99cbbe121d8755bc49586e977cd46d032) — release notes list LM20A/B support and NCS 3.4.0 compatibility; includes API headers, sleepy-role guidance and dedicated NVRAM layouts. The local project builds against installed NCS 3.4.1; this is separately validated integration, not a vendor compatibility claim.
- [Seeed XIAO nRF54LM20A getting started](https://wiki.seeedstudio.com/xiao_nrf54lm20a_getting_started/) and [pinned Seeed board source](https://github.com/Seeed-Studio/platform-seeedboards/tree/6a313a19f06994909627250a5e082e637bd34ff6/zephyr/boards/arm/xiao_nrf54lm20a) — board target, GPIO/PWM pin control and programming support. The user's tested local LED configuration is the project hardware reference.
- [Nordic LM20A/B SDK compatibility matrix](https://docs.nordicsemi.com/r/bundle/comp_matrix_nrf54lm20a/page/comp/nrf54lm20a/nrf54lm20a_nrf_connect_sdk.html) — support depends on silicon revision and protocol/add-on version; actual boards still require identification.
- [Zigbee2MQTT external converters](https://www.zigbee2mqtt.io/advanced/more/external_converters.html) and [external JavaScript setting](https://www.zigbee2mqtt.io/guide/configuration/all-settings.html#enable_external_js) — data-directory installation and explicit host enablement. [Observation report protocol](../Firmware/Nordic%20Zigbee/Report%20Protocol.md) records the bench contract.

## Acoustic datasets and evaluation

- [MathWorks acoustic beamforming example](https://www.mathworks.com/help/audio/ug/acoustic-beamforming-using-a-microphone-array.html) — time-delay alignment, directional interference suppression and input/output SINR evaluation in a speech example. Supports the processing explanation, not a measured machinery-noise result or a MATLAB dependency for this project.
- [MIMII dataset, public 1.0](https://zenodo.org/records/3384388) — normal and anomalous industrial-machine recordings, dataset description and licence.
- [MIMII baseline code](https://github.com/MIMII-hitachi/mimii_baseline) — dataset authors' anomaly-detection baseline.
- [DCASE 2025 Task 2](https://dcase.community/challenge2025/task-first-shot-unsupervised-anomalous-sound-detection-for-machine-condition-monitoring) — machine-condition anomaly-detection task, datasets and evaluation protocol. This is a reference edition; the project has not selected a dataset release yet.

## Sustain the System - [Contest brief](https://www.hackster.io/contests/sustain-the-system)

## Photographs

- [Industrial Pumps at Dubai Power Plant](https://www.pexels.com/photo/industrial-pumps-at-dubai-power-plant-35645884/) — Mumtaz Niazi; cover source supplied by the user.
- [Gray and Red Industrial Machine](https://www.pexels.com/photo/gray-and-red-industrial-machine-2569839/) — Magda Ehlers; industrial motor room photograph.
- [Pexels License](https://www.pexels.com/license/) — free use and modification permitted under its terms. Photographer names and source-page licence links checked 2026-10-25.
