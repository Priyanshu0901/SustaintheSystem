# Hardware Application Questions

User-supplied application questions, recorded 2026-10-24.

[[Hackster/Basics|Basics]] · [[Hackster/Things|Things]] · [[Hackster/Story|Story]]

## Part 1 — Project proposal

1. Title of your idea  
   Maximum 140 characters.

```text
Acoustic Fault Detection and Localization
```

2. What are you going to build?

```text
I’m building a non-contact condition-monitoring system for motors, pumps and fans using the Nordic nRF54LM20 DK and a Sipeed MAIX R6+1 seven-microphone array.

The monitoring node will process audio locally, detect departures from normal operating sound and estimate the direction of the abnormal source. Zigbee will carry compact status reports and maintenance alerts through a XIAO ESP32C6 coordinator to a local monitoring application. An ACS ACR1552U USB reader connected to Android will read the NFC tag for Bluetooth commissioning details, and the Android application will retrieve diagnostic history over BLE.

The first demonstration will use two low-voltage fans or motors running together. I’ll compare one microphone with the array to measure whether spatial processing helps identify which machine needs inspection. A later field version will use LoRa reporting for farm motors and irrigation pumps spread across larger sites.
```

3. What problem is it going to solve? How is it different from existing solutions? Why is it useful? How does it address predictive maintenance?

```text
When several machines run in the same space, an unusual sound is difficult to trace to its source. Detecting a change in mixed audio is only part of the problem; the operator also needs to know which machine to inspect.

Vibration and temperature sensors provide valuable condition information but often need to be installed on individual machines. This project adds a nearby, non-contact monitoring approach. Its focus is combining local acoustic anomaly detection with microphone-array direction estimation, then giving the operator the event history and likely source through a local application and direct Bluetooth diagnostics.

This supports predictive maintenance by flagging departures from normal operation for earlier inspection and repair. Repeated observations will provide a record of how the equipment’s sound changes over time. The intended benefit is to keep equipment in service and reduce avoidable replacement, rather than wait for an obvious breakdown.

An anomaly will be treated as an inspection indication, not a diagnosis or a prediction of remaining life. I’ll test against normal speed, load and background-noise variation, and compare the array with a single microphone to establish what the additional spatial information contributes.
```

4. Please share the link to the project you have started.  
   The project should include the beginning of your Bill of Materials (BOM) with the nRF54LM20 DK. 
```text
[Project Link](https://www.hackster.io/priyanshu-roy/acoustic-fault-detection-and-localization-0ce81d)
```
   Note: The application states that the hardware request cannot be judged without these items.

5. How does your solution work? What are the main features? How will you use Bluetooth and Edge AI on your project? Please specify how you will use the nRF54LM20 DK in your solution.

```text
I’ll use the nRF54LM20 DK as the acoustic monitoring node, targeting the nRF54LM20B and its Axon NPU. The DK will acquire synchronized audio from the Sipeed MAIX R6+1 seven-microphone array, extract features, run local inference, store event history and handle wireless communication. Audio capture and processing will run in timed windows, with the CPU waiting between cycles to reduce energy use.

For Edge AI, I’ll use a compact autoencoder trained on normal machine sounds. Training and quantization will run on a computer. On the node, the CPU will convert audio windows into log-mel features, and the Axon NPU will run the quantized model. The CPU will compare the reconstructed features with the input to calculate an anomaly score. This will flag departures from normal operation without requiring a labelled example of every possible fault or streaming audio to a server.

The microphone array will provide directional noise suppression and signal isolation as well as source-direction estimates. Delay-and-sum beamforming will emphasize the selected machine’s sound and reduce interference arriving from other directions, helping the model detect changes that could be masked in the mixed recording. Time-difference-of-arrival processing will estimate the source direction. Combining that spatial information with the anomaly score will give the operator a likely machine to inspect.

Zigbee will carry compact summaries and anomaly alerts through a XIAO ESP32C6 coordinator to a local monitoring application. Reports will include timestamps, anomaly scores and direction estimates. The application will display event history and report age, while the node will continue processing and logging if the application goes offline. The monitoring node will operate as a sleepy end device, collecting remote diagnostic requests when it polls its parent.

Bluetooth Low Energy will provide direct on-site maintenance access. The nRF node’s NFC tag will carry its identity and BLE commissioning information. A technician will read it using an ACS ACR1552U USB reader connected to Android. The companion application will then identify the node, establish an authenticated BLE connection and retrieve detailed diagnostic history independently of Zigbee. This gives the technician access beside the equipment even when the monitoring network is unavailable.
```

6. Tell us about yourself. What do you spend most of your time doing? What skills or experience do you have that will enable you to be successful in building this project?

```text
I work as a senior firmware engineer, mainly on IoT, robotics, and instrumentation systems. Most of my day involves writing firmware, debugging hardware, and getting different parts of a system to work together. That interest carries over into my free time as well. I enjoy designing PCBs, 3D modelling, experimenting with new development boards, and generally tinkering with electronics.

Over time, I've worked on everything from low-level drivers and sensor interfacing to real-time control and communication systems. I particularly enjoy the hands-on side of engineering, where I can take an idea, build something around it, and figure things out through testing and experimentation.

I was initially skeptical about how practical physical AI could be on small embedded devices. What caught my attention was Nordic's combination of low-power wireless connectivity, an NPU, and an affordable price point. That made me curious enough to experiment with it, and acoustic condition monitoring seemed like an interesting problem to start with.
```

## Part 2 — Mandatory declarations

7. I agree that if I am selected, I will submit a project to the Sustain the System contest by April 25, 2027 at 11:59 PM PDT.

   - Yes, I agree
   - No (Cannot submit if no)

8. If I am not able to submit for any reason, I will contact the contest admins at help@hackster.io by April 10, 2027 in order to arrange a possible hardware return.

   - Yes, I agree
   - No (Cannot submit if no)
