#include "C:\KEIL\C51\INC\CYGNAL\C8051F120.H"
void function2();

sbit LED1 = P3^0;
sbit LED2 = P3^1;

void function1(int i)
{
	switch (i) 
	{
		case 1: 
			LED1 = 0;
			break;
		case 2:
			function2();
		default:
		break;
	}
}