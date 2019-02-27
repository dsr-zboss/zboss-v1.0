//	zb_ubec_mrf_common.c
// 	Basic lowlevel spi-api for both ubec and mrf radio. Only for Cortex.
#if defined cortexm4 && ZB_MRF24J40

#include "zb_common.h"
#include "zb_mac.h"
#include "zb_mac_transport.h"
#include "zb_mrf24j40.h"
#include "zb_g_context.h"
#include "zb_trace.h"
#include "zb_cortexm4_usart.h"
#ifdef CHECK_RADIO_REGS
#define DECLARE_VAR() volatile uint8_t s = 0
#define CHECK_S(reg) (s = ZB_READ_SHORT_REG(reg))
#define CHECK_L(reg) (s = ZB_READ_LONG_REG(reg))
#else
#define DECLARE_VAR()
#define CHECK_S(reg)
#define CHECK_L(reg)
#endif
	// includes mrf-specific api-functions as defines
	// select/deselect radio
	// channel selection
	// reading and writing registers and fifos
	// initialization radio.
//#include "zb_mac_transport.h"
	// has some unuseable functions =)

/*
  reads short register into byte_value variable
*/
zb_uint8_t spi_sync_exchange(zb_uint8_t tx_data) {
    // Wait until it's 1, so we can write in
    while ((ZB_SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
        ;
    ZB_SPIx->DR = tx_data;
    // wait until it's 1, so we can read out
    while ((ZB_SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
        ;
    return ZB_SPIx->DR;
}

zb_uint8_t read_short_reg(zb_uint8_t addr)
{
	zb_uint8_t result;
	select_radio();
	MRF_EXCHANGE( (addr << 1)&0x7E );
	result = MRF_EXCHANGE(0x00);
	deselect_radio();
	return result;
}

void write_short_reg(zb_uint8_t addr,zb_uint8_t tx_data)
{
	select_radio();
	MRF_EXCHANGE( ((addr << 1) & 0x7E) | 0x01 );
	MRF_EXCHANGE(tx_data);
	deselect_radio();
}

zb_uint8_t read_long_reg(zb_uint16_t addr)
{
	zb_uint8_t result;
	select_radio();
	zb_uint8_t high_byte = addr >> 3;
	zb_uint8_t low_byte = addr << 5;
	MRF_EXCHANGE( (high_byte | 0x80) );
	MRF_EXCHANGE( low_byte & 0xE0);
	result = MRF_EXCHANGE(0x00);
	deselect_radio();
	return result;
}

void write_long_reg(zb_uint16_t addr, zb_uint8_t tx_data)
{
	select_radio();
	zb_uint8_t high_byte = addr >> 3;
	zb_uint8_t low_byte = addr << 5;
	MRF_EXCHANGE( (high_byte | 0x80) );
	MRF_EXCHANGE( (low_byte  & 0xE0) | 0x10 );
	MRF_EXCHANGE(tx_data);
	deselect_radio();
}


zb_uint8_t zb_mac_short_read_reg(zb_uint8_t short_addr)
{
	SPI_CTX().tx_buf[0] = ((short_addr << 1) & 0x007E);
	SPI_CTX().tx_buf[1] = 0x00;
	SPI_CTX().to_tr = 2;
	SPI_CTX().to_rv = 2;
	SELECT_RADIO();
	SPI_EXCH();
	DESELECT_RADIO();
	return SPI_CTX().rx_buf[1];	//return second argument.
}


/*
  reads long register into byte_value variable
*/
zb_uint8_t zb_mac_long_read_reg(zb_uint16_t long_addr)
{
	zb_uint8_t high_byte = long_addr >> 3;
	zb_uint8_t low_byte = long_addr << 5;
	SPI_CTX().tx_buf[0] = (( (high_byte & 0x7F) | 0x80) );
	SPI_CTX().tx_buf[1] = (( (low_byte  & 0xE0) ));
	SPI_CTX().tx_buf[2] = 0x00;
	SPI_CTX().to_tr = 3;
	SPI_CTX().to_rv = 3;
	SELECT_RADIO();
	SPI_EXCH();
	DESELECT_RADIO();
	return SPI_CTX().rx_buf[2];		// return 3rd byte, cause we sent 3 bytes via SPI.
}

/*
  writes short register, value byte_value variable; register addr is
  stored in short_addr
  @return RET_BLOCKED or RET_OK
*/
void zb_mac_short_write_reg(zb_uint8_t short_addr, zb_uint8_t byte_value)
{
	SPI_CTX().tx_buf[0] = ((short_addr <<1) & 0x007F) | 0x01;
	SPI_CTX().tx_buf[1] = byte_value;
	SPI_CTX().to_tr = 2;
	SPI_CTX().to_rv = 0;
	SELECT_RADIO();
	SPI_EXCH();
	DESELECT_RADIO();
}


/*
  writes long register
  10-bit command in 16-bit frame.
  + 8 bits data then
*/
void zb_mac_long_write_reg(zb_uint16_t long_addr, zb_uint8_t byte_value)
{
	zb_uint8_t high_byte = long_addr >> 3;
	zb_uint8_t low_byte = long_addr << 5;
	SPI_CTX().tx_buf[0] = ( ((high_byte & 0x7F) | 0x80) );
	SPI_CTX().tx_buf[1] = ( ((low_byte  & 0xE0) | 0x10) );
	SPI_CTX().tx_buf[2] = byte_value;
	SPI_CTX().to_tr = 3;
	SPI_CTX().to_rv = 0;
	SELECT_RADIO();
	SPI_EXCH();
	DESELECT_RADIO();
}


/**
 * Writes packet to fifo buffer. Packet should completely formated and
 * written to MAC_CTX().operation_buf. To send send packet via radio use
 * macro ZB_START_FIFO_TX(). To find out if TX finish, wait interrupt
 * UBEC_2400_INTER_NUMBER and check interrupt status ZB_SREG_ISRSTS
 * DS-2400 4.3. Typical TX Operations
 */
void zb_mrf_fifo_write(zb_uint16_t long_addr, zb_buf_t *buf)
{
	int i;
	zb_uint8_t *ptr = ZB_BUF_BEGIN(buf);
	TRACE_MSG(TRACE_MAC2, ">> zb_mrf24_fifo_write buf %hd len %hd", (FMT__H_H, 		ZB_REF_FROM_BUF(buf), ZB_BUF_LEN(buf)));

	for (i = long_addr; i<ZB_BUF_LEN(buf); i++)
	{
		ZB_WRITE_LONG_REG(i,*ptr);
		ptr++;
	}

	zb_buf_cut_left(buf, sizeof(zb_mrf_fifo_header_t));

	TRACE_MSG( TRACE_MAC2, "<< zb_mrf24_fifo_write, l %hd", (FMT__H_D, ZB_BUF_LEN(buf)));
}


// tx_fifo == 1 for NORMAL TX FIFO and ==0 for RX FIFO reading
void zb_mrf_fifo_read(zb_uint8_t tx_fifo, zb_buf_t *buf, zb_uint8_t len)
{
	zb_uint16_t fifo_addr;
	zb_uint16_t i;
	zb_uint8_t *ptr = 0;
	//zb_uint8_t reg = 0;
	// len == 0 -> we must read from RXFIFO.
	// 1. read first byte with a length.
	// 2. read packet
	// 3. do something with rssi and lqi payload.

	TRACE_MSG(TRACE_MAC2, ">> zb_mrf_fifo_read", (FMT__0));
	ZB_DISABLE_TRANSIVER_INT();

  /* Note: When the first byte of the RXFIFO is read, the MRF24J40 is ready to receive the next
     packet. To avoid receiving a packet while the RXFIFO is being read, set the Receive
     Decode Inversion (RXDECINV) bit (0x39<2>) to '1' to disable the MRF24J40
     from receiving a packet off the air. Once the data is read from the RXFIFO, the
     RXDECINV should be cleared to '0' to enable packet reception

     See DISABLE_AIR() and ENABLE_AIR() macro usage.
 */

	if (tx_fifo)	// read txfifo
	{
    fifo_addr = ZB_NORMAL_TX_FIFO;
  }
	else
  {
    fifo_addr = ZB_RX_FIFO;
    DISABLE_AIR();

    //disable the ACKs until the buffer is cleared
    ZB_WRITE_SHORT_REG(ZB_SREG_RXMCR,0x20);

    if (len == 0)
    {
      /* if len == 0, this is read from rx fifo. Must first read packet length
       * then packet body */
      len = ZB_READ_LONG_REG(ZB_RX_FIFO);
      len += ZB_MAC_PACKET_LENGTH_SIZE + ZB_MAC_EXTRA_DATA_SIZE;
      /* really read packet length twice to simplify traffic analyzed logic */
    }
  }

  if (fifo_addr == ZB_RX_FIFO)
  {
  }

	zb_buf_initial_alloc(buf, len);
	ptr = ZB_BUF_BEGIN(buf);
	//ZB_BZERO(ptr,len);
	for (i = 0; i<len; i++)
	{
		*ptr = ZB_READ_LONG_REG(fifo_addr);
		ptr++;
		fifo_addr++;
	}

//	if (!tx_fifo)
	{
		/* reg = ZB_READ_SHORT_REG(ZB_SREG_RXFLUSH); */
		ZB_WRITE_SHORT_REG(ZB_SREG_RXFLUSH, 0x01);
                /* ZB_WRITE_SHORT_REG(ZB_SREG_RXFLUSH,reg | 1L); */
		//while (!((reg = ZB_READ_SHORT_REG(ZB_SREG_RXFLUSH)) & 0x01))
		//	;
    //enable the ACKs
    ZB_WRITE_SHORT_REG(ZB_SREG_RXMCR,0x00);
		ENABLE_AIR();
	}
	ZB_ENABLE_TRANSIVER_INT();
	TRACE_MSG(TRACE_MAC2, "<< zb_mrf_fifo_read %i", (FMT__0));
}

void zb_read_rx_fifo(zb_buf_t *buf)
{	
	TRACE_MSG(TRACE_MAC2, ">> zb_read_rx_fifo ", (FMT__0));
	ZB_DISABLE_TRANSIVER_INT();
	DISABLE_AIR();
	uint8_t i;
	uint16_t fifo = ZB_RX_FIFO; // 0x300
	uint8_t frm_len = ZB_READ_LONG_REG(fifo);
	frm_len += ZB_MAC_EXTRA_DATA_SIZE+1;
	zb_buf_initial_alloc(buf,frm_len);
	zb_uint8_t *ptr = ZB_BUF_BEGIN(buf);
	for (i = 0; i< frm_len; i++)
	{
		*ptr = ZB_READ_LONG_REG(fifo);
		fifo++;
		ptr++;
	}
	ENABLE_AIR();
	ZB_ENABLE_TRANSIVER_INT();
	TRACE_MSG(TRACE_MAC2, "<< zb_read_rx_fifo ", (FMT__0));
}

void select_radio()
{	
	GPIO_ResetBits(ZB_SPIx_NSS_GPIO_PORT, ZB_SPIx_NSS_PIN);	// set CS to low state
}

void deselect_radio()
{
	GPIO_SetBits(ZB_SPIx_NSS_GPIO_PORT, ZB_SPIx_NSS_PIN);	// set CS to high state
}

void zb_transceiver_select_channel(zb_uint8_t n) //Works
{
	uint8_t val = 0;
	DECLARE_VAR();	
	TRACE_MSG(TRACE_COMMON1, "zb_transceiver_select_channel",(FMT__0));
	if (n == MAC_CTX().current_channel)		// if channel has already chosen
		return;								// nothing to do here
	// writing channel number + optimization value
	//ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL0,((n-ZB_TRANSCEIVER_START_CHANNEL_NUMBER)<<4)|0x03);
	val = (n - ZB_TRANSCEIVER_START_CHANNEL_NUMBER)<<4;
	val |= 0x03;
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL0,val);
	CHECK_L(ZB_LREG_RFCTRL0);

	// perform RF state machine reset as described in datasheet
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL,0x04);
	USEC192_DELAY();
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x02);
	USEC192_DELAY();
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x01);
	USEC192_DELAY();
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x00);
	MAC_CTX().current_channel = n;		// set new currenc channel
}	

