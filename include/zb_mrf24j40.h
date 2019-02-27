// zb_mrf24j40.h

#ifndef ZB_MRF24J40_H_
#define ZB_MRF24J40_H_

	#include "zb_osif_cortexm4.h"
	#include "zb_cortexm4_spi.h"
	#include "zb_mrf24j40_registers.h"

	typedef struct zb_transceiver_ctx_s
	{
		zb_uint8_t int_status;
		zb_uint8_t tx_status;
		zb_uint8_t interrupt_flag;
	}
	zb_transceiver_ctx_t;
	
	// It's like ubec but mrf. Yeah.
	typedef struct zb_mrf_fifo_header_s
	{
	  zb_uint8_t header_length;
	  zb_uint8_t frame_length;
	} ZB_PACKED_STRUCT
	zb_mrf_fifo_header_t;
	
	#define MRF_EXCHANGE spi_sync_exchange
	
	#define SELECT_RADIO()					select_radio()
	#define	DESELECT_RADIO()				deselect_radio()
#if 0
	#define	ZB_WRITE_LONG_REG(a,d)			zb_mac_long_write_reg((a),(d))
	#define	ZB_WRITE_SHORT_REG(a,d)			zb_mac_short_write_reg((a),(d))
	#define	ZB_READ_LONG_REG(a)				zb_mac_long_read_reg((a))
	#define	ZB_READ_SHORT_REG(a)			zb_mac_short_read_reg((a))
