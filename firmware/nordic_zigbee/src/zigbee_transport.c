/** @file
 * @brief ZBOSS endpoint registration, report scheduling and buffer ownership.
 *
 * Application threads encode into a spinlock-protected pending slot. ZBOSS
 * callbacks own in_flight, allocate the outgoing buffer and free it after
 * completion or immediate rejection. No persistent event queue is implemented.
 */
#include <errno.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <zboss_api.h>
#include <zb_nrf_platform.h>
#include <zigbee/zigbee_app_utils.h>
#include "board_status.h"
#include "zigbee_transport.h"

LOG_MODULE_REGISTER(sustain_zigbee, LOG_LEVEL_INF);

#define NODE_ENDPOINT 10
#define OBS_CLUSTER 0xfc00
#define OBS_ATTRIBUTE 0x0000
/* This private cluster uses generic read/discover commands only. */
#define OBS_CLUSTER_SERVER_ROLE_INIT NULL
#define OBS_CLUSTER_CLIENT_ROLE_INIT NULL

static zb_uint8_t zcl_version = ZB_ZCL_VERSION;
static zb_uint8_t power_source;
static zb_uint16_t identify_time;
static zb_char_t manufacturer[] = "\x07" "Sustain";
static zb_char_t model[] = "\x0e" "SustainNode-v1";
/* Octet string: ZCL length byte followed by the versioned wire record. */
static zb_uint8_t observation_attr[1 + OBSERVATION_WIRE_SIZE] = { OBSERVATION_WIRE_SIZE };
static uint8_t pending[OBSERVATION_WIRE_SIZE];
static struct k_spinlock pending_lock;
static atomic_t joined;
static bool in_flight; /* Only touched in ZBOSS context. */

ZB_ZCL_START_DECLARE_ATTRIB_LIST(basic_attrs)
 ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &zcl_version)
 ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &power_source)
 ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, manufacturer)
 ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, model)
ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST;

ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(identify_attrs, &identify_time);

ZB_ZCL_START_DECLARE_ATTRIB_LIST(obs_attrs)
 ZB_ZCL_SET_ATTR_DESC_M(OBS_ATTRIBUTE, observation_attr, ZB_ZCL_ATTR_TYPE_OCTET_STRING,
                       ZB_ZCL_ATTR_ACCESS_READ_ONLY)
ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST;

