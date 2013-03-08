#include "debug_print.h"

void debugPrint(unsigned char * str)
{
	static unsigned char * GROUP_SIGNATURE = "G019_test: ";
	uart0_send_string(GROUP_SIGNATURE);		
	uart0_send_string(str);	
	uart0_send_string( "\r\n");
}
