/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: nwkAddressMap routines test.
*/

#include "zb_common.h"
#include "zb_nwk.h"
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
  zb_address_pan_id_ref_t my_pan_ref;
  zb_address_ieee_ref_t addr_ref;
  zb_ext_pan_id_t pan_addr;
  zb_ext_pan_id_t my_pan_addr;
  zb_ieee_addr_t ieee_addr;
  zb_uint_t i;
  zb_ret_t ret = RET_OK;
  ARGV_UNUSED;

  ZB_INIT("nwk_ext_neighbor_test", NULL, NULL);

  TRACE_MSG(TRACE_INFO2, "started", (FMT__0));

  /* 1. fill base neighbor table: need to test its pack and its part eat.
     Add 7 panids.
   */

  /* a) fill panid table */
  for (i = 0 ; i < ZB_PANID_TABLE_SIZE && ret == RET_OK ; ++i)
  {
    gen_64_bit_addr(pan_addr, i+1, i+1);
    ret = zb_address_set_pan_id(0, pan_addr, &pan_ref);
  }

  if (ret == RET_OK)
  {
    /* Remember "my" pan id - means, pan id which will be when join complete (no
     * join in this test - just imitate it). */
    gen_64_bit_addr(my_pan_addr, 2, 2);
    ret = zb_address_set_pan_id(0, my_pan_addr, &my_pan_ref);
    if (ret == RET_ALREADY_EXISTS)
    {
      ret = RET_OK;
    }
  }

  /* fill base neighbor table */
  for (i = 0 ; ret == RET_OK && i < ZG->nwk.neighbor.base_neighbor_size / 3 ; ++i)
  {
    /* we are interesting in addr field only */
    ZG->nwk.neighbor.base_neighbor[i].used = 1;
    gen_64_bit_addr(ieee_addr, i+1, i+2);
    /* test all address types */
    switch (i % 3)
    {
      case 0:
        ret = zb_address_update(ieee_addr, SHORT_BY_IDX(i,i+1), ZB_TRUE, &addr_ref);
        break;
      case 1:
        ret = zb_address_by_ieee(ieee_addr, ZB_TRUE, ZB_TRUE, &addr_ref);
        break;
      case 2:
        ret = zb_address_by_short(SHORT_BY_IDX(i,i+1), ZB_TRUE, ZB_TRUE, &addr_ref);
        break;
    }
    ZG->nwk.neighbor.base_neighbor[i].addr_ref = addr_ref;
    ZG->nwk.neighbor.addr_to_neighbor[addr_ref] = i;
    ZG->nwk.neighbor.base_neighbor_used = i+1;
  }

  if (ret == RET_OK)
  {

    /* 2. call zb_nwk_exneighbor_start() check 1/2 base neighbor table cut */
    zb_nwk_exneighbor_start();

    /* 3. call zb_nwk_exneighbor_start() next time: must do nothing */
    zb_nwk_exneighbor_start();

    /* 4. call zb_nwk_exneighbor_stop(): check space put to the base tabe without
     * any data transfer */

    zb_nwk_exneighbor_stop(-1);

    /* 5. call zb_nwk_exneighbor_stop(): check for already stopped ext neighbor */

    zb_nwk_exneighbor_stop(-1);
  }

  if (ret == RET_OK)
  {
    /* 6. fill base neighbor to be filled more then 1/2 */
    for ( ; ret == RET_OK && i < ZG->nwk.neighbor.base_neighbor_size / 2 + 3 ; ++i)
    {
      /* we are interesting in addr field only */
      ZG->nwk.neighbor.base_neighbor[i].used = 1;
      gen_64_bit_addr(ieee_addr, i+1, i+2);
      /* test all address types */
      switch (i % 3)
      {
        case 0:
          ret = zb_address_update(ieee_addr, SHORT_BY_IDX(i,i+1), ZB_TRUE, &addr_ref);
          break;
        case 1:
          ret = zb_address_by_ieee(ieee_addr, ZB_TRUE, ZB_TRUE, &addr_ref);
          break;
        case 2:
          ret = zb_address_by_short(SHORT_BY_IDX(i,i+1), ZB_TRUE, ZB_TRUE, &addr_ref);
          break;
      }
      ZG->nwk.neighbor.base_neighbor[i].addr_ref = addr_ref;
      ZG->nwk.neighbor.base_neighbor[i].device_type = ZB_NWK_DEVICE_TYPE_NONE;
      ZG->nwk.neighbor.addr_to_neighbor[addr_ref] = i;
      ZG->nwk.neighbor.base_neighbor_used = i+1;
    }
  }

  if (ret == RET_OK)
  {
    /* 7. call zb_nwk_exneighbor_start(), check < 1/2 base neighbor cut */

    zb_nwk_exneighbor_start();

    /* 8. in a loop call zb_nwk_exneighbor_by_short and zb_nwk_exneighbor_by_ieee
     * with different addresses:
     - different pan id ref
     - different addresses. Must test address dups as well

     Exit loop when got error
   */
  }
  for (i = 0 ; ret == RET_OK ; ++i)
  {
    zb_ext_neighbor_tbl_ent_t *e;
    if (i % 2)
    {
      ret = zb_nwk_exneighbor_by_short(i % ZB_PANID_TABLE_SIZE, SHORT_BY_IDX(i,i+1), &e);
    }
    else
    {
      gen_64_bit_addr(ieee_addr, i+1, i+2);
      ret = zb_nwk_exneighbor_by_ieee(i % ZB_PANID_TABLE_SIZE, ieee_addr, &e);
    }
    if (ret == RET_OK)
    {
      e->device_type = i % 3;
      e->lqi = i % 7;
    }
  }

  if (ret == RET_NO_MEMORY)
  {
    ret = RET_OK;
  }
  if (ret == RET_OK)
  {
  /* 9. Set ZG->nwk.handle.joined flag, assign ext pan id */
    ZG->nwk.handle.joined = 1;
    ZB_MEMCPY(ZB_NIB_EXT_PAN_ID(), my_pan_addr, sizeof(my_pan_addr));

  /* 10. call zb_nwk_exneighbor_stop. Check ext neighbor table records migration. */
    zb_nwk_exneighbor_stop(-1);
  }

  if (ret == RET_OK)
  {
    TRACE_MSG(TRACE_INFO2, "SUCCESS", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_INFO2, "ERROR %d", (FMT__D, ret));
  }
  TRACE_DEINIT();
  MAIN_RETURN(ret);
}


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

void zb_nlme_join_indication(zb_uint8_t param)
{
  ZVUNUSED(param);
}

void zb_nlme_join_confirm(zb_uint8_t param)
{
  ZVUNUSED(param);
}
#endif

/*! @} */