// Read interrupt pending register and writing it value into TRANS_CTX
void zb_mrf_check_int_status()
{
/*
	SREG ISRSTS
	Bit 7 SLPIF: Sleep alert interrupt bit
	Bit 6 WAKEIF: Wake-up alert interrupt bit
	Bit 5 HSYMTMRIF: Half symbol timer interrupt bit
	Bit 4 SECIF: Security key request interrupt bit
	Bit 3 RXIF: RX receive interrupt bit
	Bit 2 TXG2IF: TX GTS2 FIFO transmission interrupt bit
	Bit 1 TXG1IF: TX GTS1 FIFO transmission interrupt bit
	Bit 0 TXNIF: TX Normal FIFO transmission interrupt bit
*/
	zb_uint8_t reg = 0;
	TRACE_MSG(TRACE_MAC1, "Ints: %d" , (FMT__D_D, ZB_IOCTX().int_counter));
	// anyway - if we should read int status, interrupt has already occured.
	ZB_CLEAR_TRANS_INT();
	reg = ZB_READ_SHORT_REG(ZB_SREG_INTSTAT);
	TRACE_MSG(TRACE_MAC1, "interrupt INTSTAT: 0x%hx" , (FMT__H, reg));
	TRANS_CTX().int_status = reg;
	
	// TODO: check following comment:
	// if uz2400 is sleeping, it still can produce an interrupt, and we can even handle it,   but we can't read/write fifo or some long regs 
	
	// TODO: check indirect data situation, which described in uz2400.c here.
	
#ifdef ZB_MAC_SECURITY
	zb_uz_secur_handle_rx();
#endif  /* ZB_MAC_SECURITY */

  /* TODO: Fixme after autoack pending bit problem resolved */
  /* check for a pending frame bit in received ACK, but only after DATA_REQUEST */
	if (ZB_MAC_GET_INDIRECT_DATA_REQUEST())
	{
		reg = ZB_READ_SHORT_REG(ZB_SREG_TXNCON);
		TRACE_MSG(TRACE_COMMON2, "TXNCON read: 0x%hx", (FMT__H, reg));
		if (reg & 0x10)
		{
		  TRACE_MSG(TRACE_COMMON2, "Pending data set!", (FMT__0));
		  ZB_MAC_SET_PENDING_DATA();
		}
		else
		{
		  ZB_MAC_CLEAR_PENDING_DATA();
		  TRACE_MSG(TRACE_MAC2, "Pending data clear", (FMT__0));
		}
		TRACE_MSG( TRACE_MAC1, "pending data %hd", (FMT__H, ZB_MAC_GET_PENDING_DATA()));
	}
	
	if (ZB_UBEC_GET_TX_DATA_STATUS())
	{
		MAC_CTX().tx_cnt++;
		TRACE_MSG(TRACE_MAC1, "TX counter: %hd", (FMT__H, MAC_CTX().tx_cnt));
		reg = ZB_READ_SHORT_REG(ZB_SREG_TXSTAT);
		//TRACE_MSG(TRACE_COMMON3, "tx status: 0x%hx" , (FMT__H, ZB_MAC_GET_BYTE_VALUE()));
		TRANS_CTX().tx_status = reg;
	}
}

