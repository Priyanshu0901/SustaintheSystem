"""Reject stale or hand-edited EEZ output; normal builds do not need EEZ installed."""
import hashlib
import json
from pathlib import Path


def check(integration):
    ui = integration / "ui"
    generation = json.loads((ui / "generated/generation.json").read_text())
    lock = json.loads((ui / "lvgl.lock.json").read_text())
    sha = lambda path: hashlib.sha256(path.read_text(encoding="utf-8-sig").encode("utf-8")).hexdigest()
    if generation.get("hashNormalization") != "UTF-8 text with LF":
        raise ValueError("Regenerate UI provenance with canonical text hashes")
    if generation["projectSha256"] != sha(ui / "editor/coordinator.eez-project"):
        raise ValueError("EEZ source changed: run ui/Generate-UI.ps1")
    if generation["version"] != lock["generator"]["version"] or generation["flowSupport"]:
        raise ValueError("Unreviewed EEZ generator configuration")
    if generation["firmwareApiVersion"] != lock["lvgl"]["version"]:
        raise ValueError("EEZ output is tied to a different LVGL release")
    actual = {p.name for p in (ui / "generated").iterdir() if p.suffix in (".c", ".h")}
    if actual != set(generation["files"]):
        raise ValueError("EEZ generated file set differs from generation.json")
    for name, digest in generation["files"].items():
        if sha(ui / "generated" / name) != digest:
            raise ValueError(f"Generated file changed: {name}; regenerate in EEZ")


if __name__ == "__main__":
    check(Path(__file__).resolve().parents[1])
    print("PASS: generated EEZ source/output/version provenance")