static zb_zcl_cluster_desc_t clusters[] = {
	ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_BASIC, ZB_ZCL_ARRAY_SIZE(basic_attrs, zb_zcl_attr_t),
		basic_attrs, ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
	ZB_ZCL_CLUSTER_DESC(ZB_ZCL_CLUSTER_ID_IDENTIFY, ZB_ZCL_ARRAY_SIZE(identify_attrs, zb_zcl_attr_t),
		identify_attrs, ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
	ZB_ZCL_CLUSTER_DESC(OBS_CLUSTER, ZB_ZCL_ARRAY_SIZE(obs_attrs, zb_zcl_attr_t),
		obs_attrs, ZB_ZCL_CLUSTER_SERVER_ROLE, ZB_ZCL_MANUF_CODE_INVALID),
};

ZB_DECLARE_SIMPLE_DESC(3, 0);
static ZB_AF_SIMPLE_DESC_TYPE(3, 0) simple_desc = {
	NODE_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_HA_CUSTOM_ATTR_DEVICE_ID, 1, 0, 3, 0,
	{ ZB_ZCL_CLUSTER_ID_BASIC, ZB_ZCL_CLUSTER_ID_IDENTIFY, OBS_CLUSTER }
};
ZB_AF_DECLARE_ENDPOINT_DESC(node_ep, NODE_ENDPOINT, ZB_AF_HA_PROFILE_ID, 0, NULL,
	ZB_ZCL_ARRAY_SIZE(clusters, zb_zcl_cluster_desc_t), clusters,
	(zb_af_simple_desc_1_1_t *)&simple_desc, 0, NULL, 0, NULL);
ZBOSS_DECLARE_DEVICE_CTX_1_EP(node_ctx, node_ep);

static void report_complete(zb_bufid_t buffer)
{
	zb_zcl_command_send_status_t *status =
		ZB_BUF_GET_PARAM(buffer, zb_zcl_command_send_status_t);
	LOG_INF("Report APS completion status=%d", status->status);
	in_flight = false;
	zb_buf_free(buffer);
}

static void send_report(zb_bufid_t buffer)
{
	if (!ZB_JOINED()) {
		in_flight = false;
		zb_buf_free(buffer);
		return;
	}
	k_spinlock_key_t key = k_spin_lock(&pending_lock);
	memcpy(observation_attr + 1, pending, sizeof(pending));
	k_spin_unlock(&pending_lock, key);

	zb_uint8_t *ptr = ZB_ZCL_START_PACKET(buffer);
	ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(ptr,
		ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,
		ZB_ZCL_DISABLE_DEFAULT_RESPONSE);
	ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_REPORT_ATTRIB);
	ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, OBS_ATTRIBUTE);
	ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_ATTR_TYPE_OCTET_STRING);
	for (size_t i = 0; i < sizeof(observation_attr); ++i) {
		ZB_ZCL_PACKET_PUT_DATA8(ptr, observation_attr[i]);
	}
	/* Bench contract: coordinator short address 0, HA endpoint 1.
	 * Explicit periodic reports avoid binding/configure-report dependencies.
	 */
	zb_addr_u destination = { .addr_short = 0x0000 };
	zb_ret_t result = zb_zcl_finish_and_send_packet(buffer, ptr, &destination,
		ZB_APS_ADDR_MODE_16_ENDP_PRESENT, 1, NODE_ENDPOINT,
		ZB_AF_HA_PROFILE_ID, OBS_CLUSTER, report_complete);
	if (result != RET_OK) {
		LOG_WRN("Report rejected: %d", result);
		in_flight = false;
		zb_buf_free(buffer);
	}
}

static void request_report(zb_uint8_t unused)
{
	ARG_UNUSED(unused);
	if (in_flight || !ZB_JOINED()) { return; }
	in_flight = true;
	if (zb_buf_get_out_delayed(send_report) != RET_OK) {
		in_flight = false;
		LOG_WRN("No output buffer; latest observation waits for next interval");
	}
}

int zigbee_transport_submit(const struct observation *value)
{
	uint8_t encoded[OBSERVATION_WIRE_SIZE];
	int err = observation_encode(value, encoded);
	if (err) { return err; }
	if (!atomic_get(&joined)) { return -ENOTCONN; }
	k_spinlock_key_t key = k_spin_lock(&pending_lock);
	memcpy(pending, encoded, sizeof(pending));
	k_spin_unlock(&pending_lock, key);
	return ZB_SCHEDULE_APP_CALLBACK(request_report, 0) == RET_OK ? 0 : -EAGAIN;
}

void zboss_signal_handler(zb_bufid_t buffer)
{
	zb_ret_t result = zigbee_default_signal_handler(buffer);
	if (result != RET_OK) { LOG_WRN("Default signal handler: %d", result); }
	bool connected = ZB_JOINED();
	atomic_set(&joined, connected);
	board_status_joined(connected);
	LOG_INF("Network joined=%d", connected);
	if (buffer) { zb_buf_free(buffer); }
}

void zigbee_transport_start(void)
{
	/* USB-powered bench devices report DC power, including the SED profile. */
	power_source = ZB_ZCL_BASIC_POWER_SOURCE_DC_SOURCE;
	ZB_AF_REGISTER_DEVICE_CTX(&node_ctx);
	if (IS_ENABLED(CONFIG_SUSTAIN_SLEEPY)) {
#if defined(CONFIG_ZIGBEE_ROLE_END_DEVICE)
		zb_set_keepalive_timeout(ZB_MILLISECONDS_TO_BEACON_INTERVAL(3000));
		zigbee_configure_sleepy_behavior(true);
#endif
	}
	zigbee_enable(); /* Retain normal ZBOSS NVRAM/rejoin behavior. */
}
