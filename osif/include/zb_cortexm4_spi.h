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
FILE: zb_cortex_sub_ghz_spi.h
PURPOSE: declare SPI for Cortex-M4 & CC120X.
*/

#ifndef ZB_CORTEXM4_SPI_H
#define ZB_CORTEXM4_SPI_H

#include "zb_types.h"
#include <stm32f4xx_conf.h>

#define ZB_NO_SPI_IO_REQUEST 0
#define ZB_SPI_RX_REQUEST    1
#define ZB_SPI_TX_REQUEST    2


typedef zb_uint8_t zb_rfStatus_t;


#define MAXTR   130



typedef struct zb_spi_exch_st 
{
	zb_uint16_t tx_buf[MAXTR];
	zb_uint16_t rx_buf[MAXTR];
	zb_uint8_t to_tr;
	zb_uint8_t to_rv;
	zb_uint8_t trd;
	zb_uint8_t rvd;
	FlagStatus in_progress;
	zb_uint8_t   err_in_oper;        /* Error while operation  */
} zb_spi_exch_t;


//extern zb_uint8_t     rfint1_fl;


void 	ZB_SPI_GPIO_Configuration(void);
void 	ZB_SPI_Configuration(void);
void 	ZB_SPI_NVIC_Configuration(void);
void  	zb_spi_init(void);	

void 	ZB_SPIx_IRQHANDLER();

void	spi_exchange_via_int();
#define SPI_EXCH()		spi_exchange_via_int()
int test_spi();
//void  ZB_SPI_Set_mode(zb_uint16_t SPI_DataSize);
/*
void  select_MRF24J40();	
void  unselect_MRF24J40();	
*/
/*
void  reset_CC120X(void);
void  wakeup_CC120X(void);
void  wait_CC120X_TXE(void);
void  wait_CC120X_RXNE(void);
void  clear_CC120X_RXNE(void);

void  RfReset_pin_Init(void);
void  EXTI_RFINT1_Init(void);

void EXTI_RFINT1_Configuration(FunctionalState enable_int);
#define ZB_DISABLE_CC120X_INTER() EXTI_RFINT1_Configuration(DISABLE)
#define ZB_ENABLE_CC120X_INTER()  EXTI_RFINT1_Configuration(ENABLE)

void zb_stm_abort(void);

void zb_cortexm4_Delay(volatile zb_int32_t nCount);
#define zb_cortexm4_uS_DELAY(tm)  zb_cortexm4_Delay((tm * 114)/7)
*/
#endif /* ZB_CORTEXM4_SPI_H */
