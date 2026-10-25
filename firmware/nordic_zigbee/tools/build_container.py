"""Run only inside the pinned Linux toolchain, with /app read-only and /work volume."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys


def run(*args, cwd=None):
    subprocess.run(args, cwd=cwd, check=True)


board, role = sys.argv[1:]
targets = {"xiao": "xiao_nrf54lm20a/nrf54lm20a/cpuapp",
           "dk-a": "nrf54lm20dk/nrf54lm20a/cpuapp",
           "dk-b": "nrf54lm20dk/nrf54lm20b/cpuapp"}
assert board in targets and role in ("router", "sleepy")
work = Path('/work')
manifest = work / 'manifest'
manifest.mkdir(parents=True, exist_ok=True)
shutil.copyfile('/app/dependencies.lock.yml', manifest / 'west.yml')
if not (work / '.west').exists():
    run('west', 'init', '-l', str(manifest), cwd=work)
# Avoid shell interpolation. All projects have immutable revisions in the frozen manifest.
run('west', 'update', '--narrow', '--fetch-opt=--depth=1', cwd=work)
os.environ['ZEPHYR_BASE'] = str(work / 'zephyr')
sys.path.insert(0, '/app/tools')
from verify_inputs import verify
verify(work, work / 'seeed/zephyr', work / 'zigbee')
build = work / 'build' / f'{board}-{role}'
board_profile = 'xiao' if board == 'xiao' else 'dk'
args = ['west', 'build', '-p', 'always', '--no-sysbuild', '--build-opt=-j4',
        '-b', targets[board], '-d', str(build), '/app', '--', '-DUSE_CCACHE=OFF',
        f'-DEXTRA_CONF_FILE=/app/profiles/{role}.conf;/app/profiles/{board_profile}.conf']
if board == 'xiao':
    args.append(f'-DBOARD_ROOT={work}/seeed/zephyr')
run(*args, cwd=work)
from check_build import check
check(build, board, role)
out = Path('/out')
out.mkdir(parents=True, exist_ok=True)
hashes = {}
for name in ['zephyr.hex', 'zephyr.elf', 'zephyr.bin', '.config', 'zephyr.dts']:
    source = build / 'zephyr' / name
    shutil.copyfile(source, out / name)
    hashes[name] = hashlib.sha256(source.read_bytes()).hexdigest()
(out / 'manifest.json').write_text(json.dumps({'board': board, 'role': role,
    'build_lock': json.loads(Path('/app/build-lock.json').read_text()),
    'sha256': hashes}, indent=2) + '\n')
shutil.copyfile(build / 'manifest.json', out / 'build-evidence.json')