#endif
	#define	ZB_WRITE_LONG_REG(a,d)			write_long_reg((a),(d))
	#define	ZB_WRITE_SHORT_REG(a,d)			write_short_reg((a),(d))
	#define	ZB_READ_LONG_REG(a)				read_long_reg((a))
	#define	ZB_READ_SHORT_REG(a)			read_short_reg((a))

	#define ZB_INIT_RADIO_DEV()				zb_init_mrf24j40()
	#define ZB_TRANSCEIVER_SET_CHANNEL(c_n) zb_transceiver_select_channel((c_n))
	#define DISABLE_AIR()					ZB_WRITE_SHORT_REG(ZB_SREG_BBREG1,0x04);	
	#define ENABLE_AIR()					ZB_WRITE_SHORT_REG(ZB_SREG_BBREG1,0x00);
	#define ZB_TRANS_RECV_PACKET(buf) 		zb_mrf_fifo_read(0, buf, 0)
	//#define ZB_TRANS_RECV_PACKET(buf)		zb_read_rx_fifo(buf)
	//#define ZB_TRANS_READ_TXNFIFO(buf, len)  zb_write_tx_fifo(buf, len)
	#define ZB_TRANS_READ_TXNFIFO(buf,len)	zb_mrf_fifo_read(1,buf,len)
	
	void zb_read_rx_fifo(zb_buf_t *buf);
	// Registers read/write functions
	zb_uint8_t zb_mac_short_read_reg(zb_uint8_t short_addr);
	zb_uint8_t zb_mac_long_read_reg(zb_uint16_t long_addr);
	void zb_mac_short_write_reg(zb_uint8_t short_addr, zb_uint8_t byte_value);
	void zb_mac_long_write_reg(zb_uint16_t long_addr, zb_uint8_t byte_value);
	
	//zb_uint8_t spi_sync_exchange(zb_uint8_t tx_data);
	zb_uint8_t read_short_reg(zb_uint8_t addr);
	void write_short_reg(zb_uint8_t addr,zb_uint8_t tx_data);
	zb_uint8_t read_long_reg(zb_uint16_t addr);
	void write_long_reg(zb_uint16_t addr, zb_uint8_t tx_data);
	
	// Chip Selection for transceiver
	void select_radio();
	void deselect_radio();
	
	// Basic initialization + receive mode setup
	void zb_init_mrf24j40();
	
	// Channel selection function
	void zb_transceiver_select_channel(zb_uint8_t n);
	
	// RSSI result
	void zb_transceiver_get_rssi(zb_uint8_t *rssi_value);
	
	// Read interrupt pending register and writing it value into TRANS_CTX
	void zb_mrf_check_int_status();
	
	zb_ret_t zb_transceiver_send_fifo_packet(zb_uint8_t header_length, zb_int16_t fifo_addr, zb_buf_t *buf, zb_uint8_t need_tx);
	
	// Works with TX, RX and other fifo.
	void zb_mrf_fifo_write(zb_uint16_t long_addr, zb_buf_t *buf);
	void zb_mrf_fifo_read(zb_uint8_t tx_fifo, zb_buf_t *buf, zb_uint8_t len);
	
	// Test function that sends example broadcast packet writing it to TX FIFO.	
	void send_any_sheet(uint8_t *p, int len);
	
	#define ZB_CLEAR_NORMAL_FIFO_BUSY() (TRANS_CTX().normal_fifo_busy = 0)
	#define ZB_MRF_GET_NORMAL_FIFO_TX_STATUS() (TRANS_CTX().int_status & 0x01) /* check bit 0 */
	#define ZB_MRF_CLEAR_NORMAL_FIFO_TX_STATUS() (TRANS_CTX().int_status &= 0xFE) /* clear bit 0 */
	#define ZB_TRANS_IS_COMMAND_SEND() ZB_MRF_GET_NORMAL_FIFO_TX_STATUS() /* not 0 means command is sent */
	// Interrupt checkers
	#define ZB_CHECK_INT_STATUS()	zb_mrf_check_int_status()
	#define ZB_SET_TRANS_INT() 		(TRANS_CTX().interrupt_flag = 1)
	#define ZB_CLEAR_TRANS_INT() 	(TRANS_CTX().interrupt_flag = 0)
	#define ZB_GET_TRANS_INT() 		(TRANS_CTX().interrupt_flag)
	
	#define ZB_CLEAR_TX_STATUS() 	(TRANS_CTX().int_status &= 0xFE, TRANS_CTX().tx_status = 0)
	/* check TXSR bit 0 - Normal FIFO release status (1 - fail, retry count exceed) */
	#define ZB_IS_TX_CHANNEL_BUSY() (TRANS_CTX().tx_status & 0x20)
	/* check TXSR bit 5 - CCAFAIL: Channel busy causes CSMA-CA fails */
	#define ZB_IS_TX_RETRY_COUNT_EXCEEDED() (TRANS_CTX().tx_status & 0x01)
	
	#define ZB_TRANS_CHECK_CHANNEL_BUSY_ERROR() (ZB_IS_TX_CHANNEL_BUSY()) /* not 0 means channel busy error */
	#define ZB_TRANS_CHECK_TX_RETRY_COUNT_EXCEEDED_ERROR() (ZB_IS_TX_RETRY_COUNT_EXCEEDED()) /* not 0 means cca fail error */

	#define	ZB_UPDATE_LONGMAC()													\
	do 																			\
	{																			\
	  zb_ushort_t i;															\
	  for (i = 0 ; i < 8 ; i++)													\
	  {																			\
		ZB_WRITE_SHORT_REG(ZB_SREG_EADR0 + i, ZB_PIB_EXTENDED_ADDRESS()[i]);	\
	  }																			\
	} while(0)
	
	#define ZB_TRANSCEIVER_SET_PAN_ID(pan_id)							\
	(	ZB_WRITE_SHORT_REG(ZB_SREG_PANIDL, ZB_GET_LOW_BYTE(pan_id)),	\
		ZB_WRITE_SHORT_REG(ZB_SREG_PANIDH, ZB_GET_HI_BYTE(pan_id))		)
		
	#define ZB_UPDATE_PAN_ID()	(ZB_TRANSCEIVER_SET_PAN_ID(MAC_PIB().mac_pan_id))
	
	#define ZB_UPDATE_SHORT_ADDR() 	\
	(	ZB_WRITE_SHORT_REG(ZB_SREG_SADRL,ZB_GET_LOW_BYTE(MAC_PIB().mac_short_address)),	\
		ZB_WRITE_SHORT_REG(ZB_SREG_SADRH,ZB_GET_HI_BYTE	(MAC_PIB().mac_short_address))	)
		
	#define ZB_CLEAR_SHORT_ADDR()	\
	(	ZB_WRITE_SHORT_REG(ZB_SREG_SADRL,-1),	\
		ZB_WRITE_SHORT_REG(ZB_SREG_SADRH,-1)	)
	
			
	#define ZB_TRANSCEIVER_START_CHANNEL_NUMBER 	11
	#define ZB_TRANSCEIVER_MAX_CHANNEL_NUMBER   	26
	
	// it's *_UBEC_* due to historical reasons.
	#define ZB_UBEC_GET_RX_DATA_STATUS() 	(TRANS_CTX().int_status & 0x08)  /* get bit 3 */
	#define ZB_UBEC_GET_TX_DATA_STATUS() 	(TRANS_CTX().int_status & 0x01)  /* get bit 0 */
	#define ZB_UBEC_GET_WAKE_STATUS()    	(TRANS_CTX().int_status & 0x40)  /* get bit 6 */
	#define ZB_UBEC_GET_SECUR_STATUS()   	(TRANS_CTX().int_status & 0x10)  /* get bit 4 */
	
	#define ZB_UBEC_CLEAR_SECUR_STATUS() 	(TRANS_CTX().int_status &= (~0x10))
	#define ZB_UBEC_CLEAR_RX_DATA_STATUS() 	(TRANS_CTX().int_status &= (~0x08))  /* clear bit 3 */
	
	#define ZB_TRANS_CUT_SPECIFIC_HEADER(zb_buffer)                         \
	{                                                                       \
	  /* 1st byte contains packet length*/                                  \
	  (void)zb_buf_cut_left((zb_buffer), 1);                                \
	}
	
	#ifdef ZB_CHANNEL_ERROR_TEST
	#define ZB_TRANS_CHECK_CHANNEL_ERROR() (((ZG->nwk.nib.nwk_tx_total % 3) == 0 && ZB_MAC_GET_CHANNEL_ERROR_TEST()) ? (1) : 0)
	#else
	#define ZB_TRANS_CHECK_CHANNEL_ERROR() (ZB_IS_TX_CHANNEL_BUSY() || ZB_IS_TX_RETRY_COUNT_EXCEEDED())
	#endif
		
	#define ZB_TRANSCEIVER_SET_COORD_SHORT_ADDR(addr) 					\
	do 																	\
	{																	\
		ZB_WRITE_LONG_REG(ZB_LREG_ASSOSADR0, ZB_GET_LOW_BYTE(addr));	\
		ZB_WRITE_LONG_REG(ZB_LREG_ASSOSADR1, ZB_GET_HI_BYTE(addr));		\
	} while(0)
	
	#define ZB_CHECK_BEACON_MODE_ON() ZB_TRUE
	
	#define ZB_RXFLUSH() \
	(	ZB_MAC_GET_BYTE_VALUE() = ZB_READ_SHORT_REG(ZB_SREG_RXFLUSH), \
		ZB_MAC_GET_BYTE_VALUE()|=0x01,      \
		ZB_WRITE_SHORT_REG(ZB_SREG_RXFLUSH, ZB_MAC_GET_BYTE_VALUE())	)
	
	/**
   Send command/data/beacon to the transiver FIFO

   @param header_length - mhr length to write to UZ transiver
   @param buf - buffer to send
 */

	#define ZB_TRANS_SEND_COMMAND(header_length, buf)	\
		zb_transceiver_send_fifo_packet((header_length), ZB_NORMAL_TX_FIFO, (buf), 1)
	#define ZB_TRANS_FILL_FIFO(header_length, buf)      \
		zb_transceiver_send_fifo_packet((header_length), ZB_NORMAL_TX_FIFO, (buf), 0)
		
	#define ZB_START_NORMAL_FIFO_TX(retries, need_ack)  \
	(	MAC_CTX().tx_cnt = 0,							\
		(ZB_WRITE_SHORT_REG(ZB_SREG_TXNCON, ((retries << 5) & 0xE0) | ((need_ack)? 0x05 : 0x01)))\
	)

	#define ZB_MAC_GET_BYTE_VALUE() (ZG->mac.mac_ctx.rw_reg.value.byte_value)
	/* set up/down pending bit */
	#define ZB_SET_PENDING_BIT()	\
		(	ZB_MAC_GET_BYTE_VALUE() = ZB_READ_SHORT_REG(ZB_SREG_ACKTMOUT),                                \
			ZB_WRITE_SHORT_REG(ZB_SREG_ACKTMOUT, ZB_MAC_GET_BYTE_VALUE()|0x80)	)

	#define ZB_CLEAR_PENDING_BIT() 	\
		(	ZB_MAC_GET_BYTE_VALUE() = ZB_READ_SHORT_REG(ZB_SREG_ACKTMOUT),								\
			ZB_WRITE_SHORT_REG(ZB_SREG_ACKTMOUT, ZB_MAC_GET_BYTE_VALUE()&0x7F) 	)
		
	//void zb_transceiver_set_coord_ext_addr(zb_ieee_addr_t coord_addr_long);
	#define ZB_TRANSCEIVER_SET_COORD_EXT_ADDR(addr) 						\
	do {																	\
		zb_uint8_t i = 0;													\
		while (i < sizeof(zb_ieee_addr_t))									\
		{																	\
		/* write one bye one 8 bytes of the extended address */				\
			ZB_WRITE_LONG_REG(ZB_LREG_ASSOEADR0 + i, coord_addr_long[i]);	\
			i++;															\
		}																	\
	} while(0)
	
	
	// NOTE: This nop-delay defines uses only for transmitter routines according to datasheet
	// 168 MHz - это не шубу в трусы заправлять...
	#define USEC_DELAY()	\
	__asm volatile(			\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0\n\t"\
		"mov     r0, r0"\
	)

	#define TEN_USEC_DELAY()		\
	do 								\
	{								\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
		USEC_DELAY();				\
	} while(0)

	#define HUNDRED_USEC_DELAY()	\
	do 								\
	{								\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
		TEN_USEC_DELAY();			\
	} while(0)
	
	
	// Not so accurate as previouse delays =)
	#define MSEC_DELAY_N(count)		\
	do 								\
	{								\
		int len = count * 10;		\
		int i = 0;					\
		while(i<len)				\
		{							\
			HUNDRED_USEC_DELAY();	\
			i++;					\
		}							\
	} while(0)						

	#define USEC192_DELAY() 		\
	do 								\
	{								\
		HUNDRED_USEC_DELAY();		\
		HUNDRED_USEC_DELAY();		\
	} while(0)
	
#endif
