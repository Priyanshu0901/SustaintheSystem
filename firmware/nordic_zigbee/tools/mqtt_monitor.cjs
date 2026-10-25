// Capture only bench device telemetry and selected bridge status; never configuration/keys.
const {createRequire} = require('node:module');
const path = require('node:path');
const fs = require('node:fs');
const hostRequire = createRequire(path.resolve(__dirname, '../../esp32c6_coordinator/host/package.json'));
const mqtt = hostRequire('mqtt');
const [output, stopFile, seconds = '1200'] = process.argv.slice(2);
if (!output || !stopFile || !Number.isFinite(Number(seconds))) throw new Error('output, stop-file, seconds required');
const base = 'sustain/zigbee';
const client = mqtt.connect('mqtt://127.0.0.1:1883', {reconnectPeriod: 1000});
const log = fs.createWriteStream(output, {flags: 'a'});
let count = 0;
let ending = false;
client.on('connect', () => client.subscribe([
    `${base}/+`, `${base}/bridge/state`, `${base}/bridge/event`,
    `${base}/bridge/devices`, `${base}/bridge/response/permit_join`,
], {qos: 1}, error => {
    if (error) throw error;
    console.log('MQTT monitor subscribed');
}));
client.on('message', (topic, bytes, packet) => {
    let payload;
    try { payload = JSON.parse(bytes.toString()); } catch { payload = bytes.toString(); }
    log.write(`${JSON.stringify({time: new Date().toISOString(), topic, retained: packet.retain, payload})}\n`);
    if (typeof payload?.sequence === 'number' && (++count === 1 || count % 20 === 0)) console.log(`Decoded device reports: ${count}`);
});
client.on('error', error => console.error(error.message));
const end = () => {
    if (ending) return;
    ending = true;
    clearInterval(poll);
    clearTimeout(deadline);
    client.end(false, () => log.end(() => console.log(`MQTT monitor stopped; decoded reports=${count}`)));
};
const poll = setInterval(() => { if (fs.existsSync(stopFile)) end(); }, 500);
const deadline = setTimeout(end, Number(seconds) * 1000);
process.on('SIGINT', end);
