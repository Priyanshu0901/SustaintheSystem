"""Container entry point for the active Waveshare coordinator revision."""
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
    resolve = "--resolve-dependencies" in sys.argv[1:]
    integration, work, out = Path("/integration"), Path("/work"), Path("/out")
    lock = json.loads((integration / "upstream.lock.json").read_text())
    dep_lock = integration / "dependencies.lock"
    if not dep_lock.exists() and not resolve:
        raise SystemExit("Dependency lock missing; maintainer resolution required")
    # Remove the previous success marker before any work; failures cannot leave a flashable manifest.
    (out / "manifest.json").unlink(missing_ok=True)
    archive = integration / "build/source.tar"
    inputs = [archive, integration / "sdkconfig.defaults", Path(__file__)]
    local_files = []
    for folder in ("ui", "boards/waveshare_touch_1_9", "patches"):
        local_files.extend(sorted(p for p in (integration / folder).rglob("*")
                                  if p.is_file() and "tests" not in p.parts
                                  and p.suffix in {".c", ".h", ".txt", ".patch"}))
    inputs.extend(local_files)
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
        shutil.copytree(integration / "ui", project / "components/coordinator_ui",
                        ignore=shutil.ignore_patterns("tests", "README.md"))
        shutil.copytree(integration / "boards/waveshare_touch_1_9",
                        project / "components/waveshare_display")
        # Keep upstream immutable. Only the build copy receives Waveshare board
        # initialization and the bounded UI hook; no alternative board paths.
        for name in ("main/main.cpp", "main/zb_ncp.cpp"):
            path = project / name
            path.write_bytes(path.read_bytes().replace(b"\r\n", b"\n"))
        patch = str(integration / "patches/waveshare-ui.patch")
        run("git", "apply", "--check", patch, cwd=project)
        run("git", "apply", patch, cwd=project)
        if dep_lock.exists():
            shutil.copy2(dep_lock, project / "dependencies.lock")
        marker.write_text(identity)
    project, build = work / "project", work / "build"
    env = os.environ.copy()
    env["IDF_TARGET"] = lock["target"]
    defaults = ";".join([str(project / "sdkconfig.defaults"),
                         str(integration / "sdkconfig.defaults")])
    run("idf.py", "-B", str(build), "-D", "SDKCONFIG=/work/sdkconfig",
        "-D", f"SDKCONFIG_DEFAULTS={defaults}", "build", cwd=project, env=env)
    actual_lock = project / "dependencies.lock"
    if dep_lock.exists() and sha(dep_lock) != sha(actual_lock):
        raise SystemExit("Build changed dependencies.lock; review a separate dependency update")
    config = (work / "sdkconfig").read_text()
    flash_mb = 8
    expected = ["CONFIG_NCP_BUS_MODE_USB=y", "# CONFIG_NCP_UART_TRANSPORT is not set",
                "CONFIG_ESP_CONSOLE_SECONDARY_NONE=y", 'CONFIG_ESPTOOLPY_FLASHSIZE="8MB"']
    for setting in expected:
        if setting not in config.splitlines():
            raise SystemExit(f"Waveshare configuration validation failed: {setting}")
    if "select_external_antenna" in (project / "main/main.cpp").read_text():
        raise SystemExit("Historical XIAO antenna path remains in the active build")
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
    manifest = {**lock, "board": "waveshare-touch-1.9", "inputsSha256": identity,
                "sourceArchiveSha256": sha(archive), "flashOffset": "0x0",
                "applicationOffset": flash["app"]["offset"],
                "flashSizeBytes": flash_mb * 1024 * 1024, "dependencyResolutionOnly": resolve,
                "localSourceFiles": {str(p.relative_to(integration)): sha(p) for p in local_files},
                "files": {p.name: sha(p) for p in [out / "factory.bin", out / "app.bin",
                                                   out / "sdkconfig", out / "dependencies.lock",
                                                   out / "flasher_args.json"]}}
    (out / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"PASS: Waveshare; SHA256 {manifest['files']['factory.bin']}")


if __name__ == "__main__":
    main()
