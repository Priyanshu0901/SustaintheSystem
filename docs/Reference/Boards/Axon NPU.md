# Axon NPU and DSP capabilities

Reference check: **2026-09-26**; targeted follow-ups retain their dates below.

Axon accelerates supported neural-network inference. Nordic also describes Neuton models executing on the CPU; **LM20A is not restricted to DSP-only processing**. The earlier proposed A/B division was an architecture suggestion, not a chip limitation. [Nordic Edge AI overview](https://www.nordicsemi.com/Products/Technologies/Edge-AI/Axon-NPU).

| Software capability | Documented path |
| --- | --- |
| Neural inference | Compile model, integrate generated data/code and invoke Axon driver |
| Model inputs | int8 `.tflite`/`.lite`; local compiler also documents Keras `.h5` with quantization data |
| Compilation | Nordic Edge AI Lab or local compiler |
| Validation | Compiler accuracy checks/test vectors; on-device inference comparison |
| Compatibility | Model operators, shapes and buffer requirements must satisfy compiler/runtime support |

The compiler documents a model-support scanner. A TensorFlow Lite file is not automatically supported merely because it fits storage. Neither source establishes a general on-device model-training capability. Sources: [Axon compiler](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/axon_compiler/Axon_npu_tflite_compiler.html), [driver integration](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/quick_start/axon_driver_inference.html).

Axon additionally exposes **DSP intrinsics**, including vector arithmetic, FFT/power spectrum, FIR filtering, mathematical and 2D operations. These can be called without compiling a neural model. Sources: [DSP API workflow](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/quick_start/axon_driver_intrinsics.html), [documented intrinsic tests](https://nrfconnectdocs.nordicsemi.com/addons/addon-edge-ai/latest/tests/axon/intrinsics/README.html).

For performance context only, Nordic reports keyword-spotting inference at **4.5 ms / 40.5 µJ**, and its anomaly-detection benchmark at **1.3 ms / 13.7 µJ**, with Axon at 128 MHz and current measured at 3 V. These specific benchmark results do not establish our model's latency, accuracy or energy. [Vendor benchmarks](https://www.nordicsemi.com/Products/Technologies/Edge-AI/Axon-NPU).

[DK hardware](Nordic%20DK.md)