void zb_init_mrf24j40()
{
	TRACE_MSG(TRACE_COMMON1, "zb_init_mrf24j40",(FMT__0));
	DECLARE_VAR();
	/**** MY ****/
	// Baseband register
	//ZB_WRITE_SHORT_REG(BBREG2,0x80); 	// CCA mode 1 - energy above treshold.
	// ENERGY DETECTION THRESHOLD FOR CCA REGISTER
	//ZB_WRITE_SHORT_REG(CCAEDTH,0x60);	//  Clear Channel Assessment (CCA) Energy Detection (ED) Mode bits -69dBm - recommended
	ZB_WRITE_SHORT_REG(ZB_SREG_SOFTRST, 0x07);	
	CHECK_S(ZB_SREG_SOFTRST);
	while(ZB_READ_SHORT_REG(ZB_SREG_SOFTRST) & 0x07) ;
    USEC192_DELAY();
    USEC192_DELAY();
	volatile uint8_t reg  = 0;
	//-------------------------------------------------------
	

	// GATECLK: GATED CLOCK CONTROL REGISTER (ADDRESS: 0x26)
	//MMI module clock off 
	ZB_WRITE_SHORT_REG(ZB_SREG_GATECLK, 0x20);	
	CHECK_S(ZB_SREG_GATECLK);
	// PACON1: POWER AMPLIFIER CONTROL 1 REGISTER (ADDRESS: 0x17)
	//Power amplifier on time before beginning of packet = 4*16us
	ZB_WRITE_SHORT_REG(ZB_SREG_PACON1, 0x08);		
	CHECK_S(ZB_SREG_PACON1);
	
	// PACON2: POWER AMPLIFIER CONTROL 2 REGISTER (ADDRESS: 0x18) in mrf | FIFOEN in UBEC
	// TXFIFO and RXFIFO output always enable,
	// Transmitter on time before beginning of packet = 5*16us
	// à â mrf ñòåêå 0x98
        ZB_WRITE_SHORT_REG(0x18, 0x98);		
	CHECK_S(0x18);
        
	ZB_WRITE_SHORT_REG(ZB_SREG_FIFOEN, 0x94);	//0b1001.0100
	CHECK_S(ZB_SREG_FIFOEN);
	// TXSTBL: TX STABILIZATION REGISTER (ADDRESS: 0x2E) in MRF. | TXPEMISP in UBEC.
	// The minimum number of symbols forming a Short Interframe Spacing (SIFS) period = 5*16us
	//  VCO Stabilization Period bits = recommended value
	ZB_WRITE_SHORT_REG(ZB_SREG_TXPEMISP, 0x95);
	CHECK_S(ZB_SREG_TXPEMISP);

	// !!!!!! =================================================================!!!!!!!!!!!!!!!!!!!!!!
	
	
	
	// My 5 kopeek. Setup CCA treshold
	// mrf stack = 0x80 0x78
	ZB_WRITE_SHORT_REG(ZB_SREG_BBREG2,0x78);
	CHECK_S(ZB_SREG_BBREG2);
	
	// BBREG3: BASEBAND 3 REGISTER (ADDRESS: 0x3B) in MRF. | BBREG3 : Preamble Search ED  in UBEC.
	// Energy Valid Threshold = 0x5. recommended is 0b1101 was 0x50
	ZB_WRITE_SHORT_REG(ZB_SREG_BBREG3, 0xD0);
	CHECK_S(ZB_SREG_BBREG2);
	// BBREG5 : Preamble Searching Time Threshold/Boundary ONLY IN UBEC
	// PeakEarly = 56
	// PeakLate = 80
	ZB_WRITE_SHORT_REG(ZB_SREG_BBREG4, 0x9c);
	
	ZB_WRITE_SHORT_REG(ZB_SREG_BBREG5, 0x07);
	CHECK_S(ZB_SREG_BBREG5);
	// BBREG6: BASEBAND 6 REGISTER (ADDRESS: 0x3E). RSSI mode in UBEC
	// RSSI Mode 2 bit. calculating RSSI for RX packet. The RSSI value is stored in RXFIFO
	ZB_WRITE_SHORT_REG(ZB_SREG_BBREG6, 0x40);
	CHECK_S(ZB_SREG_BBREG6);
	// ZB_LREG_RFCTRL0: RF CONTROL 0 REGISTER (ADDRESS: 0x200) in MRF. | RFCTRL0 in UBEC
	// Select 11 channel and load optimized 0x03 value
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL0, CHANNEL_11 | 0x03);
	CHECK_L(ZB_LREG_RFCTRL0);
	// RFCON1: RF CONTROL 1 REGISTER	 (ADDRESS: 0x201) in MRF.
	// Set VCO optimize control bits
	// mrf = 0x01
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL1, 0x02);
	CHECK_L(ZB_LREG_RFCTRL1);
	// RFCON2: RF CONTROL 2 REGISTER (ADDRESS: 0x202) 
	// Set MAGIC number. PLL<7> bit is off. I dont know why.
	// mrf stack = 0x80
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL2, 0x66);
	CHECK_L(ZB_LREG_RFCTRL2);
	// mrf stack has PHYSetLongRAMAddr(RFCTRL3,PA_LEVEL);

