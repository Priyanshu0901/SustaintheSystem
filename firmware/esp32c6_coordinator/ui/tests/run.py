"""Build sanitized native tests/screenshots against the same locked LVGL source."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import zipfile

parser=argparse.ArgumentParser()
parser.add_argument("--output",type=Path,required=True)
parser.add_argument("--previews",action="store_true")
args=parser.parse_args()
ui=Path(__file__).resolve().parents[1]
lock=json.loads((ui/"lvgl.lock.json").read_text())["lvgl"]
archive=ui.parent/"build/dependencies"/lock["archive"]
if hashlib.sha256(archive.read_bytes()).hexdigest()!=lock["sha256"]:
    raise SystemExit("LVGL archive hash mismatch")
output=args.output.resolve();output.mkdir(parents=True,exist_ok=True)
source=output/"dependency"
if not (source/lock["root"]/"CMakeLists.txt").exists():
    with zipfile.ZipFile(archive) as bundle:
        bundle.extractall(source)
build=output/"native"
subprocess.run(["cmake","-S",str(ui/"tests"),"-B",str(build),"-G","Ninja",
    f"-DLVGL_PATH={source/lock['root']}","-DCMAKE_BUILD_TYPE=Debug"],check=True)
subprocess.run(["cmake","--build",str(build),"-j","8"],check=True)
binary=build/"ui-test"
subprocess.run([str(binary)],check=True)
subprocess.run([str(build/"ui-test-no-debug")],check=True)
if args.previews:
    for page in ("network","waiting","menu","settings","rotation","timeout","help","saved","debug","about","splash"):
        for rotation in (0,1):
            with (output/f"{page}-{rotation}.ppm").open("wb") as stream:
                subprocess.run([str(binary),page,str(rotation)],stdout=stream,check=True)

if args.previews:
    for rotation in (0,1):
        with (output/f"menu-no-debug-{rotation}.ppm").open("wb") as stream:
            subprocess.run([str(build/"ui-test-no-debug"),"menu",str(rotation)],stdout=stream,check=True)
