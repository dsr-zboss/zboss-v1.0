/***************************************************************************
*                      ZBOSS ZigBee Pro 2007 stack                         *
*                                                                          *
*          Copyright (c) 2013 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*          Copyright (c) 2012 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*          Copyright (c) 2011 DSR Corporation Denver CO, USA.              *
*                       http://www.dsr-wireless.com                        *
*                                                                          *
*                            All rights reserved.                          *
*                                                                          *
*                                                                          *
* ZigBee Pro 2007 stack, also known as ZBOSS (R) ZB stack is available     *
* under either the terms of the Commercial License or the GNU General      *
* Public License version 2.0.  As a recipient of ZigBee Pro 2007 stack,    *
* you may choose which license to receive this code under (except as noted *
* in per-module LICENSE files).                                            *
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
* Licensees holding valid DSR Commercial licenses may use this file        *
* in accordance with the DSR Commercial License Agreement provided with    *
* the Software or, alternatively, in accordance with the terms contained   *
* in a written agreement between you and DSR.                              *
****************************************************************************
PURPOSE:
*/


#ifndef __ZB_OLIMEX_STM32_E407_H
#define __ZB_OLIMEX_STM32_E407_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"

#define QZFR    12                       /* 12 MHz */


/* ZB SPI */
#define ZB_SPIx                           SPI2
#define ZB_SPIx_CLK                       RCC_APB1Periph_SPI2
#define ZB_SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define ZB_SPIx_IRQn                      SPI2_IRQn
#define ZB_SPIx_IRQHANDLER                SPI2_IRQHandler

/* ZB SPI PINS  */
/* SCK PB.10 */
#define ZB_SPIx_SCK_PIN                   GPIO_Pin_10
#define ZB_SPIx_SCK_GPIO_PORT             GPIOB
#define ZB_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define ZB_SPIx_SCK_SOURCE                GPIO_PinSource10
#define ZB_SPIx_SCK_AF                    GPIO_AF_SPI2

/* MISO PC.2 */
#define ZB_SPIx_MISO_PIN                  GPIO_Pin_2
#define ZB_SPIx_MISO_GPIO_PORT            GPIOC
#define ZB_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define ZB_SPIx_MISO_SOURCE               GPIO_PinSource2
#define ZB_SPIx_MISO_AF                   GPIO_AF_SPI2

/* MOSI PC.3 */
#define ZB_SPIx_MOSI_PIN                  GPIO_Pin_3
#define ZB_SPIx_MOSI_GPIO_PORT            GPIOC
#define ZB_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define ZB_SPIx_MOSI_SOURCE               GPIO_PinSource3
#define ZB_SPIx_MOSI_AF                   GPIO_AF_SPI2

/* NSS(SEL) PG.10 */
#define ZB_SPIx_NSS_PIN                   GPIO_Pin_10
#define ZB_SPIx_NSS_GPIO_PORT             GPIOG
#define ZB_SPIx_NSS_GPIO_CLK              RCC_AHB1Periph_GPIOG

/* RF Int */
#define ZB_SPIx_RFINT_PIN                GPIO_Pin_7
#define ZB_SPIx_RFINT_GPIO_PORT          GPIOC
#define ZB_SPIx_RFINT_GPIO_CLK           RCC_AHB1Periph_GPIOC
#define ZB_SPIx_RFINT_SOURCE             GPIO_PinSource7
#define ZB_SPIx_RFINT_PORTSOURCE         EXTI_PortSourceGPIOC

#define ZB_SPIx_RFINT_LINE               EXTI_Line7
#define ZB_SPIx_RFINT_IRQHANDLER         EXTI9_5_IRQHandler
#define ZB_SPIx_RFINT_IRQCHANNEL         EXTI9_5_IRQn

/* RF Reset_N PB.8 */
#define ZB_SPIx_RFRES_PIN                 GPIO_Pin_8
#define ZB_SPIx_RFRES_GPIO_PORT           GPIOB
#define ZB_SPIx_RFRES_GPIO_CLK            RCC_AHB1Periph_GPIOB


/* Leds */
/** @addtogroup LOW_LEVEL_LED
  * @{
  */
typedef enum {
  LED1 = 0,
} Led_TypeDef;

#define LEDn 1

#define LED1_PIN            GPIO_Pin_13
#define LED1_GPIO_PORT      GPIOC
#define LED1_GPIO_CLK       RCC_AHB1Periph_GPIOC

extern GPIO_TypeDef*  LEDS_GPIO_PORTS[];
extern const uint16_t LEDS_GPIO_PINS[];
extern const uint32_t LEDS_GPIO_CLKS[];

#define   BAUD_RATE_1200        1200
#define   BAUD_RATE_2400        2400
#define   BAUD_RATE_9600        9600
#define   BAUD_RATE_19200       19200
#define   BAUD_RATE_38400       38400
#define   BAUD_RATE_57600       57600
#define   BAUD_RATE_115200      115200

#define   ZB_SERIAL_BAUD_RATE   BAUD_RATE_115200

#define ZB_SERIAL_DEV             USART1
#define ZB_SERIAL_IRQHandler      USART1_IRQHandler
#define ZB_SERIAL_IRQChannel      USART1_IRQn
#define ZB_SERIAL_CLK             RCC_APB2Periph_USART1
#define ZB_SERIAL_RCC_PeriphClock RCC_APB2PeriphClockCmd

/* USART_TX PB.06 */
#define ZB_SERIAL_TX_PIN                    GPIO_Pin_6
#define ZB_SERIAL_TX_GPIO_PORT              GPIOB
#define ZB_SERIAL_TX_GPIO_MODE              GPIO_Mode_AF
#define ZB_SERIAL_TX_GPIO_OTYPE             GPIO_OType_PP
#define ZB_SERIAL_TX_GPIO_PUPD              GPIO_PuPd_UP
#define ZB_SERIAL_TX_GPIO_RCC_PeriphClock   RCC_AHB1PeriphClockCmd
#define ZB_SERIAL_TX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define ZB_SERIAL_TX_SOURCE                 GPIO_PinSource6
#define ZB_SERIAL_TX_AF                     GPIO_AF_USART1

/* USART_RX PB.07 */
#define ZB_SERIAL_RX_PIN                    GPIO_Pin_7
#define ZB_SERIAL_RX_GPIO_PORT              GPIOB
#define ZB_SERIAL_RX_GPIO_MODE              GPIO_Mode_AF
#define ZB_SERIAL_RX_GPIO_OTYPE             GPIO_OType_PP
#define ZB_SERIAL_RX_GPIO_PUPD              GPIO_PuPd_UP
#define ZB_SERIAL_RX_GPIO_RCC_PeriphClock   RCC_AHB1PeriphClockCmd
#define ZB_SERIAL_RX_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define ZB_SERIAL_RX_SOURCE                 GPIO_PinSource7
#define ZB_SERIAL_RX_AF                     GPIO_AF_USART1


/* Time constant for the delay caclulation allowing to have a millisecond
   incrementing counter. This value should be equal to (System Clock / 1000).
   ie. if system clock = 168MHz then sEE_TIME_CONST should be 168. */
#define sEE_TIME_CONST                   168

#ifdef __cplusplus
}
#endif

#endif /* __ZB_OLIMEX_STM32_E407_H */
