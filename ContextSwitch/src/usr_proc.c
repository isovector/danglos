/**
 *@file:  usr_proc.c
 *@brief: Two user processes proc1 and proc2
 *@author: Irene Huang
 *@date: 2013/01/12
 */

#include "rtx.h"
#include "uart_polling.h"
#include "mmu.h"
#include "p_queue/p_queue.h"

#ifdef DEBUG_0
#include <stdio.h>
#include <stdlib.h>
#endif  /* DEBUG_0 */

int TEST_MEM_BLOCK = 0;
int TEST_PRIORITY_SWAP = 0;

volatile int i = 0;
volatile int j = 0;

int num_successful_tests = 0;

extern void debugPrint(unsigned char *);

extern uint32_t g_timer;


void null_proc(void)
{
	while(1)
	{
		release_processor();
		uart0_put_char((g_timer % 10) + '0');
	}
}

void proc_print(void)
{
    while ( 1 ) {
        uart0_put_string("\t(1) ping\n\r");
		release_processor();	
	}
}

/* Starts high */
void proc_priority_one (void)
{
	while ( i < 10 ) {
		i++;
		release_processor();
		if ( j > 0 ) {
			set_my_priority(3);
		}
	}
	set_my_priority(3);
	set_priority(0, 4);
	release_processor();
	if ( i == 10 && j == 10 ) {
		debugPrint("TEST 2 OK");
		num_successful_tests++;
	} else {
		debugPrint("TEST 2 FAIL");
	}
	uart0_put_string("G019_test: ");
	uart0_put_char(num_successful_tests + '0');
	uart0_put_string("/2 tests OK\r\n");
	
	uart0_put_string("G019_test: ");
	uart0_put_char((2 - num_successful_tests) + '0');
	uart0_put_string("/2 tests FAIL\r\n");
	
	debugPrint("END");
	while ( 1 ) {
		release_processor();
	}
}

/* Starts low */
void proc_priority_two (void)
{
	while ( j < 10 ) {
		j++;
		release_processor();
		if ( i != 10 ) {
			set_my_priority(3);
		}
	}
	set_my_priority(3);
	set_priority(0, 3);
	release_processor();
	
	while ( 1 ) {
		release_processor();
	}

}

void proc_allocAll(void){
	void* mem[100];
	volatile int i = 0;
	
	debugPrint("START");
	debugPrint("total 2 tests");
	
	while (mmu_can_alloc_mem())
	{
		mem[i++] = s_request_memory_block();
	}
    
	s_request_memory_block();
	TEST_MEM_BLOCK = 1;
	debugPrint("TEST 1 OK");
	num_successful_tests++;
	set_my_priority(3);
	set_priority(0, 3);
    
	while (i >= 0) {
		s_release_memory_block(mem[--i]);
	}
    
	for (;;) {
		release_processor();
	}
}

void proc_alloc1(void)
{
	void* m = s_request_memory_block();
	
	set_my_priority(2);
	release_processor();
	s_release_memory_block(m);
	set_my_priority(3);
    
	for (;;) {
		release_processor();
	}
}