//	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL3, 0);
//	CHECK_L(ZB_LREG_RFCTRL3);
        
	// REG04 : RFCTRL4 - ONLY IN UBEC
	// set magic number
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL4, 0x09);
	CHECK_L(ZB_LREG_RFCTRL4);
	// RFCON6: RF CONTROL 6 REGISTER (ADDRESS: 0x206) 
	// Recovery from Sleep control. Less than 1 ms (recommended)
	// mrf stack = 0x90
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL6, 0x30);
	CHECK_L(ZB_LREG_RFCTRL6);
	// RFCON7: RF CONTROL 7 REGISTER (ADDRESS: 0x207) 
	// setting strange Sleep Clock Selection bits. there are only 10 and 01 variants.
	// setting magic 0xC number.
	// mrf stack = 0x80
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL7, 0xEC);
	CHECK_L(ZB_LREG_RFCTRL7);
	// RFCON8: RF CONTROL 8 REGISTER (ADDRESS: 0x208) 
	// VCO Control bit = 0 and other magic numbers.
	// mrf stack has = 0x10
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL8, 0x8C);
	CHECK_L(ZB_LREG_RFCTRL8);
	// REG36: ASSO_BCN_LADR6
	// set Long address 6 of associated coordinator . 
	ZB_WRITE_LONG_REG(ZB_LREG_GPIODIR, 0x00);
	CHECK_L(ZB_LREG_GPIODIR);
	// BLACK MAGIC. 
	ZB_WRITE_LONG_REG(ZB_LREG_SECCTRL, 0x20);
	
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL50, 0x05);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL51, 0xC0);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL52, 0x01);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL59, 0x00);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL73, 0x40);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL74, 0xC5);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL75, 0x13);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL76, 0x07);
	
	// Disable SLPCLKEN, falling edge.
	ZB_WRITE_LONG_REG(ZB_LREG_IRQCTRL,0x80);
	
	// Setting Enable_ALL interrupt mask
	ZB_WRITE_SHORT_REG(ZB_SREG_INTMSK, 0x00);

	// perform soft reset
	ZB_WRITE_SHORT_REG(ZB_SREG_SOFTRST, 0x02);
