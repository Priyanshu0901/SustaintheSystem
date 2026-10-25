// Zigbee2MQTT 2.14.2 / zigbee-herdsman-converters 26.115.1.
const {Zcl} = require('zigbee-herdsman');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const m = require('zigbee-herdsman-converters/lib/modernExtend');
const e = exposes.presets;
const ea = exposes.access;

function decodeObservation(value) {
    if (!Buffer.isBuffer(value) && !(value instanceof Uint8Array) && !Array.isArray(value)) return;
    const bytes = Buffer.from(value);
    // Herdsman strips the ZCL octet-string length byte.
    if (bytes.length !== 16 || bytes[0] !== 1 || bytes[15] !== 0 || (bytes[1] & ~3) !== 0) return;
    const score = bytes.readUInt16LE(10);
    const direction = bytes.readUInt16LE(12);
    if (score > 1000 || direction >= 360 || ![1, 2].includes(bytes[14])) return;
    return {
        schema_version: bytes[0],
        simulated: Boolean(bytes[1] & 1),
        anomaly: Boolean(bytes[1] & 2),
        sequence: bytes.readUInt32LE(2),
        uptime: bytes.readUInt32LE(6),
        anomaly_score: score / 1000,
        direction: direction,
        node_role: bytes[14] === 1 ? 'router' : 'sleepy_end_device',
    };
}

const definition = {
    zigbeeModel: ['SustainNode-v1'],
    model: 'SustainNode-v1',
    vendor: 'Sustain',
    description: 'Simulated acoustic observation transport test node',
    extend: [m.deviceAddCustomCluster('sustainObservation', {
        name: 'sustainObservation', ID: 0xfc00,
        attributes: {observation: {name: 'observation', ID: 0x0000, type: Zcl.DataType.OCTET_STR}},
        commands: {}, commandsResponse: {},
    })],
    fromZigbee: [{
        cluster: 'sustainObservation', type: ['attributeReport', 'readResponse'],
        convert: (_model, msg) => decodeObservation(msg.data.observation ?? msg.data[0]),
    }],
    toZigbee: [],
    exposes: [
        e.numeric('schema_version', ea.STATE),
        e.binary('simulated', ea.STATE, true, false),
        e.binary('anomaly', ea.STATE, true, false),
        e.numeric('sequence', ea.STATE),
        e.numeric('uptime', ea.STATE).withUnit('s'),
        e.numeric('anomaly_score', ea.STATE).withValueMin(0).withValueMax(1),
        e.numeric('direction', ea.STATE).withUnit('°'),
        e.enum('node_role', ea.STATE, ['router', 'sleepy_end_device']),
    ],
};

module.exports = definition;
