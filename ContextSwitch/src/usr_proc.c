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
#endif  /* DEBUG_0 */

void null_proc(void)
{
	while(1)
	{
		release_processor();
	}
}

void proc_print(void)
{
  while ( 1 ) {
    uart0_put_string("\t(1) ping\n\r");
		release_processor();	
	}

}

void proc_priority(void)
{
	while ( 1 ) {
		uart0_put_string("(2) Step Priority Highest\n\r");
		set_priority(0);
		release_processor();
		
		uart0_put_string("(2) Step Priority Med\n\r");
		set_priority(2);
		release_processor();
	}
}

void proc_allocAll(void){
	volatile void * m;
	volatile int i = 1;
	uart0_put_string("ALLOCATE ALL, BITCH!");
	while(i)
	{
		m = s_request_memory_block();
	}
}

void proc_alloc1(void)
{
	volatile void*m = s_request_memory_block();
	volatile int i = 1;
	
	set_priority(LOW);
	release_processor();
	uart0_put_string("Allocation test passed!\r\n");
	while(i) {
		release_processor();
	}
	s_release_memory_block(m);
}
