/***************************************************************************
*                                                                          *
* INSERT COPYRIGHT HERE!                                                   *
*                                                                          *
****************************************************************************
PURPOSE: mac (reset) test;
*/

/*! \addtogroup ZB_TESTS */
/*! @{ */

#include <intrins.h>
#include "zb_common.h"
#include "zb_config.h"
#include "zb_bufpool.h"
#include "zb_ringbuffer.h"
#include "zb_mac_transport.h"
#include "zb_ubec24xx.h"

#define UINT8 unsigned char
#define uint8 UINT8
#define UINT16	unsigned short int
#define bool unsigned char

zb_ret_t zb_ubec_check_int_status();

UINT8 spi_sr(UINT8 Address);
void spi_sw(UINT8 Address, UINT8 Data);
void spi_lw(UINT16 Address, UINT8 Data);
uint8 spi_lr(UINT16 Address);
void spi_lw_block(UINT16 Address, UINT8 *DataPtr, UINT8 Size);
uint8 RF_SECURE_TxN(uint8 *tx_frm, uint8 frm_len, bool ack_req, uint8 non_enc_len, uint8 sec_mode, uint8 *sec_key, uint8 *nonce);

void spi_dump_fifo(UINT16 Address, UINT8 *DataPtr, UINT8 Length);
void spi_fill_fifo(UINT16 Address, UINT8 *DataPtr, UINT8 Length);

void txTest();
sbit LED1 = P3^0;


/* NVRAM TEST FUNC */
zb_ret_t zb_write_nvram_config(zb_uint8_t aps_designated_coord, zb_uint8_t aps_use_insecure_join, zb_uint8_t aps_use_extended_pan_id, 
zb_ieee_addr_t mac_extended_address, zb_node_desc_t node_desc, zb_power_desc_t pwr_desc, zb_simple_desc_t simple_desc);

zb_ret_t zb_write_formdesc_data(zb_uint8_t profile_in_use, zb_ieee_addr_t long_parent_addr, zb_uint32_t aps_channel_mask,
zb_uint16_t short_parent_addr, zb_uint8_t     depth, zb_uint16_t pan_id, zb_ext_pan_id_t ext_pan_id, zb_uint16_t nwk_short_addr);

typedef union _spi_config_union_
{
UINT8 c;
  struct
    {
        UINT8 RXBMT  : 1;
        UINT8 SRMT   : 1;
        UINT8 NSSIN  : 1;
        UINT8 SLVSEL : 1;
        UINT8 CKPOL  : 1;
        UINT8 CKPHA  : 1;
        UINT8 MSTEN  : 1;
        UINT8 SPIBSY : 1;
    }b;
}SPICFG;


zb_uint8_t key[16] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF };
zb_uint8_t nonce[13] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0x03, 0x02, 0x01, 0x00, 0x06 };
zb_uint8_t m[31] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E };
zb_uint8_t a[8] = { 00, 01, 02, 03, 04, 05, 06, 07 };
  

