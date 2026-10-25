"""Check the effective role/partition limits and save a hash manifest."""
import hashlib
import json
from pathlib import Path
import re
import sys


def check(build, board, role):
    app = Path(__file__).resolve().parent.parent
    output = build / 'zephyr'
    config = dict(re.findall(r'^(CONFIG_\w+)=(.*)$', (output / '.config').read_text(), re.M))
    selected = 'CONFIG_ZIGBEE_ROLE_ROUTER' if role == 'router' else 'CONFIG_ZIGBEE_ROLE_END_DEVICE'
    assert config[selected] == 'y'
    assert config.get('CONFIG_SUSTAIN_SLEEPY', 'n') == ('y' if role == 'sleepy' else 'n')
    assert config.get('CONFIG_SUSTAIN_STATUS_RGB', 'n') == ('y' if board == 'xiao' else 'n')
    assert config['CONFIG_USE_DT_CODE_PARTITION'] == 'y'
    if config.get('CONFIG_FLASH_USES_MAPPED_PARTITION') != 'y':
        assert int(config['CONFIG_FLASH_LOAD_OFFSET'], 0) == 0
        assert int(config['CONFIG_FLASH_LOAD_SIZE'], 0) == 1992 * 1024
    dts = (output / 'zephyr.dts').read_text()
    expected = {'slot0_partition': (0, 1992 * 1024),
                'storage_partition': (0x1f2000, 8 * 1024),
                'zboss_nvram': (0x1f4000, 32 * 1024),
                'zboss_product_config': (0x1fc000, 4 * 1024)}
    for name, bounds in expected.items():
        match = re.search(rf'{name}: partition@\w+\s*\{{.*?reg = <\s*(0x[\da-f]+)\s+(0x[\da-f]+)\s*>;', dts, re.S)
        assert match and tuple(int(v, 16) for v in match.groups()) == bounds, name
    assert 'slot0_ns_partition:' not in dts and 'slot1_ns_partition:' not in dts
    # Validate data-bearing Intel HEX records do not occupy persistent storage.
    base = 0
    for line in (output / 'zephyr.hex').read_text().splitlines():
        record = bytes.fromhex(line[1:])
        assert sum(record) % 256 == 0
        size, kind = record[0], record[3]
        if kind == 4:
            base = int.from_bytes(record[4:6], 'big') << 16
        elif kind == 0:
            address = base + int.from_bytes(record[1:3], 'big')
            assert address + size <= 1992 * 1024, hex(address)
    sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
    sources = [app / n for n in ['CMakeLists.txt', 'Kconfig', 'prj.conf',
        'build-lock.json', 'dependencies.lock.yml', 'Build-Native.ps1', 'Build-Docker.ps1', 'Flash-Native.ps1']]
    for folder in ['src', 'include', 'boards', 'profiles', 'tools', 'host', 'tests']:
        sources.extend(p for p in (app / folder).rglob('*') if p.is_file() and '__pycache__' not in p.parts)
    manifest = {'board': board, 'role': role, 'checks': 'role, profile, linker/DT partitions, HEX bounds/checksums',
        'source_sha256': {p.relative_to(app).as_posix(): sha(p) for p in sorted(sources)},
        'output_sha256': {n: sha(output / n) for n in ['zephyr.hex', 'zephyr.elf', 'zephyr.bin', '.config', 'zephyr.dts']}}
    (build / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(f'{board}/{role}: effective role, partition and HEX checks PASS', flush=True)


if __name__ == '__main__':
    check(Path(sys.argv[1]), sys.argv[2], sys.argv[3])
