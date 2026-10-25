"""Summarize captured test-node MQTT episodes, excluding duplicate cached records."""
import argparse
import datetime as dt
import json
from pathlib import Path
import statistics

parser = argparse.ArgumentParser()
parser.add_argument('mqtt_log', type=Path)
parser.add_argument('--output', type=Path)
args = parser.parse_args()
rows = [json.loads(line) for line in args.mqtt_log.read_text(encoding='utf-8').splitlines() if line]
episodes = []
duplicates = 0
for row in rows:
    value = row.get('payload')
    if not isinstance(value, dict) or 'sequence' not in value:
        continue
    sequence = value['sequence']
    assert value['schema_version'] == 1 and value['simulated'] is True
    assert value['node_role'] == 'sleepy_end_device'
    assert value['anomaly'] == (sequence % 10 == 0)
    assert value['anomaly_score'] == (0.9 if sequence % 10 == 0 else 0.1)
    assert value['direction'] == (sequence % 24) * 15
    if episodes:
        previous = episodes[-1][-1]['payload']
        if (sequence, value['uptime']) == (previous['sequence'], previous['uptime']):
            duplicates += 1
            continue
    if not episodes or sequence < episodes[-1][-1]['payload']['sequence']:
        episodes.append([])
    episodes[-1].append(row)

def timestamp(row):
    return dt.datetime.fromisoformat(row['time'].replace('Z', '+00:00')).timestamp()

result = {'episodes': [], 'duplicates_or_cached_republishes': duplicates, 'payload_validation': 'pass'}
for episode in episodes:
    gaps = []
    intervals = []
    for previous, current in zip(episode, episode[1:]):
        intervals.append(timestamp(current) - timestamp(previous))
        step = current['payload']['sequence'] - previous['payload']['sequence']
        if step != 1:
            gaps.append({'after': previous['payload']['sequence'], 'next': current['payload']['sequence']})
    result['episodes'].append({'reports': len(episode), 'first_utc': episode[0]['time'],
        'last_utc': episode[-1]['time'], 'duration_s': round(timestamp(episode[-1]) - timestamp(episode[0]), 3),
        'sequence_first': episode[0]['payload']['sequence'], 'sequence_last': episode[-1]['payload']['sequence'],
        'uptime_first_s': episode[0]['payload']['uptime'], 'uptime_last_s': episode[-1]['payload']['uptime'],
        'sequence_gaps': gaps, 'anomaly_reports': sum(row['payload']['anomaly'] for row in episode),
        'interval_s': {'min': round(min(intervals), 3), 'median': round(statistics.median(intervals), 3),
            'max': round(max(intervals), 3)} if intervals else None,
        'linkquality_min': min(row['payload']['linkquality'] for row in episode),
        'linkquality_max': max(row['payload']['linkquality'] for row in episode)})
text = json.dumps(result, indent=2) + '\n'
if args.output:
    args.output.write_text(text, encoding='utf-8')
print(text)
