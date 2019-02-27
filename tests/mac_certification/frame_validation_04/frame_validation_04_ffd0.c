/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - TP/154/MAC/FRAME-VALIDATION-04, FFD0 (DUT)

Check that the MAC sublayer will discard a received frame that has a reserved value in the
Frame Type subfield.

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MLME_SET_CONFIRM
#define USE_ZB_MCPS_DATA_INDICATION
#define USE_ZB_MLME_ASSOCIATE_INDICATION
#define USE_ZB_MLME_COMM_STATUS_INDICATION
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define TEST_PAN_ID       0x1AAA
#define ZB_TEST_CHANNEL   0x14
#define TEST_CHANEL_MASK  (1l << ZB_TEST_CHANNEL)

/* DUT (FFD0)     */
static zb_ieee_addr_t g_DUT_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
#define ZB_DUT_ADDR       0x1122

/* Tester (FFD1)  */
static zb_ieee_addr_t g_TESTER_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
#define ZB_TESTER_ADDR    0x3344

#define LOG_FILE      "mac_fv_04_ffd0"



MAIN()
{
  ARGV_UNUSED;

  ZB_INIT(LOG_FILE, argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_DUT_addr);
  MAC_PIB().mac_pan_id   = TEST_PAN_ID;
  ZB_PIB_SHORT_ADDRESS() = ZB_DUT_ADDR;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_mlme_set_request_t *set_req;

    /* set rx_on_when_idle to true */
    ZB_BUF_INITIAL_ALLOC(buf, sizeof(zb_mlme_set_request_t) + sizeof(zb_uint8_t), set_req);
    set_req -> pib_attr   = ZB_PIB_ATTRIBUTE_RX_ON_WHEN_IDLE;
    set_req -> pib_length = sizeof(zb_uint8_t);
    *((zb_uint8_t *)(set_req + 1)) = ZB_TRUE;

    ZB_SCHEDULE_CALLBACK(zb_mlme_set_request, ZB_REF_FROM_BUF(buf));
  }

  while (1)
  {
    zb_sched_loop_iteration();

    /* check for sec interrupt */
    if ( TRANS_CTX().int_status & 0x10 )
    {
      TRACE_MSG(TRACE_NWK2, "got sec interrupt", (FMT__0));

      /* drop sec packet, clear rxfifo */
      ZB_RXFLUSH();

      /* clear context*/
      TRANS_CTX().int_status &= (~0x10);

      /* send mlme status indication */
      {
        zb_buf_t *buf = zb_get_out_buf();
        zb_mlme_comm_status_indication_t *ind = ZB_GET_BUF_PARAM(buf, zb_mlme_comm_status_indication_t);

        ind -> src_addr_mode        = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
        ind -> src_addr.addr_short  = 0x0100;
        ind -> dst_addr_mode        = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
        ind -> dst_addr.addr_short  = ZB_DUT_ADDR;
        ind -> status               = MAC_UNSUPPORTED_LEGACY;

        /* call comm status */
        ZB_SCHEDULE_CALLBACK(zb_mlme_comm_status_indication, ZB_REF_FROM_BUF(buf));
      }
    }
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_set_confirm(zb_uint8_t param) ZB_CALLBACK
{
  static zb_short_t state = 0;

  TRACE_MSG(TRACE_NWK2, "zb_mlme_set_confirm", (FMT__0));

  /* start PAN */
  if (state == 0)
  {
    zb_mlme_start_req_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_start_req_t);

    ZB_BZERO(req, sizeof(*req));
    req -> pan_id = MAC_PIB().mac_pan_id;
    req -> logical_channel  = ZB_TEST_CHANNEL;
    req -> pan_coordinator  = ZB_TRUE;      /* will be coordinator */
    req -> beacon_order     = ZB_TURN_OFF_ORDER;
    req -> superframe_order = ZB_TURN_OFF_ORDER;

    ZB_SCHEDULE_CALLBACK(zb_mlme_start_request, param);
  }
  else
  {
    zb_free_buf(ZB_BUF_FROM_REF(param));
  }

  state++;
}


void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}


void zb_mlme_associate_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_ieee_addr_t device_address;
  zb_mlme_associate_indication_t *request = ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_mlme_associate_indication_t);
  TRACE_MSG(TRACE_NWK1, ">>mlme_associate_ind %hd", (FMT__H, param));
  /*
    Very simple implementation: accept anybody, assign address 0x3344
   */
  ZB_IEEE_ADDR_COPY(device_address, request -> device_address);
  ZB_MLME_BUILD_ASSOCIATE_RESPONSE(ZB_BUF_FROM_REF(param), device_address, ZB_TESTER_ADDR, 0);
  ZB_SCHEDULE_CALLBACK(zb_mlme_associate_response, param);
  TRACE_MSG(TRACE_NWK1, "<<mlme_associate_ind", (FMT__0));
}


void zb_mcps_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *buf = ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_MAC1, ">> zb_mcps_data_indication param %hd", (FMT__H, param));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}


void zb_mlme_comm_status_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_comm_status_indication_t *ind_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_comm_status_indication_t);

  TRACE_MSG(TRACE_MAC1,
            "zb_mlme_comm_status_indication param %hd status %hd src_addr 0x%x dst_addr 0x%x",
            (FMT__D_H_H_D, param, ind_params->status,
             ind_params->src_addr.addr_short, ind_params->dst_addr.addr_short));

  zb_free_buf(ZB_BUF_FROM_REF(param));
}

/*! @} */
