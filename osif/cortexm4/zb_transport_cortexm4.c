// zb_transport_cortexm4.c
#include "zb_common.h"
#if defined cortexm4

//#include "zb_bufpool.h"
//#include "zb_ringbuffer.h"
//#include "zb_scheduler.h"
#include "zb_mac_transport.h"
#include "zb_cortexm4_spi.h"
#include "zb_transport_cortexm4.h"
#include "zb_mrf24j40.h"




void zb_mac_transport_init()
{
	TRACE_MSG(TRACE_COMMON1, ">> zb_mac_transport_init()", (FMT__0));
	
	zb_spi_init();
	
	#ifdef ZB_MRF24J40
	
	RF_INT_Configuration();
	zb_init_mrf24j40();
	#endif
	
	TRACE_MSG(TRACE_COMMON1, "<< zb_mac_transport_init()", (FMT__0));
}

void spi_exchange_via_int()
{
	while (SPI_CTX().in_progress == 1)
		; // Wait while busy
	SPI_CTX().in_progress = 1; // Make structure busy
	SPI_CTX().trd = 0;			// bytes transmitted = 0
	SPI_CTX().rvd = 0;			// bytes received = 0
	SPI_ITConfig(ZB_SPIx,SPI_I2S_IT_TXE,ENABLE);	// start transmission.
	while (SPI_CTX().in_progress == 1)	// Waiting interrupt handler finish.
		;
}

void zb_mac_transport_put_data(zb_buf_t *buf)
{
	(void*)buf;
	TRACE_MSG(TRACE_COMMON1, ">> zb_mac_transport_put_data()", (FMT__0));
}

void zb_mac_transport_start_recv(zb_buf_t *buf, zb_short_t bytes_to_recv)
{
	(void*)buf;
	(void)bytes_to_recv;
	TRACE_MSG(TRACE_COMMON1, ">> zb_mac_transport_start_recv()", (FMT__0));
}

#endif
