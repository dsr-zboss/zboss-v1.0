#ifndef ZB_OSIF_CORTEXM4_H_
#define ZB_OSIF_CORTEXM4_H_
#include <stdlib.h>
#include <string.h>
#include "zb_types.h"
#include "zb_cortexm4_usart.h"
#include "zb_cortexm4_exti.h"
//#include "zb_ubec24xx.h"
//Redefine for current platform
	#define ZB_VOLATILE 	volatile
	#define ZB_MEMCPY 		memcpy
	#define ZB_MEMMOVE 		memmove
	#define ZB_MEMSET 		memset
	#define ZB_MEMCMP 		memcmp
	#define ZB_BZERO(s,l) 	ZB_MEMSET((char*)(s), 0, (l))
	#define ZB_BZERO2(s) 	ZB_BZERO(s, 2)
	#define ZVUNUSED(v) 	(void)v

	zb_uint16_t zb_random();		// реализована в common стека
	#define ZB_RANDOM() 	zb_random()
	
	typedef zb_uint32_t zb_minimal_vararg_t;

// Old sdcc 8051 defines. Not needed in cortex.

	#define ZB_SDCC_XDATA
	#define ZB_CALLBACK
	#define ZB_SDCC_REENTRANT
	#define ZB_SDCC_BANKED
	#define ZB_KEIL_REENTRANT

// А это пошли заглушки, которые надо нормально заменить
	#define CHECK_INT_N_TIMER() // даже в 8051 не используется	
	#define ZB_ABORT()			// яхз.
#ifdef ZB_TRACE_LEVEL
  #define ZB_START_DEVICE() (ZB_TIMER_INIT(), ZB_SERIAL_Init(), LED_BUTTON_Configuration() )
#else
  #define ZB_START_DEVICE() (ZB_TIMER_INIT(), LED_BUTTON_Configuration() ) //ZB_SERIAL_Init(),
#endif

	#define ZB_ENABLE_ALL_INTER()		__enable_irq()	// вот зачем они здесь?
	#define ZB_DISABLE_ALL_INTER()		__disable_irq()	// наверное как у всех чтоб было.
	
	#define ZB_OSIF_GLOBAL_LOCK()   	ZB_DISABLE_ALL_INTER()
	#define ZB_OSIF_GLOBAL_UNLOCK() 	ZB_ENABLE_ALL_INTER()

	#define ZB_GO_IDLE()
	#define ZB_TRY_IO()
	
	#define ZB_ENABLE_TRANSIVER_INT()	NVIC_EnableIRQ(EXTI9_5_IRQn)
	#define ZB_DISABLE_TRANSIVER_INT()	NVIC_DisableIRQ(EXTI9_5_IRQn)
/*
void zb_mac_transport_init() должна быть заменена на нормальную с инициализацией spi и, возможно, радио.

*/



#endif
