#ifndef ZB_TRANSPORT_CORTEXM4_H
#define ZB_TRANSPORT_CORTEXM4_H 1

/*! \cond internals_doc */
/*! \addtogroup ZB_MAC */
/*! @{ */

//#include "zb_8051_serial.h"
#ifndef ZB_SNIFFER
//#include "zb_8051_spi.h"
#endif

#ifdef ZB_TRANSPORT_CORTEXM4_UART
	#include "zb_cortexm4_usart.h"
#endif

#ifdef ZB_TRANSPORT_CORTEXM4_DATA_SPI
	#include "zb_cortexm4_spi.h"

#endif
	
#include "zb_cortexm4_exti.h"

typedef struct zb_io_ctx_s
{
#ifndef ZB_SNIFFER
  zb_buf_t *send_data_buf;    /* pointer to zigbee data buffer to send, can be sent via UART or SPI */
  zb_buf_t *recv_data_buf;    /* pointer to buffer to receive to */
  zb_ushort_t bytes_to_recv;  /* bytes number to receive; if 0, calculate this number automatically */
  zb_uint8_t recv_finished;    /* receive status - ZB_NO_IO, ZB_IO_ERROR, ZB_RECV_PENDING, ZB_RECV_FINISHED */
  zb_uint8_t send_finished;    /* send status - ZB_NO_IO, ZB_IO_ERROR, ZB_SEND_IN_PROGRESS, ZB_SEND_FINISHED */
#endif
#ifdef ZB_TRANSPORT_CORTEXM4_UART
  zb_cortexm4_serial_ctx_t serial_ctx;
#endif
#ifdef ZB_TRANSPORT_CORTEXM4_DATA_SPI
  zb_spi_exch_t spi_ctx;
#endif
  zb_uint16_t int_counter;
} zb_io_ctx_t;


/*! @} */
/*! \endcond */

#endif /* ZB_TRANSPORT_CORTEXM4_H */
