"""Export a CycloneDX 1.6 firmware source-package inventory and bundled notices.

ESP-IDF is represented as a bundle, not an assertion that every SDK/compiler
file has been individually licence- or vulnerability-audited. Host npm packages
are outside this firmware inventory and retain their own package-lock.json.
"""
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import uuid

import yaml


def generate(integration, project, build, out, identity):
    lock = json.loads((integration / "upstream.lock.json").read_text())
    ui = json.loads((integration / "ui/lvgl.lock.json").read_text())
    deps = yaml.safe_load((project / "dependencies.lock").read_text())["dependencies"]
    lvgl = project / "components/lvgl"
    notices = out / "licenses"
    notices.mkdir(exist_ok=True)
    sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
    license_files = {}

    def notice(name, source):
        target = notices / name
        shutil.copy2(source, target)
        license_files["licenses/"+name] = sha(target)

    notice("LVGL-MIT.txt", lvgl / "LICENCE.txt")
    notice("Montserrat-OFL.txt", lvgl / "scripts/built_in_font/font_license/Montserrat/OFL.txt")
    notice("FontAwesome-Free.txt", lvgl / "scripts/built_in_font/font_license/FontAwesome5/LICENSE.txt")
    notice("printf-MIT.txt", lvgl / "src/stdlib/builtin/LICENSE_SPRINTF.txt")
    tlsf = (lvgl / "src/stdlib/builtin/lv_tlsf.h").read_text()
    start = tlsf.index("/*")
    text = tlsf[start:tlsf.index("*/", start)+2]+"\n"
    (notices / "TLSF-BSD-3-Clause.txt").write_text(text)
    license_files["licenses/TLSF-BSD-3-Clause.txt"] = sha(notices / "TLSF-BSD-3-Clause.txt")
    notice("esp-coordinator-MIT.txt", project / "LICENSE")
    notice("ESP-IDF-Apache-2.0.txt", Path("/opt/esp/idf/LICENSE"))
    for package in ("esp-zboss-lib", "esp-zigbee-lib"):
        notice(package+"-LICENSE.txt", project / "managed_components" / ("espressif__"+package) / "LICENSE")

    def component(ref, name, version, source, license_id=None, license_name=None):
        reference_type = ("distribution" if source.startswith("https://components.espressif.com/")
                          else "license" if source.startswith("https://fontawesome.com/") else "vcs")
        item = {"type": "library", "bom-ref": ref, "name": name, "version": version,
                "externalReferences": [{"type": reference_type, "url": source}]}
        if license_id:
            item["licenses"] = [{"license": {"id": license_id}}]
        elif license_name:
            item["licenses"] = [{"license": {"name": license_name}}]
        return item

    components = [
        component("lvgl", "LVGL", ui["lvgl"]["version"],
                  "https://github.com/lvgl/lvgl/tree/"+ui["lvgl"]["revision"], "MIT"),
        component("idf", "ESP-IDF (SDK bundle)", "5.5.1", "https://github.com/espressif/esp-idf/tree/v5.5.1", "Apache-2.0"),
        component("ncp", "esp-coordinator", lock["version"], lock["fork"].removesuffix(".git")+"/tree/"+lock["revision"], "MIT"),
        component("zigbee", "esp-zigbee-lib", deps["espressif/esp-zigbee-lib"]["version"],
                  "https://components.espressif.com/components/espressif/esp-zigbee-lib", "Apache-2.0"),
        component("zboss", "esp-zboss-lib", deps["espressif/esp-zboss-lib"]["version"],
                  "https://components.espressif.com/components/espressif/esp-zboss-lib", license_name="Espressif restricted BSD-style ZBOSS licence; see packaged notice"),
        component("montserrat", "LVGL built-in Montserrat glyphs", "LVGL-"+ui["lvgl"]["version"]+"-subset",
                  "https://github.com/JulietaUla/Montserrat", "OFL-1.1"),
        component("fontawesome", "Font Awesome Free font glyphs", "LVGL-"+ui["lvgl"]["version"]+"-subset",
                  "https://fontawesome.com/license/free", "OFL-1.1"),
        component("tlsf", "TLSF allocator", "3.1", "https://github.com/mattconte/tlsf", "BSD-3-Clause"),
        component("printf", "mpaland printf (LVGL bundled copy)", "LVGL-"+ui["lvgl"]["version"], "https://github.com/mpaland/printf", "MIT")]
    components[0]["hashes"] = [{"alg": "SHA-256", "content": ui["lvgl"]["sha256"]}]
    components[0]["properties"] = [{"name": "raykestrel:hash-target", "value": "pinned LVGL source archive"}]
    components[2]["hashes"] = [{"alg": "SHA-256", "content": sha(integration / "build/source.tar")}]
    for ref, package in (("zigbee", "esp-zigbee-lib"), ("zboss", "esp-zboss-lib")):
        item = next(c for c in components if c["bom-ref"] == ref)
        item["properties"] = [{"name": "espressif:registry-component-hash", "value": deps["espressif/"+package]["component_hash"]}]
    logo_config = json.loads((integration / "ui/assets/splash.json").read_text())
    logo = integration / "ui/assets" / logo_config["image"]
    components.append({"type": "file", "bom-ref": "logo", "name": "RayKestrel logo",
                       "hashes": [{"alg": "SHA-256", "content": sha(logo)}],
                       "properties": [{"name": "raykestrel:provenance", "value": "User-supplied replaceable ui/assets PNG; no third-party licence assertion"}]})
    description = json.loads((build / "project_description.json").read_text())
    sdk_components = description.get("build_components", [])
    try:
        idf_revision = subprocess.check_output(["git", "-C", "/opt/esp/idf", "rev-parse", "HEAD"], text=True).strip()
    except subprocess.CalledProcessError:
        idf_revision = "not available in image"
    bom = {
        "bomFormat": "CycloneDX", "specVersion": "1.6", "version": 1,
        "serialNumber": "urn:uuid:"+str(uuid.uuid5(uuid.NAMESPACE_URL, "raykestrel:"+identity)),
        "metadata": {
            "timestamp": datetime.now(timezone.utc).isoformat(timespec="seconds").replace("+00:00", "Z"),
            "lifecycles": [{"phase": "build"}],
            "component": {"type": "firmware", "bom-ref": "firmware", "name": "RayKestrel Waveshare Zigbee coordinator",
                          "version": "source-"+identity[:16], "hashes": [{"alg": "SHA-256", "content": sha(out / "app.bin")}]},
            "tools": {"components": [{"type": "application", "name": "EEZ Studio", "version": ui["generator"]["version"],
                       "licenses": [{"license": {"id": "GPL-3.0-only"}}],
                       "properties": [{"name": "raykestrel:scope", "value": "Build-time generator only; EEZ Flow disabled; no EEZ runtime linked"}]},
                       {"type": "container", "name": "ESP-IDF toolchain image", "version": lock["idfImage"]}]},
            "properties": [
                {"name": "raykestrel:coverage", "value": "Firmware source-package inventory. ESP-IDF and compiler runtimes are bundle-level, not a per-file licence/security audit. Host npm/broker dependencies excluded; see host/package-lock.json and tool-versions.json."},
                {"name": "raykestrel:source-inputs-sha256", "value": identity},
                {"name": "raykestrel:idf-revision", "value": idf_revision},
                {"name": "raykestrel:configured-build-components", "value": json.dumps(sdk_components)},
                {"name": "raykestrel:dependency-lock-sha256", "value": sha(project / "dependencies.lock")},
                {"name": "raykestrel:generated-ui-provenance-sha256", "value": sha(integration / "ui/generated/generation.json")}]},
        "components": components,
        "dependencies": [
            {"ref": "firmware", "dependsOn": ["ncp", "idf", "lvgl", "zigbee", "zboss", "logo"]},
            {"ref": "lvgl", "dependsOn": ["montserrat", "fontawesome", "tlsf", "printf"]},
            {"ref": "ncp", "dependsOn": ["idf", "zigbee", "zboss"]},
            {"ref": "zigbee", "dependsOn": ["idf", "zboss"]}]}
    (out / "sbom.cdx.json").write_text(json.dumps(bom, indent=2)+"\n")
    return license_files
