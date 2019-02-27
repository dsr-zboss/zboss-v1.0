/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for trivial NWK data transfer
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_ringbuffer.h"
#include "zb_bufpool.h"
#include "zb_mac_transport.h"
#include "zb_nwk.h"
#include "zb_secur.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

#include "zb_bank_common.h"

#if defined FIRST_TEST
  #define MY_ADDR   0x1
  #define PEER_ADDR 0x2
  #define LOG_FILE "nwk_test_first"
#elif defined SECOND_TEST
  #define MY_ADDR   0x2
  #define PEER_ADDR 0x1
  #define LOG_FILE "nwk_test_second"
#else
  #error "Test built without define"
#endif

/**
   \par NWK data test.

   This test sends 80-bytes packet and receives 80-bytes packet.
   Supposed that 2 tests exemplars connected via ns-3.

 */

void func1(zb_uint8_t param1) ZB_CALLBACK
{

  zb_nlde_data_req_t req;
  zb_uint8_t *ptr = NULL;
  zb_short_t i;
  zb_buf_t *nsdu;

  ZVUNUSED(param1);

  req.dst_addr = PEER_ADDR;
  req.radius = 1;
  nsdu = zb_get_out_buf();
  req.addr_mode = ZB_ADDR_16BIT_DEV_OR_BROADCAST;
  req.nonmember_radius = 0;
  req.discovery_route = 0;
  req.security_enable = 0;
  if (!nsdu)
  {
    TRACE_MSG(TRACE_ERROR, "out buf alloc failed!", (FMT__0));
  }
  else
  {
    nsdu->u.hdr.handle = 0x11;
    ZB_BUF_INITIAL_ALLOC(nsdu, 80, ptr);
    for (i = 0 ; i < 80 ; ++i)
    {
      ptr[i] = i % 32 + '0';
    }
    TRACE_MSG(TRACE_NWK3, "Sending nlde_data.request", (FMT__0));

    ZB_MEMCPY(
      ZB_GET_BUF_TAIL(nsdu, sizeof(zb_nlde_data_req_t)),
      &req, sizeof(req));
    ZB_SCHEDULE_CALLBACK(zb_nlde_data_request, ZB_REF_FROM_BUF(nsdu));
  }

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
  int ret = 0;
  zb_short_t i;
  zb_short_t len;
  zb_uint8_t *ptr;
  zb_buf_t *nsdu = (zb_buf_t *)ZB_BUF_FROM_REF(param);

  TRACE_MSG(TRACE_NWK3, "nlde_data_indication: packet %p handle 0x%x", (FMT__P_D,
            nsdu, nsdu->u.hdr.status));

  /* Remove NWK header from the packet */
  ZB_NWK_HDR_CUT(nsdu, ptr);

  len = ZB_BUF_LEN(nsdu);

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

  zb_free_buf(nsdu);
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

  if ( argc < 3 )
  {
    usage(argv);
    return 0;
  }

  zb_init_wo_aps(LOG_FILE, argv[1], argv[2]);

  /* In the real life network address will be assigned at network join; now
   * hardcode it. */
  ZB_NIB_NETWORK_ADDRESS() = MY_ADDR;
  ZG->nwk.handle.joined = ZB_TRUE;

  /* call nwk API from the scheduler loop */
  ZB_SCHEDULE_CALLBACK(func1, 0);

  for (i = 0 ; i < 10 ; ++i)
  {
    zb_sched_loop_iteration();
  }

  TRACE_DEINIT();

  MAIN_RETURN(0);
}


/* just to compile... */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_apsde_data_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_network_discovery_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_network_formation_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_ed_scan_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}

void zb_nlme_start_router_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}

void zb_nlme_route_discovery_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}

void zb_nlme_reset_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_sync_confirm(zb_uint8_t param) ZB_CALLBACK
{
  (void)param;
}

zb_ret_t zb_nwk_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
  return 0;
}

void zb_aps_secure_frame(zb_buf_t *src, zb_uint_t mac_hdr_size, zb_buf_t *dst)
{
  (void)src;
  (void)dst;
  (void)mac_hdr_size;
}

zb_ret_t zb_nwk_unsecure_frame(zb_uint8_t param, zb_mac_mhr_t *mhr, zb_uint8_t mhr_len)
{
  (void)param;
  (void)mhr;
  (void)mhr_len;
  return 0;
}

void secur_generate_keys()
{
}

zb_bool_t secur_has_preconfigured_key()
{
  return ZB_FALSE;
}

void zb_zdo_check_fails(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_nlme_status_indication(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

void zb_apsme_update_device_request(zb_uint8_t param)
{
  ZVUNUSED(param);
}


void zb_nlme_leave_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_leave_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}



/*! @} */