/*
	ZB_WRITE_SHORT_REG(ZB_SREG_PANIDH,0xff);
	ZB_WRITE_SHORT_REG(ZB_SREG_PANIDL,0xff);
	ZB_WRITE_SHORT_REG(ZB_SREG_SADRH,0xff);
	ZB_WRITE_SHORT_REG(ZB_SREG_SADRL,0xff);
*/

	/*	Initial config.
		1. Set reception mode RXMCR<1:0>
			00 - normal
			10 - error
			01 - promiscuous
		2. Setup frame format filter. RXFLUSH<3:1>
			000 - all frames
			100 - command only
			010 - data only
			001 - beacon only
		3. Don't forget about acknowledgement
		4. Set correct interrupt mask in INTCON
	*/	
	ZB_WRITE_SHORT_REG(ZB_SREG_RXMCR,0x00); // 1
	//CHECK_S(ZB_SREG_RXMCR);
	ZB_WRITE_SHORT_REG(ZB_SREG_RXFLUSH, 0x00); // 2
	CHECK_S(ZB_SREG_RXFLUSH);
	ZB_WRITE_SHORT_REG(ZB_SREG_INTMSK,0x00);	// all ints.
	
	// from UBEC DS
	//ZB_WRITE_LONG_REG(ZB_LREG_RXFRMTYPE, 0x0F);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL2, 0x80);
	ZB_WRITE_LONG_REG(ZB_LREG_RFCTRL4, 0x66); 
	//ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x03);
	

	// rerform correct RF state machine reset and TURN ON tx transmit.

	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL,0x04);
	/* TODO: check, is it enough for "192us waiting" */
	USEC192_DELAY();
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x02);
	USEC192_DELAY();
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x01);
	USEC192_DELAY();
	ZB_WRITE_SHORT_REG(ZB_SREG_RFCTL, 0x00);
	/* LREG 0x23C RXFRMTYPE
     Now, it's disabled, according to DS-2400-51_v0_6_RN.pdf, p.158
     RXFTYPE[7:0]: RX Frame Type Filter
     00001011: (default - Do Not Change)
     bit 7-4 reserver
     bit3 command
     bit2 ack                                                                                          ?
     bit1 data
     bit0 beacon */
     	ZB_WRITE_LONG_REG(ZB_LREG_RXFRMTYPE, 0x0B); /* we accept all frames */
	CHECK_L(ZB_LREG_RXFRMTYPE);
	/*
	LREG SCLKDIV
	Bit 7 I2CWDTEN: I2C watchdog timer enable
	Bit 4-0 SCLKDIV: sleep clock division selection.
	n: the sleep clock is divided by 2^n before being fed to logic circuit.
	
	SCLKDIV = 1
	I2CWDTEN = 0
	0000.0001 == 0x1
	*/
	ZB_WRITE_LONG_REG(ZB_LREG_SCLKDIV, 0x01);
	
	
