/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: test for mac - end device side
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_secur.h"

#define USE_ZB_MLME_SCAN_CONFIRM
#define USE_ZB_MLME_BEACON_NOTIFY_INDICATION
#define USE_ZB_MLME_START_CONFIRM
#define USE_ZB_MCPS_DATA_CONFIRM
#include "zb_mac_only_stubs.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#define MY_ADDR   0x0002
#define LOG_FILE "tst_ze"
//#define TEST_CHANEL_MASK (1l << 14)
#define TEST_CHANEL_MASK ZB_TRANSCEIVER_ALL_CHANNELS_MASK /* (0xffff << 11) */

void scanning_03(zb_uint8_t param1) ZB_CALLBACK
{
  zb_buf_t *buf = zb_get_out_buf();
  zb_nlme_network_discovery_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_network_discovery_request_t);
  zb_ret_t ret;

  ZVUNUSED(param1);

  MAC_PIB().mac_auto_request = 1;
  ZB_AIB().aps_channel_mask = TEST_CHANEL_MASK;
  req->scan_channels = ZB_AIB().aps_channel_mask;
  req->scan_duration = ZB_DEFAULT_SCAN_DURATION; /* TODO: configure it somehow? */
  TRACE_MSG(TRACE_APS1, "disc, then join by assoc", (FMT__0));

  ZDO_CTX().zdo_ctx.discovery_ctx.disc_count = ZDO_CTX().conf_attr.nwk_scan_attempts;
  ret = ZB_SCHEDULE_CALLBACK(zb_nlme_network_discovery_request, ZB_REF_FROM_BUF(buf));
}


/*
  Link without APS.
  Define here APS routines.
*/

void zb_nlde_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *nsdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_NWK3, "nlde_data_confirm: packet %p status %d", (FMT__P_D,
            nsdu, nsdu->u.hdr.status));

  /* FIXME: Seems this buffer removed twice */
  zb_free_buf(nsdu);
}


void zb_nlde_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_buf_t *nsdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_NWK3, "nlde_data_indication: packet %p handle 0x%x", (FMT__P_D,
            nsdu, nsdu->u.hdr.status));
  zb_free_buf(nsdu);
}


void usage(char **argv)
{
#ifndef ZB8051
  printf("%s <read pipe path> <write pipe path>\n", argv[0]);
#else
  ZVUNUSED(argv);
#endif
}


MAIN()
{
  ARGV_UNUSED;

  ZB_INIT(LOG_FILE, argv[1], argv[2]);

  /* In the real life network address will be assigned at network join; now
   * hardcode it. */
  ZB_NIB_NETWORK_ADDRESS() = MY_ADDR;
  ZG->nwk.handle.joined = ZB_TRUE;

  /* call nwk API from the scheduler loop */
  ZB_SCHEDULE_CALLBACK(scanning_03, 0);

  while(1)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_mlme_start_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK2, "zb_mlme_start_confirm", (FMT__0));

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

    data_req->dst_addr = 0x3344;
    data_req->src_addr = 0x1122;
    data_req->msdu_handle = 0;
    data_req->tx_options = 0;
  }

  ZB_SCHEDULE_CALLBACK(zb_mcps_data_request, param);
}

void zb_mcps_data_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mcps_data_confirm_params_t *confirm_params = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mcps_data_confirm_params_t);
  TRACE_MSG(TRACE_NWK2, "zb_mcps_data_confirm param %hd handle 0x%hx status 0x%hx",
            (FMT__H_H_H, (zb_uint8_t)param, (zb_uint8_t)confirm_params->msdu_handle, (zb_uint8_t)ZB_BUF_FROM_REF(param)->u.hdr.status));
  zb_free_buf(ZB_BUF_FROM_REF(param));
}


