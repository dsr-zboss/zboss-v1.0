/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - RFD
Associate with FFD without discovery
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_bufpool.h"
#include "zb_mac.h"

#define USE_ZB_MLME_ASSOCIATE_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#define USE_ZB_MCPS_DATA_INDICATION
#define USE_ZB_MLME_POLL_CONFIRM
#include "zb_mac_only_stubs.h"

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define ASSOCIATE_TO 0x1122
#define ASSOCIATE_TO_PAN 0x1aaa
#define CHANNEL 0x14
#define CAP_INFO 0x80           /* 80 - "allocate address" */
static zb_ieee_addr_t g_mac_addr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};
static zb_ieee_addr_t g_zc_addr = {0x01, 0x00, 0x00, 0x00, 0x00, 0x48, 0xde, 0xac};

static zb_short_t state = 1;

void poll_alarm(zb_uint8_t param) ZB_CALLBACK;

MAIN()
{
  ARGV_UNUSED;

  ZB_INIT("association_01_rfd", argv[1], argv[2]);

  ZB_IEEE_ADDR_COPY(ZB_PIB_EXTENDED_ADDRESS(), &g_mac_addr);
  MAC_PIB().mac_pan_id = 0x1aaa;

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_uint16_t addr = ASSOCIATE_TO;

    ZB_MLME_BUILD_ASSOCIATE_REQUEST(buf, CHANNEL,
                                    ASSOCIATE_TO_PAN,
                                    ZB_ADDR_16BIT_DEV_OR_BROADCAST, &addr,
                                    CAP_INFO);

    ZB_SCHEDULE_CALLBACK(zb_mlme_associate_request, ZB_REF_FROM_BUF(buf));
  }

  while(1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_associate_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_associate_confirm_t *request = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_associate_confirm_t);

  TRACE_MSG(TRACE_NWK2, "zb_mlme_associate_confirm param %hd status %hd short_address %hd",
            (FMT__H_H_H, param, request->status, request->assoc_short_address));

#if 0                           /* test for intra-pan portability */
  {
    zb_beacon_frame_params_t beacon_frame_params;

    zb_ret_t zb_send_beacon_frame(zb_beacon_frame_params_t *beacon_frame_params) ZB_SDCC_REENTRANT;


    beacon_frame_params.src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
    beacon_frame_params.security_enabled = 0;
    beacon_frame_params.beacon_order = MAC_PIB().mac_beacon_order;
    beacon_frame_params.superframe_order = MAC_PIB().mac_superframe_order;
    beacon_frame_params.ble = 0;
    beacon_frame_params.frame_pending = 0;
    zb_send_beacon_frame(&beacon_frame_params);
  }
#endif

  if (request->status == 0)
  {
    /* a. D.U.T. to tester.: Short Address to Short Address, Unicast, no ACK */
    {
      zb_uint8_t *pl;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x00;
      pl[1] = 0x01;
      pl[2] = 0x02;
      pl[3] = 0x03;
      pl[4] = 0x04;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->dst_addr.addr_short = 0x1122;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x3344;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = 0;
    }


    TRACE_MSG(TRACE_MAC1, "a. D.U.T. to tester.: Short Address to Short Address, Unicast, no ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
  }
}


void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx",
            (FMT__H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle, (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status));


  switch (state)
  {
    case 1:
      /* b. D.U.T. to tester.: Short Address to Short Address, Unicast, with ACK  */
    {
      zb_uint8_t *pl;
      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x01;
      pl[1] = 0x02;
      pl[2] = 0x03;
      pl[3] = 0x04;
      pl[4] = 0x05;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->dst_addr.addr_short = 0x1122;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x3344;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "b. D.U.T. to tester.: Short Address to Short Address, Unicast, with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 2:
      /* c. D.U.T. to tester.: Short Address to Extended Address, Unicast with ACK  */

    {
      zb_uint8_t *pl;
      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x10;
      pl[1] = 0x11;
      pl[2] = 0x12;
      pl[3] = 0x13;
      pl[4] = 0x14;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->src_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      ZB_IEEE_ADDR_COPY(data_req->dst_addr.addr_long, g_zc_addr);
      data_req->dst_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 9;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "c. D.U.T. to tester.: Short Address to Extended Address, Unicast with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 3:
      /* d. D.U.T. to tester.: Extended Address to Short Address , Unicast with ACK  */

    {
      zb_uint8_t *pl;
      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x20;
      pl[1] = 0x21;
      pl[2] = 0x22;
      pl[3] = 0x23;
      pl[4] = 0x24;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_mac_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_addr.addr_short = 0x1122;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "d. D.U.T. to tester.: Extended Address to Short Address , Unicast with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;


    case 4:
      /* e. D.U.T. to tester.: Extended Address to Extended Address , Unicast with ACK  */

    {
      zb_uint8_t *pl;
      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x17;
      pl[1] = 0x28;
      pl[2] = 0x39;
      pl[3] = 0x4a;
      pl[4] = 0x5b;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_mac_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(data_req->dst_addr.addr_long, g_zc_addr);
      data_req->dst_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }
    TRACE_MSG(TRACE_MAC1, "e. D.U.T. to tester.: Extended Address to Extended Address , Unicast with ACK", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 5:
      /* f. D.U.T. to tester.: Extended Address to Broadcast Address  */

    {
      zb_uint8_t *pl;
      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x6c;
      pl[1] = 0x7d;
      pl[2] = 0x8e;
      pl[3] = 0x9f;
      pl[4] = 0xa0;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      ZB_IEEE_ADDR_COPY(data_req->src_addr.addr_long, g_mac_addr);
      data_req->src_addr_mode = ZB_ADDR_64BIT_DEV;
      data_req->dst_addr.addr_short = 0xffff;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = 0;
    }
    TRACE_MSG(TRACE_MAC1, "f. D.U.T. to tester.: Extended Address to Broadcast Address", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;

    case 6:
      /* g. D.U.T. to tester.: Short Address to Broadcast Address */

    {
      zb_uint8_t *pl;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 5, pl);
      pl[0] = 0x00;
      pl[1] = 0x01;
      pl[2] = 0x02;
      pl[3] = 0x03;
      pl[4] = 0x04;
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->src_addr.addr_short = 0x3344;
      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_addr.addr_short = 0xffff;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = 0;
    }

    TRACE_MSG(TRACE_MAC1, "g. D.U.T. to tester.: Short Address to Broadcast Address", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
    break;


    case 7:
      /* h. D.U.T. to tester: Short Address to Short Address, Unicast with ACK - maximum data size */

    {
      zb_uint8_t *pl;
      zb_ushort_t i;

      ZB_BUF_INITIAL_ALLOC(ZB_BUF_FROM_REF(param), 74, pl);
      for (i = 0 ; i < 74 ; ++i)
      {
        pl[i] = i;
      }
    }
    {
      zb_mcps_data_req_params_t *data_req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_req_params_t);

      data_req->src_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->src_addr.addr_short = 0x3344;
      data_req->dst_addr.addr_short = 0x1122;
      data_req->dst_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      data_req->dst_pan_id = ASSOCIATE_TO_PAN;
      data_req->msdu_handle = 0;
      data_req->tx_options = MAC_TX_OPTION_ACKNOWLEDGED_BIT;
    }

    TRACE_MSG(TRACE_MAC1, "h. D.U.T. to tester: Short Address to Short Address, Unicast with ACK - maximum data size", (FMT__0));
    ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);

    break;
  case 8:
    ZB_SCHEDULE_ALARM(poll_alarm, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(1000));
    break;
  }

  state++;
}

void zb_mcps_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_MAC1, ">> zb_mcps_data_indication param %hd", (FMT__H, param));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}