#if 0
	/*
	SREG INTMSK
	Bit 7 SLPMSK: Sleep alert interrupt mask
	Bit 6 WAKEMSK: Wake-up alert interrupt mask
	Bit 5 HSYMTMRMSK: Half symbol timer interrupt mask
	Bit 4 SECMSK: security interrupt mask
	Bit 3 RXMSK: RX receive interrupt mask
	Bit 2 TXG2MSK: TX GTS2 FIFO transmission interrupt mask
	Bit 1 TXG1MSK: TX GTS1 FIFO transmission interrupt mask
	Bit 0 TXNMSK: TX Normal FIFO transmission interrupt mask
	
	Read original value and turn on wakeup interrupt
	wakemsk = 0
	1011.1111 == 0xbf
	*/
	//zb_uz24_mask_short_reg(ZB_SREG_INTMSK, 0xBF);
	reg = ZB_READ_SHORT_REG(ZB_SREG_INTMSK);
	ZB_WRITE_SHORT_REG(ZB_SREG_INTMSK,reg & 0xBF);
	/*
	SREG SLPACK
	Bit 7 SLPACK: Sleep acknowledge. Set this bit to 1 will cause UZ2400 enter the sleep mode
	immediately. This bit will be automatically cleared to 0.
	Bit 3-0 WAKECNT: System clock (20MHz) recovery time
	
	Read original value and set bit to enter sleep mode
	slpack = 1
	1000.0000 == 0x80
	*/


	/*
	SREG WAKECTL
	Bit 7 IMMWAKE: Immediate wake-up mode enables.
	Bit 6 REGWAKE: Register-triggered wake-up signal. It should be
	cleared to 0 by host MCU.
	
	Read original value and set 2 bits, turn on external wake up mode
	1100.0000 == 0xC0
	*/
	
	//zb_uz24_or_mask_short_reg(ZB_SREG_WAKECTL, 0xC0);
	reg = ZB_READ_SHORT_REG(ZB_SREG_WAKECTL);
	ZB_WRITE_SHORT_REG(ZB_SREG_WAKECTL, reg | 0xC0);
	/* TODO: check - ubec stack uses long cycle with nop here */
	
	/* TODO: check, in ubec stack interrupts are disabled here */
	
	/*
	SREG WAKECTL
	Bit 7 IMMWAKE: Immediate wake-up mode enables.
	Bit 6 REGWAKE: Register-triggered wake-up signal. It should be
	cleared to 0 by host MCU.
	
	Read original value and clear regwake to wake up the chip
	1011.1111 == 0xBF
	*/
	//zb_uz24_mask_short_reg(ZB_SREG_WAKECTL, 0xBF);
	reg = ZB_READ_SHORT_REG(ZB_SREG_WAKECTL);
	ZB_WRITE_SHORT_REG(ZB_SREG_WAKECTL,reg & 0xBF);
	/*
	SREG INTSTAT
	Bit 7 SLPIF: Sleep alert interrupt bit
	Bit 6 WAKEIF: Wake-up alert interrupt bit
	Bit 5 HSYMTMRIF: Half symbol timer interrupt bit
	Bit 4 SECIF: Security key request interrupt bit
	Bit 3 RXIF: RX receive interrupt bit
	Bit 2 TXG2IF: TX GTS2 FIFO transmission interrupt bit
	Bit 1 TXG1IF: TX GTS1 FIFO transmission interrupt bit
	Bit 0 TXNIF: TX Normal FIFO transmission interrupt bit
	
	check wakeif bit
	0100.0000 == 0x40
	*/
	
	/* wait for chip ready 
	//do
	//{
	//	ZB_ZB_READ_SHORT_REG(ZB_SREG_ISRSTS);
	//}
	//while (!(ZB_MAC_GET_BYTE_VALUE() & 0x40));
	*/
	//while (!(reg = ZB_READ_SHORT_REG(ZB_SREG_ISRSTS) & 0x40));
	MSEC_DELAY_N(300);
	/*
	SREG WAKECTL
	Bit 7 IMMWAKE: Immediate wake-up mode enables.
	Bit 6 REGWAKE: Register-triggered wake-up signal. It should be
	cleared to 0 by host MCU.
	
	immwake = 0 turn back normal wake up mode
	0111.1111 == 0x7f
	*/
	
	//zb_uz24_mask_short_reg(ZB_SREG_WAKECTL, 0x7F);
	reg = ZB_READ_SHORT_REG(ZB_SREG_WAKECTL);
	ZB_WRITE_SHORT_REG(ZB_SREG_WAKECTL,reg & 0x7F);
	/* Enable all interrupts */
	ZB_WRITE_SHORT_REG(ZB_SREG_INTMSK, 0);
