/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for NWK formation and discovery (ZC).
Start PAN, then stay in the loop debugging discovery

Note: that test compiled without ZDO and security, so will work only if all
stack built without security

*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_aps.h"
#include "zb_zdo.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

void zb_nlme_network_formation_confirm(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_permit_joining_request_t *request = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_permit_joining_request_t);
  TRACE_MSG(TRACE_NWK1, "formation confirm status %d", (FMT__D, ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status));

  request->permit_duration = 0xff; /* permit forewer */
  ZB_SCHEDULE_CALLBACK(zb_nlme_permit_joining_request, param);
}


void zb_nlme_permit_joining_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK1, "permit joining confirm status %d", (FMT__D, ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status));
}


void zb_nlme_join_indication(zb_uint8_t param) ZB_CALLBACK
{
  zb_nlme_join_indication_t *ind = ZB_GET_BUF_PARAM(ZB_BUF_FROM_REF(param), zb_nlme_join_indication_t);
  TRACE_MSG(TRACE_NWK1,
            "joined (status %d) dev %d/" TRACE_FORMAT_64 " cap: dev type %d rx on when idle. %d rejoin %d secure %d", (FMT__D_D_A_D_D_D_D,
            ((zb_buf_t *)ZB_BUF_FROM_REF(param))->u.hdr.status,
            ind->network_address, TRACE_ARG_64(ind->extended_address),
            ZB_MAC_CAP_GET_DEVICE_TYPE(ind->capability_information),
            ZB_MAC_CAP_GET_RX_ON_WHEN_IDLE(ind->capability_information),
            ind->rejoin_network, ind->secure_rejoin));
}

void nwk_form_data_indication(zb_uint8_t param) ZB_CALLBACK
{
  int ret = 0;
  zb_short_t i;
  zb_short_t len;
  zb_uint8_t *ptr;
  zb_buf_t *asdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_APS3, "apsde_data_indication: packet %p handle 0x%x", (FMT__P_D,
            asdu, asdu->u.hdr.status));

  /* Remove APS header from the packet */
  ZB_APS_HDR_CUT(asdu, ptr);
  len = ZB_BUF_LEN(asdu);

  if (len != 80)
  {
    TRACE_MSG(TRACE_ERROR, "Wrong len: %d, wants 80", (FMT__D, len));
    ret = -1;
  }

  for (i = 0 ; i < len ; ++i)
  {
    if (ptr[i] != i % 32 + '0')
    {
      TRACE_MSG(TRACE_ERROR, "Bad ptr[%d]: %d, wants %d", (FMT__D_D_D, i, ptr[i], i % 32 + '0'));
      ret = -1;
    }
  }

  if ( !ret )
  {
    TRACE_MSG(TRACE_ERROR, "SUCCESS!!!", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_ERROR, "ERROR!!!", (FMT__0));
  }

  zb_free_buf(asdu);
}


void usage(char **argv)
{
#ifndef ZB8051
  printf("%s <read pipe path> <write pipe path>\n", argv[0]);
#else
  (void)argv;
#endif
}



MAIN()
{
  zb_int_t i;
  ARGV_UNUSED;

#ifndef KEIL
  if ( argc < 3 )
  {
    usage(argv);
    return 0;
  }
#endif

#ifndef ZB8051
  ZB_INIT("formation", argv[1], argv[2]);
#else
  ZB_INIT("formation", "0", "0");
#endif

  {
    zb_buf_t *buf = zb_get_out_buf();
    zb_nlme_network_formation_request_t *req = ZB_GET_BUF_PARAM(buf, zb_nlme_network_formation_request_t);
    req->scan_channels = (1<<12)|(1<<13);
    req->scan_duration = 3;     /* timeout for every channel is
                                   ((1<<duration) + 1) * 15360 / 1000000

                                   For duration 8 ~ 4s
                                   For duration 5 ~0.5s
                                   For duration 2 ~0.08s
                                   For duration 1 ~0.05s
                                */
    ZB_SCHEDULE_CALLBACK(zb_nlme_network_formation_request, ZB_REF_FROM_BUF(buf));
  }

  for (i = 0 ; i < 1000 ; ++i)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
  zb_af_set_data_indication(nwk_form_data_indication);
}

/* just to compile... */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}


void zb_nlme_network_discovery_confirm(zb_uint8_t param) ZB_CALLBACK
{
  TRACE_MSG(TRACE_NWK1, ">>zb_nlme_network_discovery_confirm %hd", (FMT__H, param));
  TRACE_MSG(TRACE_NWK1, "<<zb_nlme_network_discovery_confirm", (FMT__0));
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_aps_in_transport_key(zb_uint8_t param)
{
  (void)param;
}

void zb_aps_in_update_device(zb_uint8_t param)
{
  (void)param;
}

void zb_nwk_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
}

void zb_aps_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
}

zb_ret_t zb_aps_unsecure_frame(zb_buf_t *buf)
{
  (void)buf;
  return 0;
}

zb_ret_t zb_nwk_unsecure_frame(zb_buf_t *nsdu)
{
  (void)nsdu;
  return 0;
}

void zb_secur_aps_aux_hdr_fill(zb_uint8_t *p, zb_bool_t nwk_key)
{
  (void)p;
  (void)nwk_key;
}

void zb_zdo_poll_parent(zb_uint8_t param)
{
  (void)param;
}

zb_bool_t zb_aps_command_add_secur(zb_buf_t *buf, zb_uint8_t command_id)
{
  (void)buf;
  (void)command_id;
  return 0;
}

zb_ushort_t zb_aps_secur_aux_size(zb_uint8_t *p)
{
  (void)p;
  return 0;
}

void secur_generate_keys()
{
}


/*! @} */
