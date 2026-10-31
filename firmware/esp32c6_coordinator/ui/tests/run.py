"""Compile portable UI checks and optionally export same-renderer PPM previews."""
import argparse
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser()
parser.add_argument("--output", type=Path, required=True)
parser.add_argument("--previews", action="store_true")
args = parser.parse_args()
ui = Path(__file__).resolve().parents[1]
output = args.output.resolve()
output.mkdir(parents=True, exist_ok=True)
subprocess.run([sys.executable, str(ui / "generate_splash.py"),
                str(ui / "assets/splash.json"), str(output / "ui_splash_asset.h")], check=True)
binary = output / "ui-test"
subprocess.run(["gcc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-g",
                "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                f"-I{ui}", f"-I{output}", str(ui / "ui_model.c"),
                str(ui / "ui_geometry.c"), str(ui / "ui_view.c"),
                str(ui / "tests/model_and_preview.c"), "-o", str(binary)], check=True)
subprocess.run([str(binary)], check=True)
if args.previews:
    for page in ("network", "waiting", "menu", "debug", "settings", "splash"):
        for rotation in (0, 1):
            with (output / f"{page}-{rotation}.ppm").open("wb") as stream:
                subprocess.run([str(binary), page, str(rotation)], stdout=stream, check=True)