#endif
	/* TODO: check, in ubec stack interrupts are enabled here */
	//ZB_ENABLE_TRANSIVER_INT();
}


void zb_transceiver_get_rssi(zb_uint8_t *rssi_value)
{
	uint8_t reg = 0;
  TRACE_MSG(TRACE_MAC2, ">> zb_transceiver_get_rssi", (FMT__0));
  ZB_ASSERT(rssi_value);

/*
  SREG0x3E BBREG6
  Bit 7 RSSIMODE1: RSSI mode 1 enable
  1: calculate RSSI for firmware request, will be clear to 0 when RSSI calculation is finished.
  Bit 6 RSSIMODE2: RSSI mode 2 enable
  1: calculate RSSI for each received packet, the RSSI value will be stored in RXFIFO.
  0: no RSSI calculation for received packet.
  Bit 0 RSSIRDY: RSSI ready signal for RSSIMODE1 use
  If RSSIMODE1 is set, this bit will be cleared to 0 until RSSI calculation is done. When RSSI
  calculation is finished and the RSSI value is ready, this bit will be set to 1 automatically
*/

/* 1000.0000 == 0x80 set bit RSSIMODE1 */
//  zb_uz24_mask_short_reg(ZB_SREG_BBREG6, 0x80);
  ZB_WRITE_SHORT_REG(ZB_SREG_BBREG6, 0x80);

  while (1)
  {
    reg = ZB_READ_SHORT_REG(ZB_SREG_BBREG6);
    if (reg & 0x01) /* check bit RSSIRDY */
    {
      /* RSSI value is ready to read */
      break;
    }
  }
  /* RSSI register is not described in DS-2400, but it is used in
   * ubec stack */
  
  *rssi_value = ZB_READ_LONG_REG(ZB_LREG_RSSI);
  ZB_WRITE_SHORT_REG(ZB_SREG_BBREG6, 0x40);

  TRACE_MSG(TRACE_MAC2, "<< zb_transceiver_get_rssi rssi_value %hd", (FMT__H, *rssi_value));
}

