/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: Test for encryption using test vectors from ZigBee spec
*/
#include "zb_common.h"
#include "zb_secur.h"
#include <string.h>
#include "zb_scheduler.h"
#include "zb_nwk.h"
#include "zb_mac.h"
#include "zb_mac_transport.h"
#include "zb_ubec24xx.h"
#include "zb_mac_globals.h"


#ifdef ZB_SECURITY





MAIN()
{
  zb_ret_t ret;
  zb_uint8_t key[16] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF };
  zb_uint8_t nonce[13] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0x03, 0x02, 0x01, 0x00, 0x06 };
  zb_uint8_t m[23] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E };
  zb_uint8_t a[8] = { 00, 01, 02, 03, 04, 05, 06, 07 };  
  zb_buf_t *buf;
  zb_uint8_t *ptr;
  ARGV_UNUSED;

  ZB_INIT("crypt_test", "1", NULL);
  buf = zb_get_in_buf();
  ZB_BUF_INITIAL_ALLOC(buf, 23+8+8, ptr);
  
  ret = zb_ccm_encrypt_n_auth(8, key, nonce, a, 8,m, 23, buf);

  if (ret == RET_OK)
  {
    ret = zb_ccm_decrypt_n_auth(8, key, nonce, buf, 8, 23 + 8);
  }
  if (ret == RET_OK
      && !ZB_MEMCMP(m, ZB_BUF_BEGIN(buf)+8, 23))
  {
    TRACE_MSG(TRACE_SECUR1, "crypt_test finished OK", (FMT__0));
  }
  else
  {
    TRACE_MSG(TRACE_SECUR1, "crypt_test FAILED ret %d", (FMT__D, ret));
  }
  MAIN_RETURN(ret);
}

/* just to compile... */

void zb_zdo_startup_complete(zb_uint8_t param) ZB_CALLBACK
{
  ZVUNUSED(param);
}

#if 0
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

#else
MAIN()
{
  ARGV_UNUSED;
  MAIN_RETURN(0);
}

#endif  /* ZB_SECURITY */

/*! @} */
