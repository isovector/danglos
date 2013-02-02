#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
#define DEBUG_TEST

#ifdef DEBUG_TEST



void debugPrint(unsigned char * str)
{
	static unsigned char * GROUP_SIGNATURE = "G019_test: ";
	uart0_put_string(GROUP_SIGNATURE);
	uart0_put_string(str);
	uart0_put_string("\r\n");
}
#else
void debugPrint(const char * str) {}
#endif

#endif