void zb_nlme_network_discovery_request(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_network_discovery_request_t *request =
      ZB_GET_BUF_PARAM((zb_buf_t *)ZB_BUF_FROM_REF(param), zb_nlme_network_discovery_request_t);
  zb_nlme_network_discovery_request_t rq;

  TRACE_MSG(TRACE_NWK1, ">>zb_nlme_network_discovery_request %p", (FMT__P, request));

  ZB_MEMCPY(&rq, request, sizeof(rq));
  TRACE_MSG(TRACE_NWK1, "scan channels 0x%x scan_duration %hd", (FMT__D_H, rq.scan_channels, rq.scan_duration));
  ZB_MLME_BUILD_SCAN_REQUEST((zb_buf_t *)ZB_BUF_FROM_REF(param), rq.scan_channels, ACTIVE_SCAN, rq.scan_duration);
  ZB_SCHEDULE_CALLBACK(zb_mlme_scan_request, param);

  TRACE_MSG(TRACE_NWK1, "<<zb_nlme_network_discovery_request", (FMT__0));
}

void zb_mlme_beacon_notify_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_uint8_t *mac_hdr = ZB_MAC_GET_FCF_PTR(ZB_BUF_BEGIN((zb_buf_t *)ZB_BUF_FROM_REF(param)));
  zb_mac_mhr_t mhr;
  zb_uint8_t mhr_len;
  zb_mac_beacon_payload_t *beacon_payload;

  TRACE_MSG(TRACE_NWK1, ">>zb_mlme_beacon_notify_indication %hd", (FMT__H, param));
  mhr_len = zb_parse_mhr(&mhr, mac_hdr);
  ZB_MAC_GET_BEACON_PAYLOAD(mac_hdr, mhr_len, beacon_payload);

#ifdef ZB_MAC_TESTING_MODE
  {
    zb_uint8_t payload_len;

    payload_len = ZB_BUF_LEN(ZB_BUF_FROM_REF(param)) - ZB_BEACON_PAYLOAD_OFFSET(mhr_len) - ZB_BEACON_PAYLOAD_TAIL;

    TRACE_MSG(TRACE_NWK1, "beacon payload dump", (FMT__0));
    dump_traf((zb_uint8_t*)beacon_payload, payload_len);
  }
#endif  /* ZB_MAC_TESTING_MODE */

  TRACE_MSG(TRACE_NWK1, "<<zb_mlme_beacon_notify_indication", (FMT__0));
}

void zb_mlme_scan_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_mac_scan_confirm_t *scan_confirm;
  zb_uint8_t i;

  TRACE_MSG(TRACE_NWK1, ">>zb_mlme_scan_confirm %hd", (FMT__H, param));

#ifdef ZB_MAC_TESTING_MODE

  scan_confirm = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_mac_scan_confirm_t);
  TRACE_MSG(TRACE_NWK3, "scan type %hd, status %hd, auto_req %hd",
            (FMT__H_H_H, scan_confirm->scan_type, scan_confirm->status, MAC_PIB().mac_auto_request));
  if (scan_confirm->scan_type == ACTIVE_SCAN && scan_confirm->status == MAC_SUCCESS && MAC_PIB().mac_auto_request)
  {
    zb_pan_descriptor_t *pan_desc;
    pan_desc = (zb_pan_descriptor_t*)ZB_BUF_BEGIN(ZB_BUF_FROM_REF(param));
    TRACE_MSG(TRACE_NWK3, "ative scan res count %hd", (FMT__H, scan_confirm->result_list_size));
    for(i = 0; i < scan_confirm->result_list_size; i++)
    {
      TRACE_MSG(TRACE_NWK3,
                "pan desc: coord addr mode %hd, coord addr %x, pan id %x, channel %hd, superframe %x, lqi %hx",
                (FMT__H_D_D_H_D_H, pan_desc->coord_addr_mode, pan_desc->coord_address.addr_short, pan_desc->coord_pan_id,
                 pan_desc->logical_channel, pan_desc->super_frame_spec, pan_desc->link_quality));

      if (pan_desc->coord_addr_mode == ZB_ADDR_64BIT_DEV)
      {
        TRACE_MSG(TRACE_MAC3, "Extended coord addr " TRACE_FORMAT_64,
                  (FMT__A, TRACE_ARG_64(pan_desc->coord_address.addr_long)));
      }

      pan_desc++;
    }
  }
#endif

  TRACE_MSG(TRACE_NWK1, "<<zb_mlme_scan_confirm", (FMT__0));
}


/*! @} */
