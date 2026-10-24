---
status: draft
updated: 2026-10-25
---

# Story

[Hackster Story editor](https://www.hackster.io/projects/0ce81d/edit#story) · [[Hackster/Basics|Basics]] · [[Hackster/Things|Things]]

Only the text between the article markers is intended for Hackster's Story section. Project overview: [[Overview]].

<!-- BEGIN HACKSTER ARTICLE -->

A motor that sounds different is worth paying attention to. Picking it out while a pump, two fans and everything else in the room are running is another matter. A microphone hears the whole room; an operator needs to know which machine to inspect.

I’m building a non-contact acoustic monitor for that situation. It will sit near rotating machinery, detect changes in operating sound and associate those changes with a source direction. The microphone array supplies spatial information, and an embedded model looks for departures from normal operation.

Vibration and temperature sensors already do useful work in condition monitoring. They usually require installation on individual machines. Here, I’m using sound to monitor from a nearby position, with the aim of helping an operator identify equipment that needs inspection before a small problem becomes a reason to replace it.

![Industrial Motor Room](assets/industrial-motor-room-pexels-magda-ehlers-pexels-2569839.jpg)

The monitoring node is built around the Nordic nRF54LM20 DK, targeting the nRF54LM20B and its Axon Neural Processing Unit (NPU). Audio processing and inference will run locally. Zigbee will carry reports to a local monitoring application, while Bluetooth Low Energy (BLE) will provide direct access during maintenance.

![Signal and access paths](assets/system-flow.png)

Each node will store event times, anomaly scores, operating-state information and direction estimates. Sending those observations instead of a continuous audio stream keeps the network traffic small and leaves the acoustic processing independent of the monitoring application. If the application disconnects, the node will continue collecting observations for later retrieval.

I already have XIAO nRF54LM20A Sense boards, so I’ll start microphone capture, feature extraction and CPU-based inference on those while the DKs arrive. Their built-in microphones also give me a single-microphone baseline and a way to test lower-cost monitoring nodes. The A-series boards will run their own resource-limited workload; the nRF54LM20B remains the target for NPU-accelerated inference and the array-based system.

## Microphone Array and Audio Capture

I’m using the Sipeed MAIX R6+1 seven-microphone array to capture the same acoustic scene from different positions. The relative timing between those channels is essential: direction estimation depends on differences in the signals, not just seven recordings of roughly the same moment.

The acquisition path will capture the channels together and preserve their alignment as samples move into the Nordic processing pipeline. I’ll check channel timing and sustained throughput before using the recordings for spatial analysis. A direction estimate is not useful if the capture system has introduced the delay it is trying to measure.

![Sipeed MAIX R6+1](assets/r6_plus_1_microphone_positions.png)

For spatial processing, I’ll use delay-and-sum beamforming to emphasize sound from a selected direction and evaluate time-difference-of-arrival estimates using GCC-PHAT to calculate delays between channels.

I’ll compare two processing arrangements. The first forms directional audio before running the anomaly model. The second detects a change in mixed audio and then examines the spatial information associated with it. The comparison will use correct-machine association and processing cost as well as angular error. Finding the loudest fan is not necessarily finding the abnormal one.

Reflections complicate this because the same sound reaches the array along more than one path. Nearby machines also share parts of the frequency spectrum. The output will therefore identify a likely direction or associate the event with a known machine position; the first implementation will not attempt precise room coordinates.

## Acoustic Anomaly Detection

The model will learn normal operating sound and produce an anomaly score when an observation differs from it. Starting with normal operation avoids requiring a labelled recording of every fault the equipment might develop.

I’ll begin with spectral analysis and log-mel spectrograms, using FFT and short-time Fourier processing to examine how the signal changes over time. Compact autoencoders will provide the initial model baseline. Feature extraction and inference will be measured together, since a small network does not guarantee a small processing workload.

MIMII and DCASE machinery recordings will provide the first offline training and evaluation data. I’ll keep recordings for training, threshold selection and testing separate, and record the dataset version and operating conditions with the results. Physical recordings from the test setup will then show how the model behaves with this array and these machines.

Training and quantization will run on a computer. I’ll deploy the quantized model to the Nordic device and check operator support, model size, RAM and inference time alongside its detection performance. Acquisition, feature extraction, storage and communication must all fit around that workload.

The NPU will receive a quantized feature tensor, not raw microphone samples. Firmware will apply the training-time normalization, quantize with the model’s scale and zero point, and pack the input in the layout required by the compiled model. For the autoencoder baseline, the CPU will compare the reconstructed features with the input to calculate the anomaly score. Buffers will remain valid until processing completes.

![Audio processing and Axon inference](assets/audio-axon-pipeline.png)

For detection-first processing, I’ll use a reference microphone for the model and retain the aligned multichannel window for source-direction analysis. I’ll compare this with beamforming before detection under the same recording conditions.

An anomaly score is a reason to inspect, not a diagnosis of bearing damage. Changes in speed, load and background noise also change the recording. Those normal variations will be included in testing so that a different operating condition does not automatically become a maintenance alert.

> **Image to add after recording:** Normal and abnormal spectrograms with identical axes and colour scales. Include the machine condition, speed and microphone position in the caption. Use the actual recordings from the tests.

## Wireless Monitoring and Diagnostics

The XIAO ESP32C6 will coordinate the Zigbee network and connect to the monitoring computer over USB. Its firmware will form the network, commission nodes, receive reports and pass diagnostic requests to the selected node. The monitoring application will run locally without an internet dependency.

![Monitoring network and maintenance access](assets/network-topology.png)

The Nordic nodes will operate as Zigbee Sleepy End Devices. They will send periodic operating summaries and report significant anomalies. Remote requests will be collected when a node polls its parent, so the polling interval will set the balance between response time and radio activity.

The application will maintain a node registry, cache reports and display their age. Timestamps, sequence numbers and timeouts will identify stale information without querying every node whenever the display refreshes. If reporting stops, the application will mark the node unavailable. A missing packet does not tell us whether the battery, hardware or radio link is responsible.

For on-site diagnostics, the nRF node will expose its identity and Bluetooth commissioning information as an NFC tag. I’ll connect the ACS ACR1552U reader to the Android device over USB. The companion application will read the node’s tag through that reader, use the returned details to identify the correct Bluetooth device, and establish an authenticated BLE connection to retrieve its history.

The nRF is the tag, the ACS unit is the reader, and Android handles the Bluetooth connection. The reader is part of the maintenance interface in the final product, not just a bench-testing tool. This workflow will work independently of the Zigbee network and will not require built-in NFC on the Android device. Reading the tag supplies commissioning data; the BLE connection and authorization are handled by the application.

The node will retain detailed records and acoustic features for direct retrieval instead of adding them to every network report. I’ll test tag reading through the USB reader, device identification, authorized BLE access and transfer while acoustic processing is active. Zigbee and BLE share radio resources, so connecting to an otherwise idle board will not establish that the complete system works.

![On-site diagnostics (illustration only)](assets/maintenance-workflow.png)

## Power Management

I’ll capture a window of audio, stop acquisition, extract its features and run inference. Once the result is processed, the CPU will wait for the next capture timer. Radio events will continue to run on the Zigbee and BLE schedules, and the Axon driver will return the NPU to its idle state between jobs.

![Capture and reporting cycle](assets/low-power-operation.png)

An anomaly will queue an event report; otherwise, the node will accumulate statistics for its next status update. BLE diagnostics will run as a separate session and close on disconnect or timeout. The firmware will wait for capture and inference completion through events rather than poll for them. I’ll also disable unused peripherals, console output and LEDs when measuring consumption.

I’ll set the sampling interval by comparing energy use and missed events against continuous recordings. Measurements will include the microphone array and capture interface, including their idle current and settling time when capture restarts.

## Testing and Validation

I’ll use two low-voltage fans or motors running together for the first physical tests. One will run normally. For the other, I’ll use a worn fan or change the mounting stiffness to introduce a repeatable difference in its sound. I’ll record exactly what I changed, rather than attach a fault label based only on how unpleasant it sounds.

Each session will include the operating speed, source positions and background noise. I’ll move the sources and vary the noise so that the comparison does not depend on one convenient arrangement.

> **Image to add:** A top-down test layout showing fan A, fan B, the array orientation, angles and distances. Follow it with a photograph once assembled. Use the same labels in the recording notes and result plots.

The same recordings and conditions will be evaluated with one microphone and the array. The array adds capture, memory, processing and energy costs, so I’ll measure whether it improves association with the abnormal source enough to justify them. Results will include detection performance, false alarms, direction error and correct-machine association.

I’ll validate synchronization, sample throughput and sample loss before interpreting direction results. Embedded measurements will cover model size, RAM, feature-extraction time, inference latency and total processing time per audio window.

Wireless commissioning and reporting will first use clearly labelled simulated measurements. After those paths work, I’ll connect real acoustic observations and test the complete sequence: a change at the machine, detection and source estimation at the node, a report in the monitoring application, and diagnostic retrieval beside the equipment. Delivery, request latency, stale-node detection and reconnection will be measured separately so that network failures do not get confused with detector failures.

Power testing will separate acquisition, feature extraction, inference, Zigbee, BLE and idle consumption. Current measurements will be aligned with those processing stages so that the duty-cycle comparison includes the whole node, not only the NPU or radio.

## Farm and Field Monitoring

A future version will extend the same monitoring approach to irrigation pumps, farm motors and equipment spread across larger outdoor sites. That version will use LoRa for reports between field nodes and a gateway in place of the local Zigbee network. Acoustic processing will remain on each node, and BLE will remain the direct maintenance interface.

The field deployment will need its own enclosure, power budget and communication tests. The data sent over the radio will still be compact condition observations and alerts, keeping the machine-monitoring function separate from the choice of network.

## References

- [Nordic nRF54LM20 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF54LM20-DK) and [nRF54LM20A/B specification](https://docs.nordicsemi.com/r/bundle/ps_nrf54lm20a/page/keyfeatures_html5.html) — development platform and A/B hardware differences.
- [XIAO nRF54LM20A Sense documentation](https://github.com/Seeed-Studio/wiki-documents/blob/docusaurus-version/sites/en/docs/Sensor/SeeedStudio_XIAO/SeeedStudio_XIAO_nRF54LM20A-Sense/XIAO_nRF54LM20A-Sense_Getting_Started.md) — onboard microphone and board peripherals.
- [Sipeed R6+1 microphone-array datasheet](https://dl.sipeed.com/MAIX/HDK/Sipeed-R6%2B1_MicArray/Specifications/Sipeed%20R6%2B1%20Microphone%20Array%20Datasheet%20V1.0.pdf) — array hardware reference.
- [ACS ACR1552U technical specification](https://www.acs.com.hk/download-manual/13248/TSP-ACR1552U-1.04.pdf) — USB reader capabilities and Android platform support.
- [Nordic NFC pairing sample](https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/samples/bluetooth/peripheral_nfc_pairing/README.html) — NFC tag-side Bluetooth handover reference; the Android application will handle the external-reader workflow.
- [Axon model compiler](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/axon_compiler/Axon_npu_tflite_compiler.html) — quantized-model conversion and compatibility checks.
- [Axon inference integration](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/integrations/axon.html) — model input layout, buffers and inference completion.
- [Axon supported operators](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/axon_compiler/supported_operators.html) — model compatibility and quantization guidance.
- [Axon low-power sample](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/samples/axon/axon_low_power/README.html) — inference scheduling and power profiling.
- [MIMII dataset](https://zenodo.org/records/3384388) and [DCASE 2025 Task 2](https://dcase.community/challenge2025/task-first-shot-unsupervised-anomalous-sound-detection-for-machine-condition-monitoring) — machinery-sound data and evaluation references.

<!-- END HACKSTER ARTICLE -->
