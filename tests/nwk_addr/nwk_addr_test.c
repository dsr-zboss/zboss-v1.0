/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: nwkAddressMap routines test.
*/

#include "zb_common.h"
#include "zb_scheduler.h"
#include "zb_address.h"

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
  zb_address_pan_id_ref_t pan_ref;
  zb_address_ieee_ref_t addr_ref;
  zb_ext_pan_id_t pan_addr;
  zb_ieee_addr_t ieee_addr;
  zb_uint16_t short_addr;
  zb_int_t i;
  zb_int_t j;
  zb_ret_t ret = RET_OK;
  ARGV_UNUSED;

  ZB_INIT("nwk_addr_test", NULL, NULL);
  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));


  /* Test pan id */
  for (i = 0 ; i < ZB_PANID_TABLE_SIZE && ret == RET_OK ; ++i)
  {
    gen_64_bit_addr(pan_addr, i+1, i+1);
    ret = zb_address_set_pan_id(0, pan_addr, &pan_ref);
    TRACE_MSG(TRACE_NWK3, "set_pan_id #%d ret %d ref %d", (FMT__D_D_D, i, ret, pan_ref));
    if (ret == RET_OK)
    {
      zb_address_get_pan_id(pan_ref, pan_addr);
      TRACE_MSG(TRACE_NWK3, "i %d got pan id " TRACE_FORMAT_64, (FMT__D_A, i, TRACE_ARG_64(pan_addr)));
    }
  }
  if (ret != RET_OK)
  {
    TRACE_MSG(TRACE_NWK1, "ERROR %d!", (FMT__D, ret));
  }
  else
  {
    gen_64_bit_addr(pan_addr, i+1, i+1);
    ret = zb_address_set_pan_id(0, pan_addr, &pan_ref);
    if (ret != RET_OK)
    {
      ret = RET_OK;
    }
    else
    {
      TRACE_MSG(TRACE_NWK1, "did not get ERROR when supposed!", (FMT__0));
    }
  }

  for (j = 0 ; j < 6 ; ++j)
  {
    for (i = 0 ; i < 5 && ret == RET_OK ; ++i)
    {
      gen_64_bit_addr(ieee_addr, i+1 + j*20, i+1 + j*20);
      ret = zb_address_update(ieee_addr, SHORT_BY_IDX(i,j), ZB_TRUE, &addr_ref);
      if (ret == RET_OK)
      {
        zb_address_by_ref(ieee_addr, &short_addr, addr_ref);
        TRACE_MSG(TRACE_NWK3, "i %d addr upd/get by ref %d ieee " TRACE_FORMAT_64 " short %x", (FMT__D_D_A_D,
                  i, addr_ref, TRACE_ARG_64(ieee_addr), short_addr));
      }
      else
      {
        TRACE_MSG(TRACE_NWK1, "error %d!", (FMT__D, ret));
      }
    }

    TRACE_MSG(TRACE_NWK3, "now create address by ieee", (FMT__0));
    for (; i < 10 && ret == RET_OK ; ++i)
    {
      gen_64_bit_addr(ieee_addr, i+1 + j*20, i+1 + j*20);
      ret = zb_address_by_ieee(ieee_addr, ZB_TRUE, ZB_TRUE, &addr_ref);
      if (ret == RET_OK)
      {
        zb_address_by_ref(ieee_addr, &short_addr, addr_ref);
        TRACE_MSG(TRACE_NWK3, "i %d addr upd/get by ref %d ieee " TRACE_FORMAT_64 " short %x", (FMT__D_D_A_D,
                  i, addr_ref, TRACE_ARG_64(ieee_addr), short_addr));
        ret = zb_address_update(ieee_addr, SHORT_BY_IDX(i,j), ZB_FALSE, &addr_ref);
      }
      else
      {
        TRACE_MSG(TRACE_NWK1, "error %d!", (FMT__D, ret));
      }
      if (ret == RET_OK)
      {
        zb_address_by_ref(ieee_addr, &short_addr, addr_ref);
        TRACE_MSG(TRACE_NWK3, "i %d addr upd/get by ref %d ieee " TRACE_FORMAT_64 " short %x", (FMT__D_D_A_D,
                  i, addr_ref, TRACE_ARG_64(ieee_addr), short_addr));
      }
      else
      {
        TRACE_MSG(TRACE_NWK1, "error %d!", (FMT__D, ret));
      }
    }

    TRACE_MSG(TRACE_NWK3, "now create address by short", (FMT__0));
    for (; i < 20 && ret == RET_OK ; ++i)
    {
      ret = zb_address_by_short(SHORT_BY_IDX(i,j), ZB_TRUE, ZB_TRUE, &addr_ref);
      if (ret == RET_OK)
      {
        zb_address_by_ref(ieee_addr, &short_addr, addr_ref);
        TRACE_MSG(TRACE_NWK3, "i %d addr upd/get by ref %d ieee " TRACE_FORMAT_64 " short %x", (FMT__D_D_A_D,
                  i, addr_ref, TRACE_ARG_64(ieee_addr), short_addr));
        gen_64_bit_addr(ieee_addr, i+1 + j*20, i+1 + j*20);
        ret = zb_address_update(ieee_addr, SHORT_BY_IDX(i,j), ZB_FALSE, &addr_ref);
      }
      else
      {
        TRACE_MSG(TRACE_NWK1, "error %d!", (FMT__D, ret));
      }
      if (ret == RET_OK)
      {
        zb_address_by_ref(ieee_addr, &short_addr, addr_ref);
        TRACE_MSG(TRACE_NWK3, "i %d addr upd/get by ref %d ieee " TRACE_FORMAT_64 " short %x", (FMT__D_D_A_D,
                  i, addr_ref, TRACE_ARG_64(ieee_addr), short_addr));
      }
      else
      {
        TRACE_MSG(TRACE_NWK1, "error %d!", (FMT__D, ret));
      }
    }
    for (i = 0 ; i < 20 && ret == RET_OK ; ++i)
    {
      if (j && i % j == 0)
      {
        zb_address_unlock(i);
      }
      else
      {
        zb_address_lock(i);
      }
    }
  }

  /* TODO: add self-check */

  TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
  TRACE_DEINIT();
  MAIN_RETURN(0);
}


/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param)
{
  (void)param;
}

#if 0

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

void zb_nlme_join_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}
#endif

/*! @} */
