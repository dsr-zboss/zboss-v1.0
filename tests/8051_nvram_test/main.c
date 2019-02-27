#pragma OT(0);
#include "C:\KEIL\C51\INC\CYGNAL\C8051F120.H"
sbit LED1 = P3^0; 	
sbit LED2 = P3^1;
typedef unsigned char      zb_uint8_t;

#define ZB_CODE_MEM ((zb_uint8_t code *)0x00)
#define ZB_XDATA_MEM ((zb_uint8_t xdata *)0x00)

void _init(void)
{
    WDTCN = 0xDE;  
    WDTCN = 0xAD;
    SFRPAGE = 0xf;
    OSCICN = 0x83; 
    while(!(OSCICN & 0x40));
    XBR0 = 0x06;
    XBR1 = 0x19; 
    XBR2 = 0x44; 
    P0MDOUT = 0x34; 
    P1MDOUT = 0xFF;
    P2MDOUT = 0xEF;
    P3MDOUT = 0xFF;
    SFRPAGE = 0x0;

}


void zb_flash_init()
{
  /* for deubg purpose only 
  RSTSRC = 0x02;
  */
}
/* Erasing nvram 
Step 1. Disable interrupts.
Step 2. If erasing a page in Bank 1, Bank 2, or Bank 3, set the COBANK bits (PSBANK.5-4) for 
the appropriate bank.
Step 3. If erasing a page in the Scratchpad area, set the SFLE bit (PSCTL.2).
Step 4. Set FLWE (FLSCL.0) to enable Flash writes/erases via user software.
Step 5. Set PSEE (PSCTL.1) to enable Flash erases.
Step 6. Set PSWE (PSCTL.0) to redirect MOVX commands to write to Flash.
Step 7. Use the MOVX instruction to write a data byte to any location within the page to be 
erased.
Step 8. Clear PSEE to disable Flash erases.
Step 9. Clear the PSWE bit to redirect MOVX commands to the XRAM data space.
Step 10. Clear the FLWE bit, to disable Flash writes/erases.
Step 11. If erasing a page in the Scratchpad area, clear the SFLE bit.
Step 12. Re-enable interrupts. */

void zb_erase_nvram()
{
 
    FLSCL |= 0x01;                      /* enable FLASH write/erase */
    PSCTL |= 0x03;                      /* enable erasing FLASH */
    PSCTL |= 0x04;                      /* redirect erasing FLASH to scratchpad FLASH */

    /* writing anywhere initiates erase of the whole page, scratch pad pages are 128 instead of 256 bytes    */
    //ptr_flash_scratchpad[0x00] = 0x00;      
    ZB_XDATA_MEM[0] = 0x00;
    //ptr_flash_scratchpad[0] = 0x00;          
    ZB_XDATA_MEM[129] = 0x00;
    PSCTL &= ~0x07; /* set PSWE = PSEE = SFLE = 0 to disable all access to scratchpad FLASH in place of xdata*/
    FLSCL &= ~0x01; /* disable FLASH write/erase */  
}

/*  writing nvram
Step 1. Disable interrupts.
Step 2. Clear CHBLKW (CCH0CN.0) to select single-byte write mode.
Step 3. If writing to bytes in Bank 1, Bank 2, or Bank 3, set the COBANK bits (PSBANK.5-4) f
the appropriate bank.
Step 4. If writing to bytes in the Scratchpad area, set the SFLE bit (PSCTL.2).
Step 5. Set FLWE (FLSCL.0) to enable Flash writes/erases via user software.
Step 6. Set PSWE (PSCTL.0) to redirect MOVX commands to write to Flash.
Step 7. Use the MOVX instruction to write a data byte to the desired location (repeat as 
necessary).
Step 8. Clear the PSWE bit to redirect MOVX commands to the XRAM data space.
Step 9. Clear the FLWE bit, to disable Flash writes/erases.
Step 10. If writing to bytes in the Scratchpad area, clear the SFLE bit.
Step 11. Re-enable interrupts. */


void zb_write_nvram(zb_uint8_t pos, zb_uint8_t *buf, zb_uint8_t len )
{ 
    zb_uint8_t data i;
    FLSCL |= 0x01;                      /* enable FLASH write/erase */
    PSCTL |= 0x01;                      /* enable writing to FLASH in place of xdata */
    PSCTL |= 0x04;                      /* enable writing to scratchpad FLASH instead of to FLASH */

    for (i=0;i<len;i++)
    {
      ZB_XDATA_MEM[pos+i]=buf[i];
    }
    
    PSCTL &= ~0x05;                     /* disable writing to scratchpad or regular FLASH in place of xdata */
    FLSCL &= ~0x01;                     /* disable FLASH write/erase */
}

void zb_read_nvram(zb_uint8_t pos, zb_uint8_t *buf, zb_uint8_t len)
{
    zb_uint8_t data i;
    PSCTL |= 0x04;                      /* enable reading from the scratchpad FLASH instead of from FLASH */
    for (i=0; i<len; ++i)
    {
        buf[i]=ZB_CODE_MEM[pos+i];
    }
    PSCTL &= ~0x04;                     /* disable reading from the scratchpad FLASH instead of from FLASH  */

}




typedef struct qwe
{
  int a;
  int b;
  int c;
} test_s ;


void main()
{  
  int i = 0;
  test_s test;

  
  _init();
   WDTCN = 0xde;
   WDTCN = 0xad;
   zb_read_nvram(0, (zb_uint8_t *) &test, sizeof(test));
   if (test.a == 1 && test.b == 2 && test.c == 3)
   {
    LED1 = 0;
   }     
   else 
   {
    test.a = 1;
    test.b = 2;
    test.c = 3;
  
    zb_flash_init();
    zb_erase_nvram();
    zb_write_nvram(0,(zb_uint8_t *) &test, sizeof(test)); 
    LED2 = 0;
   }
 
while (1) {
  zb_flash_init();     
}

}



