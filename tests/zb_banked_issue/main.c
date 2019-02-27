void function1(int i);
#include "C:\KEIL\C51\INC\CYGNAL\C8051F120.H"



#pragma OT(0);

void _init(void)
{
    WDTCN = 0xDE;  
    WDTCN = 0xAD;
    SFRPAGE = 0xf;
    OSCICN = 0x83; 
    while(!(OSCICN & 0x40));
    XBR0 = 0xf7;//reconfigure (for ex. to 0x06)
    XBR1 = 0x19; 
    XBR2 = 0x44; 
    P0MDOUT = 0x34; 
    P1MDOUT = 0xFF;
    P2MDOUT = 0xEF;
    P3MDOUT = 0xFF;
    SFRPAGE = 0x0;

}


void main()
{
 	_init();
    function1(1);
	function1(2);
	function1(1);
}



