// zb_cortexm4_exti.h
#ifdef ZB_CORTEXM4_EXTI_H_
#define ZB_CORTEXM4_EXTI_H_
#if defined cortexm4

#include "zb_types.h"
#include <stm32f4xx_conf.h>
// Configuration of Green led and user button on board.
void LED_BUTTON_Configuration(void);
// user button interrupt handler
void EXTI0_IRQHandler(void);

// Radio Interrupt-pin configuration
void RF_INT_Configuration(void);
// Radio Interrupt-pin handler
void EXTI9_5_IRQHandler(void);

#endif
#endif