void poll_alarm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mlme_poll_request_t *req = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mlme_poll_request_t);

  TRACE_MSG(TRACE_MAC1, "poll_alarm state %hd", (FMT__H, state));

  switch (state)
  {
    case 9:
      /* i. Tester to D.U.T: Short Address to Short Address, Indirect, with ACK  */
      req->coord_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      req->coord_addr.addr_short = 0x1122;
      req->coord_pan_id = ASSOCIATE_TO_PAN;
      TRACE_MSG(TRACE_MAC1, "poll from short to short", (FMT__0));
      ZB_SCHEDULE_CALLBACK(zb_mlme_poll_request, param);
      break;

    case 10:
      /* j. Tester to D.U.T: Short Address to Extended Address, Indirect, with ACK */
      /* poll from the long to short */
      ZB_PIB_SHORT_ADDRESS() = -1;
      req->coord_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      req->coord_addr.addr_short = 0x1122;
      req->coord_pan_id = ASSOCIATE_TO_PAN;
      TRACE_MSG(TRACE_MAC1, "poll from long to short", (FMT__0));
      ZB_SCHEDULE_CALLBACK(zb_mlme_poll_request, param);

      break;
    case 11:
      /* k. Tester to D.U.T: Extended Address to Short Address, Indirect, with ACK  */
      /* fall back to the poll from the short address */
      ZB_PIB_SHORT_ADDRESS() = 0x3344;
      req->coord_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(req->coord_addr.addr_long, g_zc_addr);
      req->coord_pan_id = ASSOCIATE_TO_PAN;
      TRACE_MSG(TRACE_MAC1, "poll from short to long", (FMT__0));
      ZB_SCHEDULE_CALLBACK(zb_mlme_poll_request, param);
      break;

    case 12:
      /* l. Tester to D.U.T: Extended Address to Extended Address, Indirect, with ACK */
      /* clear short address to poll from the long address */
      ZB_PIB_SHORT_ADDRESS() = -1;
      req->coord_addr_mode = ZB_ADDR_64BIT_DEV;
      ZB_IEEE_ADDR_COPY(req->coord_addr.addr_long, g_zc_addr);
      req->coord_pan_id = ASSOCIATE_TO_PAN;
      TRACE_MSG(TRACE_MAC1, "poll from long to long", (FMT__0));
      ZB_SCHEDULE_CALLBACK(zb_mlme_poll_request, param);
      break;

    case 13:
      /* m. Tester to D.U.T: Short Address to Short Address, Indirect, with ACK, maximum data size */
      ZB_PIB_SHORT_ADDRESS() = 0x3344;
      req->coord_addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
      req->coord_addr.addr_short = 0x1122;
      req->coord_pan_id = ASSOCIATE_TO_PAN;
      TRACE_MSG(TRACE_MAC1, "poll from short to short", (FMT__0));
      ZB_SCHEDULE_CALLBACK(zb_mlme_poll_request, param);
      break;
  }
  state++;
}


void zb_mlme_poll_confirm(zb_uint8_t param) ZB_CALLBACK
{
  ZB_SCHEDULE_ALARM(poll_alarm, param, ZB_MILLISECONDS_TO_BEACON_INTERVAL(1000));
}


/*! @} */
