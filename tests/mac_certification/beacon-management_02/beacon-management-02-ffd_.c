/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - end device side (FFD0)

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_secur.h"

#define USE_ZB_MLME_RESET_CONFIRM
#define USE_ZB_MLME_SET_CONFIRM
#define USE_ZB_MLME_START_CONFIRM
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#ifndef ZB_COORDINATOR_ROLE
#error Coordinator role is not compiled!
#endif

#define LOG_FILE      "mac_bm_02"
#define ZB_TEST_ADDR  0x1122

#define TEST_PAN_ID   0x1AAA
static zb_ieee_addr_t g_ffd0_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_ffd1_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};


#define ZB_TEST_CHANNEL               0x14
#define TEST_CHANEL_MASK (1l << ZB_TEST_CHANNEL)

#define ZB_TEST_BEACON_PAYLOAD_LENGTH 2
#define ZB_TEST_BEACON_PAYLOAD        "\x12\x34"


MAIN()
{
    ARGV_UNUSED;

    /* Init device, load IB values from nvram or set it to default */
    ZB_INIT(LOG_FILE, argv[1], argv[2]);

    ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_ffd0_addr);

    {
        zb_buf_t *buf = zb_get_out_buf();
        zb_mlme_reset_request_t *reset_req = ZB_GET_BUF_PARAM(buf, zb_mlme_reset_request_t);
        reset_req -> set_default_pib = ZB_TRUE;
        /* channel # 0x14 */
        ZB_AIB().aps_channel_mask = (1l << ZB_TEST_CHANNEL);

        ZB_SCHEDULE_CALLBACK(zb_mlme_reset_request, ZB_REF_FROM_BUF(buf));
    }

    for (;;)
    {
        zb_sched_loop_iteration();
    }

    TRACE_DEINIT();
    MAIN_RETURN(0);
}


void zb_mlme_reset_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_reset_confirm", (FMT__0));
  {
    zb_mlme_set_request_t *set_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_set_request_t);

    set_req -> pib_attr  = ZB_PIB_ATTRIBUTE_SHORT_ADDRESS;
    set_req -> pib_length = sizeof(zb_uint16_t);
    *((zb_uint16_t *)(set_req + 1)) = ZB_TEST_ADDR;

//    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, ZB_REF_FROM_BUF(param));
    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
  }
}

void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{
  static zb_uint8_t pass = 0;
  zb_uint8_t size;

  TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm_1", (FMT__0));

  if (!pass)
  {
    // 1 - st MLME-SET.request
    zb_mlme_set_request_t *set_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_set_request_t);

    set_req -> pib_attr   = ZB_PIB_ATTRIBUTE_BEACON_PAYLOAD;
    set_req -> pib_length = sizeof(zb_mac_beacon_payload_t);
    *((zb_uint8_t *)(set_req + 1)) = ZB_TEST_BEACON_PAYLOAD_LENGTH;

    size = sizeof(zb_mac_beacon_payload_t);
    if (size > ZB_TEST_BEACON_PAYLOAD_LENGTH)
    {
      size = ZB_TEST_BEACON_PAYLOAD_LENGTH;
    }

    ZB_MEMSET((zb_uint8_t *)(set_req + 1), 0, sizeof(zb_mac_beacon_payload_t));
    ZB_MEMCPY((zb_uint8_t *)(set_req + 1), ZB_TEST_BEACON_PAYLOAD, size);
    pass++;
//    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, ZB_REF_FROM_BUF(param));
    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, param);
  } else
  // 2 - nd MLME-SET.request
  {
    zb_mlme_start_req_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_start_req_t);

    TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm_2", (FMT__0));

    /* start PAN */
    ZB_BZERO(req, sizeof(*req));
    req -> pan_id                 = TEST_PAN_ID;
    req -> logical_channel        = ZB_TEST_CHANNEL;
    req -> beacon_order           = ZB_TURN_OFF_ORDER;
    req -> superframe_order       = ZB_TURN_OFF_ORDER;
    req -> pan_coordinator        = ZB_TRUE;      /* will be coordinator */
    req -> battery_life_extension = ZB_FALSE;
    req -> coord_realignment      = ZB_FALSE;

//    ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, ZB_REF_FROM_BUF(param));
    ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, param);
  }
}

void usage(char **argv)
{
#ifndef ZB8051
  printf("%s <read pipe path> <write pipe path>\n", argv[0]);
#else
  ZVUNUSED(argv);
#endif
}

void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}

/*! @} */
