// zb_cortexm4_spi.c

#if defined cortexm4

#include "zb_common.h"
#include "zb_cortexm4_spi.h"

#include "zb_g_context.h"
//#include "zb_trace.h" // remove after debug



void ZB_SPI_GPIO_Configuration(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	ZB_SPIx_CLK_INIT(ZB_SPIx_CLK, ENABLE);
	
	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(ZB_SPIx_SCK_GPIO_CLK  |
						ZB_SPIx_MISO_GPIO_CLK |
						ZB_SPIx_MOSI_GPIO_CLK |
						ZB_SPIx_NSS_GPIO_CLK, ENABLE);
	

	GPIO_PinAFConfig(ZB_SPIx_SCK_GPIO_PORT,  ZB_SPIx_SCK_SOURCE,  ZB_SPIx_SCK_AF);
	GPIO_PinAFConfig(ZB_SPIx_MISO_GPIO_PORT, ZB_SPIx_MISO_SOURCE, ZB_SPIx_MISO_AF);
	GPIO_PinAFConfig(ZB_SPIx_MOSI_GPIO_PORT, ZB_SPIx_MOSI_SOURCE, ZB_SPIx_MOSI_AF);

	/* For all pins */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin   = ZB_SPIx_SCK_PIN;
	GPIO_Init(ZB_SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  ZB_SPIx_MOSI_PIN;
	GPIO_Init(ZB_SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin   = ZB_SPIx_MISO_PIN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // GPIO_PuPd_NOPULL
	GPIO_Init(ZB_SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);
        
	/* SPI  NSS pin configuration */
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin   = ZB_SPIx_NSS_PIN;
	GPIO_Init(ZB_SPIx_NSS_GPIO_PORT, &GPIO_InitStructure);
  	GPIO_SetBits(ZB_SPIx_NSS_GPIO_PORT, ZB_SPIx_NSS_PIN);   /* High level after init */

        /* RST PIN */
        GPIO_InitStructure.GPIO_Pin =  ZB_SPIx_RFRES_PIN;
//        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(ZB_SPIx_RFRES_GPIO_PORT, &GPIO_InitStructure);

        GPIO_SetBits(ZB_SPIx_RFRES_GPIO_PORT, ZB_SPIx_RFRES_PIN);
/*
        GPIO_ResetBits(ZB_SPIx_RFRES_GPIO_PORT, ZB_SPIx_RFRES_PIN);
        USEC192_DELAY();
        USEC192_DELAY();
        
        GPIO_SetBits(ZB_SPIx_RFRES_GPIO_PORT, ZB_SPIx_RFRES_PIN);
        USEC192_DELAY();
        USEC192_DELAY();
        USEC192_DELAY();
*/
}



void ZB_SPI_Configuration(void) 
{

	SPI_InitTypeDef SPI_InitStructure;
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	ZB_SPIx_CLK_INIT(ZB_SPIx_CLK, ENABLE);
	
	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(ZB_SPIx);
	SPI_InitStructure.SPI_Direction   = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;                  /* + */
	SPI_InitStructure.SPI_CPOL        = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA        = SPI_CPHA_1Edge;
	
	SPI_InitStructure.SPI_NSS         = SPI_NSS_Soft;
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;     /* 2-256 */
	SPI_InitStructure.SPI_FirstBit      = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 10;
	
	SPI_Init(ZB_SPIx, &SPI_InitStructure);
	/* Enable SPI NSS output for master mode */
	SPI_SSOutputCmd(ZB_SPIx, ENABLE);
	SPI_Cmd(ZB_SPIx, ENABLE);               /* Enable SPI */
}

void ZB_SPI_NVIC_Configuration(void) 
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* enable SPI irq (with higher priority)   */
	NVIC_InitStructure.NVIC_IRQChannel                    = ZB_SPIx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void zb_spi_init(void) 
{
	TRACE_MSG(TRACE_COMMON1, "zb_spi_init",(FMT__0));
	ZB_SPI_GPIO_Configuration();
	ZB_SPI_Configuration();
	SPI_DataSizeConfig(ZB_SPIx, SPI_DataSize_8b);
	
	ZB_SPI_NVIC_Configuration();
	
	//TRACE_MSG(TRACE_COMMON1, "zb_spi_init()", (FMT__0));
	
	SPI_I2S_ITConfig(ZB_SPIx, SPI_I2S_IT_RXNE, DISABLE);
	SPI_I2S_ITConfig(ZB_SPIx, SPI_I2S_IT_TXE,  DISABLE);
	SPI_I2S_ITConfig(ZB_SPIx, SPI_I2S_IT_ERR,  DISABLE);
}



void ZB_SPIx_IRQHANDLER()
{
	//TRACE_MSG(TRACE_COMMON1, "ZB_SPIx_IRQHANDLER",(FMT__0));
	if (SPI_I2S_GetITStatus(ZB_SPIx,SPI_I2S_IT_RXNE) == SET)
	{
		zb_uint16_t space = SPI_I2S_ReceiveData(ZB_SPIx);
		if (SPI_CTX().rvd < SPI_CTX().to_rv)
		{
			SPI_CTX().rx_buf[SPI_CTX().rvd++] = space;
		}
		SPI_I2S_ClearITPendingBit(ZB_SPIx,SPI_I2S_IT_RXNE);		
	}
	
	if (SPI_I2S_GetITStatus(ZB_SPIx,SPI_I2S_IT_TXE) == SET)
	{

		if (SPI_CTX().trd<SPI_CTX().to_tr)
		{
			SPI_I2S_SendData(ZB_SPIx,SPI_CTX().tx_buf[SPI_CTX().trd++]);
		}
		else
		{
			SPI_CTX().in_progress = 0;
			SPI_I2S_ITConfig(ZB_SPIx,SPI_I2S_IT_TXE,DISABLE);
		}
		SPI_I2S_ClearITPendingBit(ZB_SPIx,SPI_I2S_IT_TXE);
	}
	
	if (SPI_I2S_GetITStatus(ZB_SPIx,SPI_I2S_IT_ERR) == SET)
	{
		
	}
	
}

int test_spi()
{
	TRACE_MSG(TRACE_COMMON1, "test_spi",(FMT__0));
	int ret = 0;
	SPI_CTX().tx_buf[0] = 0x55;
	SPI_CTX().tx_buf[1] = 0xaa;
	SPI_CTX().to_tr = 2;
	SPI_CTX().to_rv = 2;
	//SELECT_RADIO();
	SPI_EXCH();
	
	if (SPI_CTX().rx_buf[0] == 0x55 && SPI_CTX().rx_buf[1] == 0xaa)
		ret = 8;
	else
		ret = 25;
	SPI_CTX().rx_buf[0] = 0;
	SPI_CTX().rx_buf[1] = 0;
	//DESELECT_RADIO();
	
}

#endif
