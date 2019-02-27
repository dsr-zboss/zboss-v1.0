/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*          Copyright (c) 2011 ClarIDy Solutions, Inc., Taipei, Taiwan.     *
*                       http://www.claridy.com/                            *
*                                                                          *
*          Copyright (c) 2011 Uniband Electronic Corporation (UBEC),       *
*                             Hsinchu, Taiwan.                             *
*                       http://www.ubec.com.tw/                            *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack, you*
* may choose which license to receive this code under (except as noted in  *
* per-module LICENSE files).                                               *
*                                                                          *
* ZBOSS is a registered trademark of DSR Corporation AKA Data Storage      *
* Research LLC.                                                            *
*                                                                          *
* GNU General Public License Usage                                         *
* This file may be used under the terms of the GNU General Public License  *
* version 2.0 as published by the Free Software Foundation and appearing   *
* in the file LICENSE.GPL included in the packaging of this file.  Please  *
* review the following information to ensure the GNU General Public        *
* License version 2.0 requirements will be met:                            *
* http://www.gnu.org/licenses/old-licenses/gpl-2.0.html.                   *
*                                                                          *
* Commercial Usage                                                         *
* Licensees holding valid ClarIDy/UBEC/DSR Commercial licenses may use     *
* this file in accordance with the ClarIDy/UBEC/DSR Commercial License     *
* Agreement provided with the Software or, alternatively, in accordance    *
* with the terms contained in a written agreement between you and          *
* ClarIDy/UBEC/DSR.                                                        *
*                                                                          *
****************************************************************************
PURPOSE: Cortex-M4 timer implementation.
*/

#ifndef ZB_CORTEXM4_TIME_H
#define ZB_CORTEXM4_TIME_H

//#include "zb_time.h"
#include "stm32f4xx.h"
#include "misc.h"

/*! \addtogroup ZB_OSIF_CORTEXM4 */
/*! @{ */

/* ---------------------- SysTick registers bit mask -------------------- */
/* CTRL TICKINT Mask */
#define CTRL_TICKINT_Set      			(0x00000002)
#define CTRL_TICKINT_Reset    			(0xFFFFFFFD)

/* SysTick counter state */
#define SysTick_Counter_Disable      	(0xFFFFFFFE)
#define SysTick_Counter_Enable         	(0x00000001)
#define SysTick_Counter_Clear          	(0x00000000)


/* SysTick Flag */
#define SysTick_FLAG_COUNT             	((u32)0x00000010)
#define SysTick_FLAG_SKEW              	((u32)0x0000001E)
#define SysTick_FLAG_NOREF             	((u32)0x0000001F)

#define SysTick_COUNT_FLAG 				(0x00010000)

/*
  1 operation time = 1/ZB_XTAL_FREQ
  ticks count (per 1 msec) = 1 msec / operation_time (msec)
*/


#define SYSTICKCLOCK      (SystemCoreClock / 8)     /* SysTic clock freq, Hz */
#define SYSTICKCLOCKINIT  ((SYSTICKCLOCK/1000000) * ZB_BEACON_INTERVAL_USEC)

void zb_osif_timer_init(void);
//void SysTick_CounterCmd(u32 SysTick_Counter);
//void SysTick_ITConfig(FunctionalState NewState);
//FlagStatus SysTick_GetFlagStatus(u8 SysTick_FLAG);

//void ZB_cm4_sub_ghz_SysTick_start(void);
//void ZB_cm4_sub_ghz_SysTick_stop(void);

#define ENABLE_TIMER()				(SysTick->CTRL |= SysTick_Counter_Enable)
#define DISABLE_TIMER()				(SysTick->CTRL &= SysTick_Counter_Disable)

#define ENABLE_TIMER_INT()			(SysTick->CTRL |= CTRL_TICKINT_Set)
#define DISABLE_TIMER_INT()			(SysTick->CTRL &= CTRL_TICKINT_Reset)

#define SET_TIMER_RELOAD_VALUE(a)	(SysTick->LOAD |= (a))
#define CLEAR_TIMER_VAL()			(SysTick->VAL = 0)
#define IS_COUNTFLAG_ON()			(SysTick->CTRL & SysTick_COUNT_FLAG)

#define ZB_TIMER_INIT()       zb_osif_timer_init()

#define ZB_START_HW_TIMER()   do {\
								ENABLE_TIMER();\
								ENABLE_TIMER_INT();\
							  }while(0)
							  
#define ZB_STOP_HW_TIMER()    (DISABLE_TIMER_INT())

//#define ZB_CHECK_TIMER_IS_ON() (SysTick_GetFlagStatus(SysTick_FLAG_COUNT) == SET) /* TRUE if timer is ON */
#define ZB_CHECK_TIMER_IS_ON()	(IS_COUNTFLAG_ON())

/*! @} */
#endif /* ZB_CORTEX_SUB_GHZ_TIME_H */
