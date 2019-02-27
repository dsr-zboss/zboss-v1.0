// ------------------------- MRF REGISTERS DEFINES -------------- like ubec.

#ifndef MRF24J40_REG_H_
#define MRF24J40_REG_H_
	#define ZB_NORMAL_TX_FIFO          0x000
	#define ZB_BEACON_TX_FIFO          0x080
	#define ZB_GTS1_TX_FIFO            0x100
	#define ZB_GTS2_TX_FIFO            0x180
	#define ZB_RX_FIFO                 0x300

	#define ZB_SREG_RXMCR     	        0x00
	
	#define ZB_SREG_PANIDL      	    0x01
	#define ZB_SREG_PANIDH     	        0x02
	
	#define ZB_SREG_SADRL				0x03
	#define ZB_SREG_SADRH				0x04
	
	#define ZB_SREG_EADR0     			0x05
	#define ZB_SREG_EADR1     			0x06
	#define ZB_SREG_EADR2     			0x07
	#define ZB_SREG_EADR3     			0x08
	#define ZB_SREG_EADR4     			0x09
	#define ZB_SREG_EADR5     			0x0A
	#define ZB_SREG_EADR6     			0x0B
	#define ZB_SREG_EADR7     			0x0C
	
	#define ZB_SREG_RXFLUSH            	0x0D
	#define ZB_SREG_ORDER              	0x10
	#define ZB_SREG_TXMCR              	0x11
	#define ZB_SREG_ACKTMOUT           	0x12
	#define ZB_SREG_FIFOEN             	0x18
	#define ZB_SREG_TXNCON            	0x1B
	#define ZB_SREG_TXPEND             	0x21
	#define ZB_SREG_WAKECTL            	0x22
	#define ZB_SREG_TXSTAT               	0x24
	#define ZB_SREG_RXSR               	0x30
	#define ZB_SREG_TXBCNMSK           	0x25
	#define ZB_SREG_SOFTRST            	0x2A
	#define ZB_SREG_SECCR0             	0x2C
	#define ZB_SREG_SECCR2             	0x37
	#define ZB_SREG_INTSTAT             	0x31
	#define ZB_SREG_INTMSK             	0x32
	#define ZB_SREG_SLPACK             	0x35
	#define ZB_SREG_RFCTL              	0x36
	#define ZB_SREG_BBREG0    			0x38
	#define ZB_SREG_BBREG1    			0x39
	#define ZB_SREG_BBREG2             	0x3A
	#define ZB_SREG_BBREG3             	0x3B
	#define ZB_SREG_BBREG4				0x3C
	#define ZB_SREG_BBREG5             	0x3D
	#define ZB_SREG_BBREG6             	0x3E
	#define ZB_SREG_BBREG7             	0x3F
	#define ZB_SREG_GATECLK            	0x26
	#define ZB_SREG_PACON1             	0x17
	#define ZB_SREG_TXPEMISP           	0x2E
	
	#define ZB_LREG_GPIODIR            	0x23D
	#define ZB_LREG_RFCTRL0            	0x200
	#define ZB_LREG_RFCTRL1            	0x201
	#define ZB_LREG_RFCTRL2            	0x202
	#define ZB_LREG_RFCTRL3				0x203
	#define ZB_LREG_RFCTRL4            	0x204
	#define ZB_LREG_RFCTRL6            	0x206
	#define ZB_LREG_RFCTRL7            	0x207
	#define ZB_LREG_RFCTRL8            	0x208
	#define ZB_LREG_RFCTL2             	0x202
	#define ZB_LREG_RFCTL3             	0x203
	#define ZB_LREG_RFCTL6             	0x206
	#define ZB_LREG_RFCTL7             	0x207
	#define ZB_LREG_RFCTL8             	0x208
	#define ZB_LREG_RFCTRL77           	0x277
	#define ZB_LREG_RFCTRL50           	0x250
	#define ZB_LREG_RFCTRL51           	0x251
	#define ZB_LREG_RFCTRL52           	0x252
	#define ZB_LREG_RFCTRL59           	0x259
	#define ZB_LREG_RFCTRL73           	0x273
	#define ZB_LREG_RFCTRL74           	0x274
	#define ZB_LREG_RFCTRL75           	0x275
	#define ZB_LREG_RFCTRL76           	0x276
	#define ZB_LREG_RSSI               	0x210
	#define ZB_LREG_IRQCTRL				0x211
	#define ZB_LREG_SADRCTRL           	0x212
	#define ZB_LREG_SRCADR_0           	0x213
	#define ZB_LREG_SRCADR_7           	0x21A
	#define ZB_LREG_HLEN               	0x21E
		
	#define ZB_LREG_SCLKDIV            	0x220
	#define ZB_LREG_TESTMODE           	0x22F
	#define ZB_LREG_ASSOEADR0          	0x230
	#define ZB_LREG_ASSOEADR1          	0x231
	#define ZB_LREG_ASSOEADR2          	0x232
	#define ZB_LREG_ASSOEADR3          	0x233
	#define ZB_LREG_ASSOEADR4          	0x234
	#define ZB_LREG_ASSOEADR5          	0x235
	#define ZB_LREG_ASSOEADR6          	0x236
	#define ZB_LREG_ASSOEADR7          	0x237
	#define ZB_LREG_ASSOSADR0          	0x238
	#define ZB_LREG_ASSOSADR1          	0x239
	#define ZB_LREG_RXFRMTYPE          	0x23C
	#define ZB_LREG_SECCTRL            	0x24D
		
	#define ZB_LREG_UPNONCE_0          	0x240
	#define ZB_LREG_UPNONCE_12         	0x24C
		
	#define ZB_LREG_KEY_0              	0x280
		
	#define TXMCR_SLOTTED_MASK         	0x20
	#define ZB_SREG_ORDER_FIRST_START_VAL  	0xFF
	
	#define CHANNEL_11 					0x00
	#define CHANNEL_12 					0x10
	#define CHANNEL_13 					0x20
	#define CHANNEL_14 					0x30
	#define CHANNEL_15 					0x40
	#define CHANNEL_16 					0x50
	#define CHANNEL_17 					0x60
	#define CHANNEL_18 					0x70
	#define CHANNEL_19 					0x80
	#define CHANNEL_20 					0x90
	#define CHANNEL_21 					0xa0
	#define CHANNEL_22 					0xb0
	#define CHANNEL_23 					0xc0
	#define CHANNEL_24 					0xd0
	#define CHANNEL_25 					0xe0
	#define CHANNEL_26 					0xf0
#endif
