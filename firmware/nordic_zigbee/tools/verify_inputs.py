"""Validate checkout revisions; check integration repositories for tracked edits."""
import json
from pathlib import Path
import subprocess
import sys
import yaml


def git(path, *args):
    return subprocess.check_output(['git', '-C', str(path), *args], text=True).strip()


def verify(sdk, seeed, zigbee):
    app = Path(__file__).resolve().parent.parent
    manifest = yaml.safe_load((app / 'dependencies.lock.yml').read_text())['manifest']
    for project in manifest['projects']:
        path = sdk / project.get('path', project['name'])
        if project['name'] == 'seeed':
            path = seeed.parent
        elif project['name'] == 'zigbee':
            path = zigbee
        expected = project['revision']
        if git(path, 'rev-parse', 'HEAD') != expected:
            raise RuntimeError(f'{path}: revision differs from dependencies.lock.yml')
        # Full installed SDK worktree scans can be very slow on Windows.
        # Container SDKs are freshly checked out; native SDK cleanliness remains
        # an explicit caller prerequisite rather than an inferred guarantee.
        if project['name'] in ('zigbee', 'seeed') and git(path, 'status', '--porcelain', '--untracked-files=no'):
            raise RuntimeError(f'{path}: tracked inputs are modified')
    print('Pinned revisions and Zigbee/Seeed tracked-input cleanliness: PASS', flush=True)


if __name__ == '__main__':
    verify(*(Path(arg) for arg in sys.argv[1:]))
