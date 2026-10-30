# Coordinator upstream choice and build design

Reviewed 2026-10-25. Keep [tostmann/esp-coordinator](https://github.com/tostmann/esp-coordinator) as the maintained upstream for this **USB ZBOSS NCP evaluation**, using [our fork](https://github.com/Priyanshu0901/esp-coordinator). The integration operations guide and lock file own the selected revision and license provenance.

| Candidate | Evidence | Fit for this project |
| --- | --- | --- |
| [andryblack/esp-coordinator](https://github.com/andryblack/esp-coordinator) | Archived 2026-04-02; README identifies it as a proof of concept and points to tostmann | Preserve ancestry; do not base new work on its frozen implementation |
| [tostmann/esp-coordinator](https://github.com/tostmann/esp-coordinator) | Recent source fixes and release evidence; explicit XIAO antenna configuration; named by Zigbee2MQTT's adapter guide | Selected for the existing host protocol; validate locally before relying on it |
| Other recent forks | BragiOk/master matched selected master; aliciusschroeder/master was nine commits behind during comparison | No stronger parent identified in these comparisons; this is not an exhaustive firmware audit |
| [Espressif ESP-Zigbee-SDK](https://github.com/espressif/esp-zigbee-sdk) | Official SDK; v1.x is ZBOSS maintenance/LTS, v2.x uses Espressif's stack and is recommended upstream for new designs | A different stack generation; SDK maintenance alone does not establish a compatible Zigbee2MQTT ZBOSS serial endpoint |

[Zigbee2MQTT still labels ZBOSS experimental](https://www.zigbee2mqtt.io/guide/adapters/zboss.html), listing missing install-code support, backups and Inter-PAN; channel changes require re-pairing. The firmware's README describes extra backup capabilities through its maintainer's patched host fork. That does not establish support in the standard Zigbee2MQTT host used here. Keep the lab setup on a fixed channel and treat host parameter changes and recovery as separate validation work. Do not adopt claims about production reliability, node capacity or RF range from marketing text.

## Build isolation

Docker supplies ESP-IDF and its compiler/Python dependencies. The build extracts a `git archive` of the pinned revision into a profile-specific Docker volume, then applies configuration overlays and the reviewed component lock. Windows does not need a native ESP-IDF installation. Source is mounted read-only, while outputs and compiler caches have explicit locations. Waveshare copies project-owned UI/board sources into that build copy and applies its isolated hook patch; their hashes enter the manifest. XIAO/generic receive neither component nor patch. A Linux CI runner uses the same PowerShell entry point and image. Pinning dependencies makes rebuilding controlled; byte-for-byte reproducibility is a separate test and is not assumed.

Each successful build exports a merged factory image, application image, effective `sdkconfig`, component lock, flash arguments and a manifest with SHA-256 hashes. A failed container build removes its success manifest, preventing the development flasher from using that build. A dependency-resolution build is marked unsuitable for flashing until its generated lock is reviewed and a normal locked build passes.

[Exact inputs, licenses and build/update commands](https://github.com/Priyanshu0901/SustaintheSystem/tree/master/firmware/esp32c6_coordinator)
