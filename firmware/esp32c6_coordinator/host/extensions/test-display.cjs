const assert = require('node:assert/strict');
const Module = require('node:module');
const path = require('node:path');
const base = path.resolve(__dirname, '../node_modules');
const extensionModule = new Module(path.join(__dirname, 'coordinator-display.cjs'));
extensionModule.filename = path.join(__dirname, 'coordinator-display.cjs');
extensionModule.paths = [base];
extensionModule.load(extensionModule.filename);
const Display = extensionModule.exports;
const {FRAMES} = require('../node_modules/zigbee-herdsman/dist/adapter/zboss/commands');
const {makeFrame, writeZBOSSFrame, readZBOSSFrame} = require('../node_modules/zigbee-herdsman/dist/adapter/zboss/frame');
const events = {};
const publications = [];
const controller = {
    adapter: {driver: {netInfo: {joined: true, network: {channel: 15}},
        execCommand: async (cmd, params) => ({payload: {category:0,status:0,version:1,flags:7,page:0,
            touches:2,frames:10,drawErrors:0,uptime:10,hostAge:0,heapKiB:100}})}},
    getDevices: () => [{type:'Coordinator'},{type:'EndDevice',ieeeAddr:'node-a'}, {type:'Router',ieeeAddr:'node-b'}],
    isAdapterDisconnected: () => false, getPermitJoin: () => false,
};
const mqtt = {isConnected:()=>true, retainedMessages:{}, publish:async(t,p)=>publications.push([t,JSON.parse(p)])};
const bus = {onMQTTMessagePublished:(_,fn)=>events.publish=fn,removeListeners:()=>{}};
const settings = {get:()=>({mqtt:{base_topic:'test'},devices:{'node-a':{friendly_name:'node-a'}}})};
(async()=>{
    const ext = new Display({zhController:controller},mqtt,null,null,bus,null,null,null,settings,console);
    try {
        await ext.start();
        assert.equal(ext.snapshot().registered,2);
        assert.equal(ext.snapshot().available,0); // Unknown is not offline.
        events.publish({topic:'test/node-a/availability',payload:'{"state":"online"}'});
        events.publish({topic:'test/node-b/availability',payload:'{"state":"offline"}'});
        const s=ext.snapshot(); assert.equal(s.available,1); assert.equal(s.stale,1);
        const bytes=writeZBOSSFrame(makeFrame(0,0x7f10,s));
        assert.equal(bytes.subarray(5).toString('hex'),'01030f020001000100');
        const response=Buffer.from('0001107f01000001070002000a00000000000a00000000006400','hex');
        const parsed=readZBOSSFrame(response); assert.equal(parsed.payload.frames,10); assert.equal(parsed.payload.heapKiB,100);
        assert.equal(publications[0][0],'bridge/display');
    } finally { await ext.stop(); }
    assert.equal(FRAMES[0x7f10],undefined);
    console.log('PASS: real host codec, registry/availability semantics, UI extension cleanup');
})().catch(e=>{console.error(e);process.exitCode=1});
