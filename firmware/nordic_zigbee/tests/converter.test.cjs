const assert = require('node:assert/strict');
const {createRequire} = require('node:module');
const path = require('node:path');
// Resolve pinned coordinator dependencies without installing another package set.
const hostRequire = createRequire(path.resolve(__dirname, '../../esp32c6_coordinator/host/package.json'));
const fs = require('node:fs');
const Module = require('node:module');
const filename = path.resolve(__dirname, '../host/sustain-node.cjs');
const instance = new Module(filename);
instance.filename = filename;
instance.require = hostRequire;
instance._compile(fs.readFileSync(filename, 'utf8'), filename);
const definition = instance.exports;
const convert = data => definition.fromZigbee[0].convert(null, {data});

// Golden v1: simulated anomaly, seq=0x12345678, uptime=60, score=.9,
// direction=345°, SED. The reserved byte is zero.
const golden = Buffer.from('0103785634123c000000840359010200', 'hex');
assert.deepEqual(convert({observation: golden}), {
    schema_version: 1, simulated: true, anomaly: true, sequence: 0x12345678,
    uptime: 60, anomaly_score: 0.9, direction: 345, node_role: 'sleepy_end_device',
});
assert.equal(convert({0: Array.from(golden)}).sequence, 0x12345678);
assert.equal(convert({observation: new Uint8Array(golden)}).sequence, 0x12345678);
for (const value of [null, 'not bytes', golden.subarray(1), Buffer.concat([golden, Buffer.from([0])])]) {
    assert.equal(convert({observation: value}), undefined);
}
for (const [offset, value] of [[0, 2], [1, 4], [14, 0], [15, 1]]) {
    const invalid = Buffer.from(golden); invalid[offset] = value;
    assert.equal(convert({observation: invalid}), undefined);
}
for (const [offset, value] of [[10, 1001], [12, 360]]) {
    const invalid = Buffer.from(golden); invalid.writeUInt16LE(value, offset);
    assert.equal(convert({observation: invalid}), undefined);
}

// Exercise the actual pinned herdsman ZCL octet-string parser and custom cluster.
const {Zcl} = hostRequire('zigbee-herdsman');
const clusters = {};
const device = {addCustomCluster: (name, cluster) => { clusters[name] = cluster; }};
definition.extend[0].configure[0](device);
const zclFrame = Buffer.concat([Buffer.from('18010a00004110', 'hex'), golden]);
const parsed = Zcl.Frame.fromBuffer(0xfc00, Zcl.Header.fromBuffer(zclFrame), zclFrame, clusters);
assert.deepEqual(Buffer.from(parsed.payload[0].attrData), golden);
const {addExternalDefinition, removeExternalDefinitions, findByDevice} = hostRequire('zigbee-herdsman-converters');
(async () => {
    await addExternalDefinition({...definition, externalConverterName: 'sustain-test'});
    const resolved = await findByDevice({modelID: 'SustainNode-v1', manufacturerName: 'Sustain',
        type: 'Router', endpoints: [], customClusters: {}, addCustomCluster: device.addCustomCluster});
    assert.equal(resolved.model, 'SustainNode-v1');
    removeExternalDefinitions('sustain-test');
    console.log('Converter golden, malformed input, pinned ZCL parser and definition registration: PASS');
})().catch(error => { console.error(error); process.exitCode = 1; });
