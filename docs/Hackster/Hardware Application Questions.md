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

The monitoring node will process audio locally, detect departures from normal operating sound and estimate the direction of the abnormal source. Zigbee will carry compact status reports and maintenance alerts through a XIAO ESP32C6 coordinator to a local monitoring application. NFC-assisted Bluetooth Low Energy access will let a technician retrieve diagnostic history directly from a node.

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
Will be added
```
   Note: The application states that the hardware request cannot be judged without these items.

5. How does your solution work? What are the main features? How will you use Bluetooth and Edge AI on your project? Please specify how you will use the nRF54LM20 DK in your solution.

```text
The nRF54LM20 DK will be the monitoring node’s main processing and communication platform, targeting the nRF54LM20B and its Axon NPU. The firmware will handle synchronized audio input, feature extraction, observation storage and wireless communication, with compatible quantized model inference running on the NPU.

I’ll use spectral analysis and log-mel features with compact autoencoders as the initial anomaly-detection baseline. Training and quantization will run on a computer using MIMII/DCASE machinery recordings and recordings from the physical setup. Separate data will be used for training, threshold selection and evaluation. On the device, I’ll measure detection performance, false alarms, model size, RAM and processing latency.

The seven-channel microphone array will supply spatial information. I’ll compare beamforming before anomaly detection with detection in mixed audio followed by spatial analysis, using delay-and-sum beamforming and TDOA/GCC-PHAT methods. Channel alignment and sustained capture will be checked before evaluating direction estimates. The output will be a likely source direction or machine association.

Zigbee Sleepy End Device operation will carry summaries, anomaly scores, timestamps and direction estimates to the XIAO ESP32C6 coordinator. The local monitoring application will retain reports, display their age and flag nodes that stop reporting. It will also send small diagnostic requests through the network.

Bluetooth will provide direct maintenance access. An NFC tap will enable a temporary BLE advertising window, and an authenticated companion application will retrieve the node’s event history and diagnostic information independently of Zigbee. I’ll test this interaction while acquisition and inference are active.

Physical validation will use two simultaneous low-voltage fans or motors, with a worn fan or a repeatable mounting change providing the abnormal test case. Measurements will cover detection, correct-machine association, direction error, report delivery, diagnostic latency and reconnection. Power testing will separate sensing, processing, inference, radio and idle consumption, and compare continuous monitoring with periodic sampling and event-driven inference.
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
