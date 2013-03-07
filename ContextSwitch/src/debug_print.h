#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
#define DEBUG_TEST

#ifdef DEBUG_TEST

#include "uart.h"



void debugPrint(unsigned char * str)
{
	static unsigned char * GROUP_SIGNATURE = "G019_test: ";
	/* Disable RBR, enable THRE before trying to write*/ 
	LPC_UART0->IER = IER_THRE | IER_RLS; 
	uart0_send_string(GROUP_SIGNATURE);		
	/* Re-enable RBR, THRE left as enabled */
	LPC_UART0->IER = IER_THRE | IER_RLS | IER_RBR;
	
	/* Disable RBR, enable THRE before trying to write*/ 
	LPC_UART0->IER = IER_THRE | IER_RLS; 
	uart0_send_string(str);	
	/* Re-enable RBR, THRE left as enabled */
	LPC_UART0->IER = IER_THRE | IER_RLS | IER_RBR;
	
	/* Disable RBR, enable THRE before trying to write*/ 
	LPC_UART0->IER = IER_THRE | IER_RLS; 
	uart0_send_string( "\r\n");
	/* Re-enable RBR, THRE left as enabled */
	LPC_UART0->IER = IER_THRE | IER_RLS | IER_RBR;

}

#else
void debugPrint(const char * str) {}
#endif

#endif
