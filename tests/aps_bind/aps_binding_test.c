/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for APS binding
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_address.h"
#include "zb_aps.h"


/*! \addtogroup ZB_TESTS */
/*! @{ */

static void gen_64_bit_addr(zb_64bit_addr_t addr, zb_int_t hi, zb_int_t low)
{
  zb_short_t i;
  for (i = 0 ; i < 3 ; ++i)
  {
    addr[i] = hi % ZB_DEV_MANUFACTORER_TABLE_SIZE;
  }
  for ( ; i < 8 ; ++i)
  {
    addr[i] = low;
  }

}

#define SHORT_BY_IDX(i,j) ((j)*20 + (i) + ((i) % 2 ? 1000 :0))

MAIN()
{
  zb_address_ieee_ref_t addr_ref;
  zb_ieee_addr_t        ieee_addr;
  zb_int_t i;
  zb_apsme_binding_req_t req_b;
  zb_buf_t *aps;
#if 0
  zb_apsme_group_req_t   req_g;
  zb_buf_t *grp;
#endif

#ifdef ZB_PLATFORM_LINUX_PC32
  zb_ret_t ret = RET_OK;
  ARGV_UNUSED;

  ZB_INIT("binding_test", NULL, NULL);

#if 0  /* APS Group management not supported */
  zb_group_init();
#endif /* APS Group management not supported */

  aps = zb_get_in_buf();
#if 0  /* APS Group management not supported */
  grp = zb_get_in_buf();
#endif /* APS Group management not supported */

  /* addr_add */
  for (i = 0 ;(i < ZB_IEEE_ADDR_TABLE_SIZE || ret == RET_OK); ++i)
  {
    gen_64_bit_addr(ieee_addr, i+1, i+1);
    ret = zb_address_update(ieee_addr, SHORT_BY_IDX(i,i+1), ZB_TRUE, &addr_ref);
    TRACE_MSG(TRACE_NWK3, "addr_add #%d ret %d ref %d", (FMT__D_D_D, i, ret, addr_ref));
  }

  /* aps bind test */
  TRACE_MSG(TRACE_APS1, "start APS bind test", (FMT__0));
  for (i = 0 ;(i < ZB_APS_SRC_BINDING_TABLE_SIZE || ret == RET_OK); ++i)
  {
    gen_64_bit_addr(ieee_addr, i+1, i+1);

    ZB_MEMCPY(&req_b.src_addr, &ieee_addr, sizeof(zb_ieee_addr_t));
    req_b.src_endpoint = i;
    req_b.clusterid = 1;
    req_b.addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    gen_64_bit_addr(ieee_addr, i+2, i+2);
    ZB_MEMCPY(&req_b.dst_addr.addr_long, &ieee_addr, sizeof(zb_ieee_addr_t));
    req_b.dst_endpoint = i+1;

    ZB_MEMCPY(
              ZB_GET_BUF_TAIL(aps, sizeof(zb_apsme_binding_req_t)),
              &req_b,
              sizeof(zb_apsme_binding_req_t));

    zb_apsme_bind_request(ZB_REF_FROM_BUF(aps));
    ret = aps->u.hdr.status;
  }
  if (ret)
  {
    if (ret == RET_TABLE_FULL)
    {
      TRACE_MSG(TRACE_APS1, "binding table is FULL %d for i %d", (FMT__D_D, ret, i));
      TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
    }
    else
    {
      TRACE_MSG(TRACE_APS1, "ERROR %d", (FMT__D, ret));
    }
  }

  /* aps unbind test */
  TRACE_MSG(TRACE_APS1, "start APS unbind test", (FMT__0));
  for (i = 1 ;(i < ZB_APS_SRC_BINDING_TABLE_SIZE || ret == RET_OK); ++i)
  {
    gen_64_bit_addr(ieee_addr, i+1, i+1);

    ZB_MEMCPY(&req_b.src_addr, &ieee_addr, sizeof(zb_ieee_addr_t));
    req_b.src_endpoint = i;
    req_b.clusterid = 1;
    req_b.addr_mode = ZB_APS_ADDR_MODE_64_ENDP_PRESENT;
    gen_64_bit_addr(ieee_addr, i+2, i+2);
    ZB_MEMCPY(&req_b.dst_addr.addr_long, &ieee_addr, sizeof(zb_ieee_addr_t));
    req_b.dst_endpoint = i+1;

    ZB_MEMCPY(
              ZB_GET_BUF_TAIL(aps, sizeof(zb_apsme_binding_req_t)),
              &req_b,
              sizeof(zb_apsme_binding_req_t));
    zb_apsme_unbind_request(ZB_REF_FROM_BUF(aps));
    ret = aps->u.hdr.status;
  }
  if (ret)
  {
    if (ret == RET_INVALID_BINDING)
    {
      TRACE_MSG(TRACE_APS1, "invalid binding for i = %d ret %d", (FMT__D_D, i, ret));
      if (ZG->aps.binding.src_n_elements == 1 && ZG->aps.binding.dst_n_elements == 1)
      {
        TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
      }
      else
      {
        TRACE_MSG(TRACE_APS1, "ERROR binding table is corrupted", (FMT__0));
      }
    }
    else
    {
      TRACE_MSG(TRACE_APS1, "ERROR %d", (FMT__D, ret));
    }
  }

  zb_free_buf(aps);

#if 0  /* APS Group management not supported */
  /* aps add_group test */
  TRACE_MSG(TRACE_APS1, "start APS add_group test", (FMT__0));
  for (i = 0 ;(i < ZB_APS_BINDING_TABLE_SIZE || ret == RET_OK); ++i)
  {
    req_g.group_addr = SHORT_BY_IDX(i,i+1);
    req_g.endpoint = i+1;

    ZB_MEMCPY(
              ZB_GET_BUF_TAIL(grp, sizeof(zb_apsme_group_req_t)),
              &req_g,
              sizeof(zb_apsme_group_req_t));
    zb_apsme_add_group_request( grp);
    ret = grp->u.hdr.status;
  }
  if (ret)
  {
    TRACE_MSG(TRACE_APS1, "ERROR %d", (FMT__D, ret));
  }
  else
  {
    TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
  }


/* aps remove_group test */
  TRACE_MSG(TRACE_APS1, "start APS remove_group test", (FMT__0));
  for (i = 0 ;(i < ZB_APS_BINDING_TABLE_SIZE || ret == RET_OK); ++i)
  {
    req_g.group_addr = SHORT_BY_IDX(i,i+1);
    req_g.endpoint = i+1;

    ZB_MEMCPY(
              ZB_GET_BUF_TAIL(grp, sizeof(zb_apsme_group_req_t)),
              &req_g,
              sizeof(zb_apsme_group_req_t));
    zb_apsme_remove_group_request( grp);
    ret = grp->u.hdr.status;
  }
  if (ret)
  {
    TRACE_MSG(TRACE_APS1, "ERROR %d", (FMT__D, ret));
  }
  else
  {
    TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
  }

/* aps remove_all_group test */
  TRACE_MSG(TRACE_APS1, "start APS remove_all_group test", (FMT__0));
  for (i = 0 ;(i < ZB_APS_BINDING_TABLE_SIZE || ret == RET_OK); ++i)
  {
    req_g.group_addr = SHORT_BY_IDX(i,i+1);
    req_g.endpoint = i+1;

    ZB_MEMCPY(
              ZB_GET_BUF_TAIL(grp, sizeof(zb_apsme_group_req_t)),
              &req_g,
              sizeof(zb_apsme_group_req_t));
    zb_apsme_remove_all_group_request( grp);
    ret = grp->u.hdr.status;
  }
  if (ret)
  {
    TRACE_MSG(TRACE_APS1, "ERROR %d", (FMT__D, ret));
  }
  else
  {
    TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
  }
  zb_free_buf( grp);
#endif /* APS Group management not supported */

  TRACE_DEINIT();
#endif
  MAIN_RETURN(0);
}

void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
}


#if 0
/* just to compile... */
void zb_apsde_data_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_apsde_data_indication(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_network_discovery_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_network_formation_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  (void)param;
}

void zb_nlme_join_indication(zb_uint8_t param)
{
  (void)param;
}
#endif

/*! @} */
