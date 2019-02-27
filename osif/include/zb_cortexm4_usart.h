// zb_cortexm4_usart.h

#if defined cortexm4

#ifndef ZB_CORTEXM4_SERIAL_H_
#define ZB_CORTEXM4_SERIAL_H_

#include "zb_osif.h"
#include "stm32f4xx_conf.h"
#include "zb_types.h"
#include "zb_ringbuffer.h"

ZB_RING_BUFFER_DECLARE(serial_iobuf, zb_uint8_t, 4); // I don't know why we use capacity = 4

typedef struct zb_cortexm4_serial_ctx_s
{
  zb_uint8_t      tx_in_progress; /* if set, we are waiting for tx complete int */
  serial_iobuf_t  rx_buf;
  serial_iobuf_t  tx_buf;
} zb_cortexm4_serial_ctx_t;

void ZB_SERIAL_GPIO_Configuration(void);
void ZB_SERIAL_Configuration(void);
void ZB_SERIAL_NVIC_Configuration(FunctionalState enable_int);
void ZB_SERIAL_Init(void);
void ZB_SERIAL_IRQHandler();
void zb_dump_put_buf(zb_uint8_t *buf, zb_uint_t len, zb_bool_t is_w);
void zb_osif_serial_put_bytes(zb_uint8_t *buf, zb_short_t len);

#define	ZB_DISABLE_SERIAL_INTER() 		NVIC_DisableIRQ(ZB_SERIAL_IRQChannel)
#define ZB_ENABLE_SERIAL_INTER()		NVIC_EnableIRQ(ZB_SERIAL_IRQChannel)

#define	ZB_ENABLE_SERIAL_TR_INTER()		USART_ITConfig(ZB_SERIAL_DEV, USART_IT_TXE, ENABLE)
#define	ZB_DISABLE_SERIAL_TR_INTER()	USART_ITConfig(ZB_SERIAL_DEV, USART_IT_TXE, DISABLE)


#endif
#endif
