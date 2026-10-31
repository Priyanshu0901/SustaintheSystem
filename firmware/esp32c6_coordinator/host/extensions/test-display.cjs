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
        execCommand: async (cmd, params) => ({payload: cmd===0x7f12?
            {category:0,status:0,version:1,stage:2,error:263,x:0,y:0,width:170,height:30,rotation:0,libraryRotation:0}:
            {category:0,status:0,version:1,flags:7,page:0,
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
        assert.equal(publications[0][1].displaySleeping,false);
        assert.equal(publications[0][1].rotationDegrees,0);
        await ext.publish({flags:16|32|128,page:2},'screen-test');
        const screen=publications.at(-1)[1];
        assert.equal(screen.displaySleeping,true);
        assert.equal(screen.settingsSaved,true);
        assert.equal(screen.rotationDegrees,180);
        const fault=readZBOSSFrame(Buffer.from('0001127f01000001020701000000000000aa001e000000','hex'));
        assert.equal(fault.payload.error,263); assert.equal(fault.payload.height,30);
        await ext.publish({flags:0,drawErrors:1},'fault-test');
        assert.equal(publications.at(-1)[1].displayFault.error,263);
    } finally { await ext.stop(); }
    assert.equal(FRAMES[0x7f10],undefined);
    assert.equal(FRAMES[0x7f12],undefined);
    console.log('PASS: real host codec, registry/availability semantics, UI extension cleanup');
})().catch(e=>{console.error(e);process.exitCode=1});
