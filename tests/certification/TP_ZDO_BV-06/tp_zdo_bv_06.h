/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: 14.12 TP/ZDO/BV-06 ZC-ZDO-Transmit Bind/Unbind_req.
*/

#ifndef TP_ZDO_BV_06_H
#define TP_ZDO_BV_06_H 1

#define TEST_ED1_EP 0x01
#define TEST_ED2_EP 0xF0


#define TEST_IEEE_ADDR_C {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa}

/* For NS build first ieee addr byte should be unique */
#if defined ZB_EMBER_GOLDEN_UNIT
#define TEST_IEEE_ADDR_ED1 {0xF0, 0x23, 0x07, 0x00, 0x00, 0xED, 0x21, 0x00}
#define TEST_IEEE_ADDR_ED2 {0xEE, 0x23, 0x07, 0x00, 0x00, 0xED, 0x21, 0x00}
#elif defined ZB_NS_BUILD
#define TEST_IEEE_ADDR_ED1 {0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define TEST_IEEE_ADDR_ED2 {0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#else
#define TEST_IEEE_ADDR_ED1 {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}
#define TEST_IEEE_ADDR_ED2 {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}
#endif

#define TEST_BUFFER_LEN 0x10

#endif /* TP_ZDO_BV_06_H */
