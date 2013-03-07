#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
#define DEBUG_TEST

#ifdef DEBUG_TEST

#include "uart.h"



void debugPrint(unsigned char * str)
{
	static unsigned char * GROUP_SIGNATURE = "G019_test: ";

	uart0_send_string(GROUP_SIGNATURE);		

	uart0_send_string(str);	

	uart0_send_string( "\r\n");
}

#else
void debugPrint(const char * str) {}
#endif

#endif
