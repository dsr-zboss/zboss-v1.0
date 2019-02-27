// Cortex M4 functions for timer.
#if defined cortexm4

#include "zb_common.h"
#include "zb_osif.h"
#include "stm32f4xx.h"
#include "zb_cortexm4_time.h"

// It's like functions but without any call-stuff. So that's why these defines is here.
#define ENABLE_TIMER()				(SysTick->CTRL |= SysTick_Counter_Enable)
#define DISABLE_TIMER()				(SysTick->CTRL &= SysTick_Counter_Disable)

#define ENABLE_TIMER_INT()			(SysTick->CTRL |= CTRL_TICKINT_Set)
#define DISABLE_TIMER_INT()			(SysTick->CTRL &= CTRL_TICKINT_Reset)

#define SET_TIMER_RELOAD_VALUE(a)	(SysTick->LOAD |= (a))
#define CLEAR_TIMER_VAL()			(SysTick->VAL = 0)
#define IS_COUNTFLAG_ON()			(SysTick->CTRL & SysTick_COUNT_FLAG)

void zb_osif_timer_init(void)
{

  /* SysTick interrupt each beacon interval (15360 uS) with SysTick Clock equal to (HCLK/8) */
  if (SysTick_Config(SYSTICKCLOCKINIT))
  {
    while (1);        /* Capture error */
  }

  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);   /* Select AHB clock(HCLK) divided by 8 as SysTick clock source */

  /* Set SysTick Preemption Priority to 1 */
//    NVIC_SetPriority(SysTick_IRQn, 0x04);
}


void SysTick_Handler(void)
{
  zb_time_t timer, timer_stop;

  ZB_TIMER_CTX().timer++;

  /* assign to prevent warnings in IAR */
  timer      = ZB_TIMER_CTX().timer;
  timer_stop = ZB_TIMER_CTX().timer_stop;


  if (!ZB_TIMER_CTX().started || ZB_TIME_GE(timer, timer_stop))
  {
    ZB_STOP_HW_TIMER();
    ZB_TIMER_CTX().timer_stop = ZB_TIMER_CTX().timer;
    ZB_TIMER_CTX().started = 0;
  }
  else
  {
    ZB_START_HW_TIMER();
  }
}

#endif