void zb_transceiver_set_coord_short_addr(zb_uint16_t coord_addr_short)
{
  TRACE_MSG(TRACE_COMMON1, "zb_transceiver_set_coord_short_addr",(FMT__0));
  ZB_WRITE_LONG_REG(ZB_LREG_ASSOSADR0, ZB_GET_LOW_BYTE(coord_addr_short));
  ZB_WRITE_LONG_REG(ZB_LREG_ASSOSADR1, ZB_GET_HI_BYTE(coord_addr_short));
}

zb_ret_t zb_transceiver_send_fifo_packet(zb_uint8_t header_length, zb_int16_t fifo_addr,
                                         zb_buf_t *buf, zb_uint8_t need_tx)
{
	zb_uint8_t *fc = ZB_BUF_BEGIN(buf);

	TRACE_MSG(TRACE_MAC1, ">> zb_transceiver_send_fifo_packet, %d, addr %x, buf %p, state %hd", (FMT__D_D_P,
																								(zb_uint16_t)header_length, fifo_addr, buf));
	
	ZB_ASSERT(fifo_addr == ZB_NORMAL_TX_FIFO);
	
	/* ds-2400 4.3.1. Transmit Packet in Normal FIFO */
	
	{
		zb_mrf_fifo_header_t *mrf_fifo_header;
		ZB_BUF_ALLOC_LEFT(buf, sizeof(zb_mrf_fifo_header_t), mrf_fifo_header);
		ZB_ASSERT(mrf_fifo_header);
		mrf_fifo_header->header_length = header_length;
		mrf_fifo_header->frame_length = ZB_BUF_LEN(buf) - sizeof(zb_mrf_fifo_header_t);
	}
	
	ZB_MRF_CLEAR_NORMAL_FIFO_TX_STATUS();
	ZB_CLEAR_TX_STATUS();
	
	zb_mrf_fifo_write(fifo_addr, buf);

  /* TODO: if acknowledgement is required for normal fifo, set ackreq
   * bit (SREG0x1B[2]) */
	if (need_tx)
	{
		/* we need to determine if our frame broadcast or not */
	
		/* Don't want to parse entire mhr here. All we need is frame control and
		* destination address. Destination address has fixed position in mhr.
		* Fields layout is fc (2b), seq number (1b), dest panid (2b), dest address (2b).
		*/
		zb_uint8_t need_ack = (!((ZB_FCF_GET_FRAME_TYPE(fc) == MAC_FRAME_BEACON
								|| (ZB_FCF_GET_DST_ADDRESSING_MODE(fc) == ZB_ADDR_16BIT_DEV_OR_BROADCAST
								&& fc[5] == 0xff && fc[6] == 0xff)))
								&& ZB_FCF_GET_ACK_REQUEST_BIT(fc));
		
		/* The same bit is used to start normal and beacon fifio.
		If not joined yet (pac_pan_id is not set), do not request acks.
		*/
		
		//if (ZB_FCF_GET_ACK_REQUEST_BIT(fc))
		//	need_ack = 1;
		TRACE_MSG(TRACE_MAC2, "Need ACK: %hd", (FMT__H, need_ack));
		ZB_START_NORMAL_FIFO_TX(ZB_MAC_PIB_MAX_FRAME_RETRIES, need_ack);
	}
	TRACE_MSG(TRACE_MAC1, "<< zb_transceiver_send_fifo_packet", (FMT__0));
	return RET_OK;
}

void send_any_sheet(uint8_t *p, int len)
{
	DECLARE_VAR();
	TRACE_MSG(TRACE_COMMON1, "send_any_sheet",(FMT__0));
	
	//GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
	ZB_WRITE_SHORT_REG(ZB_SREG_TXNCON,0x00);
	volatile zb_uint8_t reg = 0;
	int i=0x000;
	zb_uint8_t *ptr = 0;
	zb_uint16_t fifo;
	zb_buf_t pack, tupack;

	zb_buf_initial_alloc(&pack,len);
	ptr = ZB_BUF_BEGIN(&pack);
	
	ZB_MEMCPY(ptr,p,len);
	
	ZB_TRANS_FILL_FIFO(9,&pack);
	zb_mrf_fifo_read(1,&tupack,len+2);
	/*
	fifo = 0x000;
	for (i = 0; i<pack.u.hdr.len; i++)
	{
		ZB_WRITE_LONG_REG(fifo,*ptr);
		fifo++;
		ptr++;
	}*/
	fifo = 0x000;
	zb_uint8_t mas[55];
	ZB_BZERO(mas,55);
	for (i = 0; i<55; i++)
	{
		mas[i] = ZB_READ_LONG_REG(fifo++);
	}
	ZB_TRANS_SEND_COMMAND(9,&pack);
	//zb_read_rx_fifo(&tupack);

	CHECK_S(ZB_SREG_TXSR);
}

#endif
