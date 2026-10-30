# Nordic implemented board and role profiles

Board and role choices are separate build inputs. Their authored files describe configuration; hardware qualification is recorded in the operations guide.

| Existing files | Responsibility |
| --- | --- |
| `profiles/xiao.conf` and XIAO device-tree overlay | Enable PWM status using the board's active-low RGB aliases: red P1.22, blue P1.23, green P1.24 |
| `profiles/dk.conf` and DK A/B overlays | Disable XIAO RGB support and select the corresponding DK configuration |
| `profiles/router.conf` | Select Zigbee router role; disable sleepy behavior |
| `profiles/sleepy.conf` | Select Zigbee end-device role; enable sleepy behavior |

`src/board_status.c` consumes the PWM aliases behind `CONFIG_SUSTAIN_STATUS_RGB`; transport and observation code contain no board pin assignments. No new antenna-switch behavior is implemented here.

One XIAO sleepy image has measured single-node validation. Router and DK profiles existing in source do not establish routed-network or DK hardware results. [Firmware operations](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/nordic_zigbee) owns SDK locks, build targets, direct-programming memory layout and actual validation.

[Architecture](Architecture.md) · [Code reference](Code%20Reference.md)
