/**
 *@file:  usr_proc.c
 *@brief: Two user processes proc1 and proc2
 *@author: Irene Huang
 *@date: 2013/01/12
 */

#include "rtx.h"
#include "uart_polling.h"
#include "mmu.h"

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

void proc1(void)
{
  volatile int i =0;
  volatile int ret_val = 10;
  while ( 1) {
    if (i!=0 &&i%5 == 0 ) {
      ret_val = release_processor();
#ifdef DEBUG_0
      printf("\n\rproc1: ret_val=%d. ", ret_val);
#else
      uart0_put_string("\n\r");
#endif /* DEBUG_0 */
    }
    uart0_put_char('A' + i%26);
    i++;
  }

}

void proc2(void)
{
  volatile int i =0;
  volatile int ret_val = 20;
  while ( 1) {
    if (i!=0 &&i%5 == 0 ) {
      ret_val = release_processor();
#ifdef DEBUG_0
      printf("\n\rproc2: ret_val=%d. ", ret_val);
#else
      uart0_put_string("\n\r");
#endif  /* DEBUG_0 */
    }
    uart0_put_char('b');
    i++;
  }
}

void procMemory(void)
{
    static int depth = 0;
    void *ptr = NULL;
    do
    {
        if (mmu_can_alloc_mem()) {
            ptr = s_request_memory_block();
            ++depth;
            procMemory();
            --depth;
            s_release_memory_block(ptr);
            release_processor();
        } else {
            uart0_put_string("Allocated all memory\n\r");
            uart0_put_string("Releasing it now\n\r");
        }
    } while (depth == 0);
}
