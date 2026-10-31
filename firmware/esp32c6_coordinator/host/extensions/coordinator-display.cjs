/* Waveshare coordinator display. Uses the existing adapter queue, never a
 * second serial connection. Compatible with pinned herdsman 10.10.0 / Z2M 2.14.2. */
const {FRAMES} = require('zigbee-herdsman/dist/adapter/zboss/commands');
const U8 = 0x20, U16 = 0x21, U32 = 0x23;
const STATUS = 0x7f10, DIAGNOSTICS = 0x7f11, FAULT = 0x7f12;
const response = [
    ['category', U8], ['status', U8], ['version', U8], ['flags', U8], ['page', U8],
    ['touches', U16], ['frames', U32], ['drawErrors', U16], ['uptime', U32],
    ['hostAge', U16], ['heapKiB', U16],
].map(([name, type]) => ({name, type}));

class CoordinatorDisplay {
    constructor(zigbee, mqtt, state, publishEntityState, eventBus, enableDisable, restart, add, settings, logger) {
        Object.assign(this, {zigbee, mqtt, eventBus, settings, logger});
        this.availability = new Map();
        this.stopped = true;
        this.failed = 0;
    }
    async start() {
        const version = require('zigbee-herdsman/package.json').version;
        if (version !== '10.10.0') throw new Error(`Display extension requires reviewed herdsman 10.10.0, found ${version}`);
        this.driver = this.zigbee.zhController.adapter.driver;
        if (!this.driver?.execCommand || FRAMES[STATUS] || FRAMES[DIAGNOSTICS] || FRAMES[FAULT])
            throw new Error('Display requires an unused vendor-command pair and the ZBOSS adapter');
        FRAMES[STATUS] = {request: [
            ['version', U8], ['flags', U8], ['channel', U8], ['registered', U16],
            ['available', U16], ['stale', U16],
        ].map(([name, type]) => ({name, type})), response};
        FRAMES[DIAGNOSTICS] = {request: [], response};
        FRAMES[FAULT] = {request: [], response: [
            ['category',U8],['status',U8],['version',U8],['stage',U8],['error',U32],
            ['x',U16],['y',U16],['width',U16],['height',U16],['rotation',U8],
            ['libraryRotation',U8],
        ].map(([name,type])=>({name,type}))};
        this.stopped = false;
        const capture = ({topic, payload}) => {
            const prefix = `${this.settings.get().mqtt.base_topic}/`;
            if (!topic.startsWith(prefix) || !topic.endsWith('/availability')) return;
            try {
                const status = JSON.parse(payload).state;
                if (status === 'online' || status === 'offline')
                    this.availability.set(topic.slice(prefix.length, -'/availability'.length), status);
            } catch { /* Unrelated/malformed MQTT payload does not affect the NCP. */ }
        };
        this.eventBus.onMQTTMessagePublished(this, capture);
        for (const message of Object.values(this.mqtt.retainedMessages || {})) {
            capture({topic: `${this.settings.get().mqtt.base_topic}/${message.topic}`, payload: message.payload});
        }
        try {
            const first = await this.driver.execCommand(DIAGNOSTICS, {}, 3000);
            await this.publish(first.payload, 'before-first-update');
        } catch (error) {
            await this.stop();
            throw new Error(`Connected firmware has no compatible display endpoint: ${error.message}`);
        }
        this.timer = setInterval(() => this.tick(), 5000);
        await this.tick();
    }
    snapshot() {
        const controller = this.zigbee.zhController;
        const devices = controller.getDevices().filter(d => d.type !== 'Coordinator');
        let available = 0, stale = 0;
        for (const d of devices) {
            const name = this.settings.get().devices?.[d.ieeeAddr]?.friendly_name || d.ieeeAddr;
            if (this.availability.get(name) === 'online') ++available;
            else if (this.availability.get(name) === 'offline') ++stale;
        }
        const network = !controller.isAdapterDisconnected() && !!this.driver.netInfo?.joined;
        return {version: 1, flags: (network ? 1 : 0) | (this.mqtt.isConnected() ? 2 : 0) |
            (controller.getPermitJoin() ? 4 : 0),
            channel: this.driver.netInfo?.network?.channel || 0,
            registered: devices.length, available, stale};
    }
    async publish(payload, phase, counts) {
        if (!this.mqtt.isConnected()) return;
        let displayFault;
        if (payload.drawErrors) {
            try { displayFault=(await this.driver.execCommand(FAULT,{},3000)).payload; }
            catch { /* Older compatible firmware may lack the optional fault endpoint. */ }
        }
        await this.mqtt.publish('bridge/display', JSON.stringify({phase, timestamp: Date.now(), ...payload,
            panelReady: !!(payload.flags & 1), touchReady: !!(payload.flags & 2),
            hostFresh: !!(payload.flags & 4),
            displaySleeping: !!(payload.flags & 16), settingsSaved: !!(payload.flags & 32),
            rotationDegrees: ((payload.flags >> 6) & 3) * 90,
            ...(displayFault?{displayFault}:{}),
            ...(counts ? {counts, unknown: counts.registered-counts.available-counts.stale} : {}),
        }), {clientOptions: {retain: true}, skipLog: true});
    }
    async tick() {
        if (this.stopped || this.pending) return;
        this.pending = (async () => {
            try {
                const counts = this.snapshot();
                const reply = await this.driver.execCommand(STATUS, counts, 3000);
                if (reply.payload.version !== 1) throw new Error('Unexpected UI response version');
                this.failed = 0;
                await this.publish(reply.payload, 'live', counts);
            } catch (error) {
                if (++this.failed === 1) this.logger.error(`Coordinator display update failed: ${error.message}`);
                if (this.failed >= 3) { clearInterval(this.timer); this.stopped = true; }
            }
        })();
        try { await this.pending; } finally { this.pending = undefined; }
    }
    async stop() {
        this.stopped = true;
        clearInterval(this.timer);
        if (this.pending) await this.pending;
        this.eventBus.removeListeners(this);
        delete FRAMES[STATUS]; delete FRAMES[DIAGNOSTICS];
        delete FRAMES[FAULT];
    }
}
module.exports = CoordinatorDisplay;
