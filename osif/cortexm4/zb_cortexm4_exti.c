// zb_cortexm4_exti.c
#include "zb_common.h"
#include "zb_cortexm4_exti.h"
#include "zb_g_context.h"
#include "zb_mrf24j40.h"

#if defined cortexm4

/* TODO: Move this configuration to application layer */
void LED_BUTTON_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_Led;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // LED
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // Button
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

 	GPIO_Led.GPIO_Pin = GPIO_Pin_13;
 	GPIO_Led.GPIO_Mode = GPIO_Mode_OUT;
 	GPIO_Led.GPIO_OType = GPIO_OType_PP;
 	GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Led.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_Init(GPIOC, &GPIO_Led); // Init LED

	GPIO_Led.GPIO_Pin = GPIO_Pin_0;
 	GPIO_Led.GPIO_Mode = GPIO_Mode_IN;
 	GPIO_Led.GPIO_OType = GPIO_OType_PP;
 	GPIO_Led.GPIO_PuPd = GPIO_PuPd_DOWN;
 	GPIO_Led.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_Led); // Init Button

#ifdef STM32F4_DISCOVERY
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // LED

 	GPIO_Led.GPIO_Pin = GPIO_Pin_15;
 	GPIO_Led.GPIO_Mode = GPIO_Mode_OUT;
 	GPIO_Led.GPIO_OType = GPIO_OType_PP;
 	GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Led.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_Init(GPIOD, &GPIO_Led); // Init LED

 	GPIO_Led.GPIO_Pin = GPIO_Pin_14;
 	GPIO_Led.GPIO_Mode = GPIO_Mode_OUT;
 	GPIO_Led.GPIO_OType = GPIO_OType_PP;
 	GPIO_Led.GPIO_PuPd = GPIO_PuPd_NOPULL;
 	GPIO_Led.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_Init(GPIOD, &GPIO_Led); // Init LED
#endif

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	EXTI_InitTypeDef exti_struct;
	exti_struct.EXTI_Line = EXTI_Line0;
	exti_struct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_struct.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_struct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_struct);


	NVIC_InitTypeDef NVIC_InitStructure;
	  /* Enable the Button External Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel                    = EXTI0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x01;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x01;
 	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}


// User button interrupt handler. Do nothing except the led.
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0))
	{
		EXTI->PR = EXTI_Line0;
		//USART_SendData(ZB_SERIAL_DEV,'i');
		//GPIO_ToggleBits(GPIOC,GPIO_Pin_13);
		//ZB_SET_TRANS_INT();
                //TRANS_CTX().interrupt_flag = 1;
		EXTI->PR = EXTI_Line0;
	}
}



// Radio interrupt pin handler. Toggle Led.
void EXTI9_5_IRQHandler(void)
{
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	if (EXTI->PR & ZB_SPIx_RFINT_LINE)	// If we have pending bit on Line 9
	{
		EXTI->PR = ZB_SPIx_RFINT_LINE;	// Clear, writing it to 1
		ZB_SET_TRANS_INT();
                TRANS_CTX().interrupt_flag = 1;
	}
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void RF_INT_Configuration(void)
{
  RCC_AHB1PeriphClockCmd(ZB_SPIx_RFINT_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	EXTI_InitTypeDef exti_struct;
	GPIO_InitTypeDef  GPIO_Led;

	GPIO_Led.GPIO_Pin = ZB_SPIx_RFINT_PIN;
 	GPIO_Led.GPIO_Mode  = GPIO_Mode_IN;
 	GPIO_Led.GPIO_OType = GPIO_OType_PP;
 	GPIO_Led.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
 	GPIO_Led.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(ZB_SPIx_RFINT_GPIO_PORT, &GPIO_Led);

	SYSCFG_EXTILineConfig(ZB_SPIx_RFINT_PORTSOURCE, ZB_SPIx_RFINT_SOURCE);

	exti_struct.EXTI_Line = ZB_SPIx_RFINT_LINE;
	exti_struct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_struct.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_struct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_struct);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel                    = ZB_SPIx_RFINT_IRQCHANNEL;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 0x00;
 	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

static volatile unsigned int _Continue;

void HardFault_Handler(void)
{
 _Continue = 0u;
 //
 // When stuck here, change the variable value to != 0 in order to step out
 //
 while (_Continue == 0u);
}

#endif
