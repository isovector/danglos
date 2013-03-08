#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
#define DEBUG_TEST

#ifdef DEBUG_TEST

#include "uart.h"



void debugPrint(unsigned char * str);

#else
void debugPrint(const char * str) {}
#endif

#endif
