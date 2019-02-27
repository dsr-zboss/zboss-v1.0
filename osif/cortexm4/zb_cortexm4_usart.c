// zb_cortexm4_usart
// Implementation of Serial Port communication for Cortex M4


#if defined cortexm4

#include "zb_common.h"
#include "zb_mac_transport.h"
#include "zb_bufpool.h"
#include "zb_ringbuffer.h"
#include "zb_g_context.h"
#include "zb_cortexm4_usart.h"
#include "stm32f4xx_conf.h"


void ZB_SERIAL_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure USART pins: Rx and Tx */
	/* Enable GPIO clocks */
	ZB_SERIAL_TX_GPIO_RCC_PeriphClock(ZB_SERIAL_TX_GPIO_CLK, ENABLE);
	ZB_SERIAL_RX_GPIO_RCC_PeriphClock(ZB_SERIAL_RX_GPIO_CLK, ENABLE);
	
	/* GPIO Deinitialisation */
	//GPIO_DeInit(ZB_SERIAL_TX_GPIO_PORT);
	//GPIO_DeInit(ZB_SERIAL_RX_GPIO_PORT);
	
	/* Connect USART pins to AF */
	GPIO_PinAFConfig(ZB_SERIAL_TX_GPIO_PORT, ZB_SERIAL_TX_SOURCE, ZB_SERIAL_TX_AF);  /* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(ZB_SERIAL_RX_GPIO_PORT, ZB_SERIAL_RX_SOURCE, ZB_SERIAL_RX_AF);  /* Connect PXx to USARTx_Rx*/

	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = ZB_SERIAL_TX_PIN;
	GPIO_InitStructure.GPIO_Mode  = ZB_SERIAL_TX_GPIO_MODE;
	GPIO_InitStructure.GPIO_OType = ZB_SERIAL_TX_GPIO_OTYPE;
	GPIO_InitStructure.GPIO_PuPd  = ZB_SERIAL_TX_GPIO_PUPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ZB_SERIAL_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure USART1 Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = ZB_SERIAL_RX_PIN;
	GPIO_InitStructure.GPIO_Mode  = ZB_SERIAL_RX_GPIO_MODE;
	GPIO_InitStructure.GPIO_OType = ZB_SERIAL_RX_GPIO_OTYPE;
	GPIO_InitStructure.GPIO_PuPd  = ZB_SERIAL_RX_GPIO_PUPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ZB_SERIAL_RX_GPIO_PORT, &GPIO_InitStructure);
}

void ZB_SERIAL_Configuration(void) {
	USART_InitTypeDef USART_InitStructure;

	/* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle
        - USART LastBit: The clock pulse of the last data bit is not output to
                         the SCLK pin
	*/
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	ZB_SERIAL_RCC_PeriphClock(ZB_SERIAL_CLK, ENABLE);     // RCC_APB2PeriphClockCmd
                                                        // RCC_APB2Periph_USART1

	USART_InitStructure.USART_BaudRate            = ZB_SERIAL_BAUD_RATE;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	//USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;	
	// USART1 ON, TX ON, RX ON
	USART_Init(ZB_SERIAL_DEV, &USART_InitStructure);
	USART_Cmd(ZB_SERIAL_DEV, ENABLE);      /* Enable USART controller */
}

void ZB_SERIAL_NVIC_Configuration(FunctionalState enable_int) {
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel                    = ZB_SERIAL_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = enable_int;
	NVIC_Init(&NVIC_InitStructure);
}


void ZB_SERIAL_Init(void)
{
	ZB_SERIAL_GPIO_Configuration();
	ZB_SERIAL_NVIC_Configuration(ENABLE);
	ZB_SERIAL_Configuration();
	NVIC_EnableIRQ(ZB_SERIAL_IRQChannel);
	USART_ITConfig(ZB_SERIAL_DEV, USART_IT_RXNE, DISABLE);
	USART_ITConfig(ZB_SERIAL_DEV, USART_IT_TXE,	 DISABLE);
}

// также внаглую спизженно. пусть например работает.
void USART1_IRQHandler()
{
	if (USART_GetITStatus(ZB_SERIAL_DEV, USART_IT_RXNE) != RESET) 
	{
	#ifdef ZB_SNIFFER
		start_channel = USART_ReceiveData(ZB_SERIAL_DEV);
		start_signal = (start_channel >= ZB_TRANSCEIVER_START_CHANNEL_NUMBER) &&
					   (start_channel <= ZB_TRANSCEIVER_MAX_CHANNEL_NUMBER);
		
		stop_signal = !start_signal;
	#endif /* ZB_SNIFFER */
	}

	if (USART_GetITStatus(ZB_SERIAL_DEV, USART_IT_TXE) != RESET) 
	{
		zb_uint8_t volatile *p = ZB_RING_BUFFER_PEEK(&SER_CTX().tx_buf);
		if (p) 
		{
		  USART_SendData(ZB_SERIAL_DEV, *p);
		  SER_CTX().tx_in_progress = 1;
		  ZB_RING_BUFFER_FLUSH_GET(&SER_CTX().tx_buf);
	//      ((&SER_CTX().tx_buf) -> written--, ((&SER_CTX().tx_buf) -> read_i = ((&SER_CTX().tx_buf)->read_i + 1) % ZB_RING_BUFFER_CAPACITY(&SER_CTX().tx_buf)));

		} 
		else 
		{                                    /* No more data */
		  SER_CTX().tx_in_progress = 0;
		  ZB_DISABLE_SERIAL_TR_INTER();
		}
	}
} 

// Внаглую спизжено из транка, переписать в соотствествии с действительностью и своими нуждами
// ну, собственно нужды на этом ограничиваются, код переписывать не буду =)
void zb_osif_serial_put_bytes(zb_uint8_t *buf, zb_short_t len)
{
	while(SER_CTX().tx_in_progress);

	while( len )
	{
		ZB_DISABLE_SERIAL_INTER();
		if( ZB_RING_BUFFER_IS_FULL(&SER_CTX().tx_buf) )
		{
			ZB_ENABLE_SERIAL_INTER();
			/* Note: this call flushes all the buffer. It is not optimal, but simple. */
			SER_CTX().tx_in_progress = 0;
			continue;
		}
		else
		{
			ZB_RING_BUFFER_PUT(&SER_CTX().tx_buf, *buf);
			buf++;
			len--;
		}
		{
			zb_ushort_t force_start = !SER_CTX().tx_in_progress;
			ZB_ENABLE_SERIAL_INTER();
			if( force_start )
			{
				ZB_ENABLE_SERIAL_TR_INTER();            /* Start transmit */
			}
		}
	}
}

#endif
