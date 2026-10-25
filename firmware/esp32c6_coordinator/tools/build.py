"""Container entry point: build an immutable source archive with isolated profiles."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(*args, **kwargs):
    subprocess.run(args, check=True, **kwargs)


def main():
    profile = sys.argv[1]
    if profile not in {"xiao-ext-usb", "generic-usb"}:
        raise SystemExit("Unknown board profile")
    resolve = "--resolve-dependencies" in sys.argv[2:]
    integration, work, out = Path("/integration"), Path("/work"), Path("/out")
    lock = json.loads((integration / "upstream.lock.json").read_text())
    dep_lock = integration / "dependencies.lock"
    if not dep_lock.exists() and not resolve:
        raise SystemExit("Dependency lock missing; maintainer resolution required")
    # Remove the previous success marker before any work; failures cannot leave a flashable manifest.
    (out / "manifest.json").unlink(missing_ok=True)
    archive = integration / "build/source.tar"
    inputs = [archive, integration / "profiles/usb.defaults",
              integration / f"profiles/{profile}.defaults", Path(__file__)]
    if dep_lock.exists():
        inputs.append(dep_lock)
    identity = hashlib.sha256(json.dumps(lock, sort_keys=True).encode() +
                              "".join(sha(p) for p in inputs).encode()).hexdigest()
    marker = work / "inputs.sha256"
    if not marker.exists() or marker.read_text() != identity:
        for name in ("project", "build"):
            path = work / name
            if path.exists():
                shutil.rmtree(path)
        (work / "sdkconfig").unlink(missing_ok=True)
        project = work / "project"
        project.mkdir()
        with tarfile.open(archive) as bundle:
            # This tar is produced by git archive from the pinned fork revision.
            bundle.extractall(project, filter="data")
        if dep_lock.exists():
            shutil.copy2(dep_lock, project / "dependencies.lock")
        marker.write_text(identity)
    project, build = work / "project", work / "build"
    env = os.environ.copy()
    env["IDF_TARGET"] = lock["target"]
    defaults = ";".join([str(project / "sdkconfig.defaults"),
                         str(integration / "profiles/usb.defaults"),
                         str(integration / f"profiles/{profile}.defaults")])
    run("idf.py", "-B", str(build), "-D", "SDKCONFIG=/work/sdkconfig",
        "-D", f"SDKCONFIG_DEFAULTS={defaults}", "build", cwd=project, env=env)
    actual_lock = project / "dependencies.lock"
    if dep_lock.exists() and sha(dep_lock) != sha(actual_lock):
        raise SystemExit("Build changed dependencies.lock; review a separate dependency update")
    config = (work / "sdkconfig").read_text()
    expected = ["CONFIG_NCP_BUS_MODE_USB=y", "# CONFIG_NCP_UART_TRANSPORT is not set",
                "CONFIG_ESP_CONSOLE_SECONDARY_NONE=y", 'CONFIG_ESPTOOLPY_FLASHSIZE="4MB"',
                "CONFIG_NCP_XIAO_EXT_ANTENNA=y" if profile == "xiao-ext-usb"
                else "# CONFIG_NCP_XIAO_EXT_ANTENNA is not set"]
    for setting in expected:
        if setting not in config.splitlines():
            raise SystemExit(f"Profile validation failed: {setting}")
    flash = json.loads((build / "flasher_args.json").read_text())
    args = []
    for offset, name in flash["flash_files"].items():
        args.extend([offset, str(build / name)])
    settings = flash["flash_settings"]
    run(sys.executable, "-m", "esptool", "--chip", lock["target"], "merge_bin",
        "--output", str(out / "factory.bin"), "--flash_mode", settings["flash_mode"],
        "--flash_freq", settings["flash_freq"], "--flash_size", settings["flash_size"], *args)
    for src, name in [(actual_lock, "dependencies.lock"), (work / "sdkconfig", "sdkconfig"),
                      (build / "esp-coordinator.bin", "app.bin"),
                      (build / "flasher_args.json", "flasher_args.json")]:
        shutil.copy2(src, out / name)
    manifest = {**lock, "profile": profile, "inputsSha256": identity,
                "sourceArchiveSha256": sha(archive), "flashOffset": "0x0",
                "flashSizeBytes": 4194304, "dependencyResolutionOnly": resolve,
                "files": {p.name: sha(p) for p in [out / "factory.bin", out / "app.bin",
                                                   out / "sdkconfig", out / "dependencies.lock"]}}
    (out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"PASS: {profile}; SHA256 {manifest['files']['factory.bin']}")


if __name__ == "__main__":
    main()