#pragma OT(0);
MAIN()
{
  uint8 ret = 11; 
  uint8 buf[50];
  uint8 addr[8]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
  uint8 addr2[8]={0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
  uint8 ext_pan_id[8]={0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
  zb_node_desc_t q;
  zb_power_desc_t w;
  zb_simple_desc_t e;


  
    
  FAKE_ARGV;
  ARGV_UNUSED;
  ZB_STOP_WATCHDOG();
  zb_init_wo_aps("qwe",0,6);
//  zb_sched_init();
//  zb_spi_init();
//  zb_xram_init();

  memset(buf,0,50);

  
  zb_write_nvram_config(1, 1, 1, addr, q, w, e);
/*  
  spi_lw(0x24d, spi_lr(0x24d)|0x20);
  ret = RF_SECURE_TxN(m, 31, 0, 8, 0, key, nonce);
  spi_dump_fifo(0x00, buf, 50);  
  
  spi_sw(0x1b, 0x01);*/
 zb_config_from_nvram();

zb_write_formdesc_data(12, addr2, 0xBBBBBBBB,0x4321, 3, 0x8051, ext_pan_id, 0x8052);  
zb_read_formdesc_data();

ZG->nwk.nib.outgoing_frame_counter = 0x12345678;
zb_write_up_counter();
zb_read_up_counter();

zb_write_security_key();
zb_read_security_key();

 

  ZB_GO_IDLE();
  }


uint8 RF_SECURE_TxN(uint8 *tx_frm, uint8 frm_len, bool ack_req, uint8 non_enc_len, uint8 sec_mode, 
uint8 *sec_key, uint8 *nonce)  
{ 
  uint8 val; 
 
   
  /* load the length which user doesn't want to encrypt */ 
  spi_lw(0x0, non_enc_len);   
  /* load frame length */ 
  spi_lw(0x1, frm_len);   
  /* load frame */ 
  spi_lw_block(0x2, tx_frm, frm_len);  
  /* load nonce */ 
  spi_lw_block(0x240, nonce, 13);    
   
  /* load security key */ 
  spi_lw_block(0x280, sec_key, 16);       
  /* Fill encryption mode for TxN FIFO */ 
  spi_sw(0x2c, (spi_sr(0x2c)|0x03));  //ENC_MIC_32
  //spi_sw(0x37, spi_sr(0x37)|0x40);
  
  
  val = spi_sr(0x1b); 
  if(ack_req) //If need wait Ack? 
  val |= 0x07;   
    else 
    val = (val&~0x04)|0x03; 
 
  /* trigger TxN */ 
  spi_sw(0x1b, val);   
   
/* wait TxN interrupt */ 
  while(1)
  {
    if (spi_sr(0x31)&0x01==0x01) break;
  }
  
   
  /* check TxN result */ 
  if(!(spi_sr(0x24)&0x01))   
    return 0; 
  return 1;   
} 
 
 




void spi_fill_fifo(UINT16 Address, UINT8 *DataPtr, UINT8 Length)
{
  Address = (Address << 5) | 0x8010;

//Set up the SFRPAGE
  NSSMD0 = 0; //Put the CS to Low

  SPI0DAT = ((UINT8*)&Address)[0]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = ((UINT8*)&Address)[1]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  while (Length--){
      SPI0DAT = *DataPtr++; //Write the data
      while(!SPIF);
      SPIF = 0; //clear the flag
  }

  NSSMD0 = 1; //Put the CS to High
  _nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  		_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  		_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
}

UINT8 spi_sr(UINT8 Address)
{
  UINT8 ret = 0;
  Address = (Address << 1);
 
  NSSMD0 = 0; //Put the CS to Low

  SPI0DAT = Address; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = 0; //Write the data (For generating the SCLK)
  while(!SPIF);
  ret = SPI0DAT;
  SPIF = 0; //clear the flag
  return ret;
  //NSSMD0 = 1; //Put the CS to High
  
}
void spi_sw(UINT8 Address, UINT8 DataPtr)
{
  Address = (Address << 1) | 0x0001; //Shfit the Address

//Set up the SFRPAGE
  NSSMD0 = 0; //Put the CS to Low

  SPI0DAT = Address; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = DataPtr; //Write the data
  while(!SPIF);
  SPIF = 0; //clear the flag

  NSSMD0 = 1; //Put the CS to High

  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();

}



void spi_lw_block(UINT16 Address, UINT8 *DataPtr, UINT8 Size)
{
  int i;
  for (i=0;i<Size;i++)
  {
    spi_lw(Address+i, DataPtr[i]);
  }

}


void spi_lw(UINT16 Address, UINT8 DataPtr)
{

  Address = (Address << 5) | 0x8010;

//Set up the SFRPAGE
  NSSMD0 = 0; //Put the CS to Low

  SPI0DAT = ((UINT8*)&Address)[0]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = ((UINT8*)&Address)[1]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = DataPtr; //Write the data
  while(!(SPI0CN & 0x80));
  SPIF = 0; //clear the flag

  NSSMD0 = 1; //Put the CS to High

  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();


}

uint8 spi_lr(UINT16 Address)
{
  uint8 ret = 0;
  Address = (Address << 5) | 0x8000;


//Set up the SFRPAGE
  NSSMD0 = 0; //Put the CS to Low

  SPI0DAT = ((UINT8*)&Address)[0]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = ((UINT8*)&Address)[1]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = 0; //Write the NULL data
  while(!SPIF);
  ret = SPI0DAT;
  SPIF = 0; //clear the flag

  NSSMD0 = 1; //Put the CS to High

  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();

  return ret;
}

void spi_dump_fifo(UINT16 Address, UINT8 *DataPtr, UINT8 Length)
{

  Address = (Address << 5) | 0x8000;

//Set up the SFRPAGE
  NSSMD0 = 0; //Put the CS to Low

  SPI0DAT = ((UINT8*)&Address)[0]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  SPI0DAT = ((UINT8*)&Address)[1]; //Write the address
  while(!SPIF);
  SPIF = 0; //clear the flag

  while(Length--){
      SPI0DAT = 0; //Write the NULL data
      while(!SPIF);
      *DataPtr++ = SPI0DAT;
      SPIF = 0; //clear the flag
  }

  NSSMD0 = 1; //Put the CS to High

  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
  			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();


}





/*! @} */
