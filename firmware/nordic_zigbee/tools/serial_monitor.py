"""Timestamp a selected development UART into a local JSONL log."""
import argparse
import datetime
import json
from pathlib import Path
import time
import serial

parser = argparse.ArgumentParser()
parser.add_argument('--port', required=True)
parser.add_argument('--output', type=Path, required=True)
parser.add_argument('--seconds', type=int, default=1200)
parser.add_argument('--stop-file', type=Path, required=True)
args = parser.parse_args()
deadline = time.monotonic() + args.seconds
with serial.Serial(args.port, 115200, timeout=0.5) as uart, args.output.open('a', encoding='utf-8') as log:
    print(f'UART monitor ready: {args.port}', flush=True)
    while time.monotonic() < deadline and not args.stop_file.exists():
        data = uart.read_until(b'\n')
        if data:
            log.write(json.dumps({'time': datetime.datetime.now(datetime.timezone.utc).isoformat(),
                                  'text': data.decode('utf-8', errors='replace').rstrip()}) + '\n')
            log.flush()
