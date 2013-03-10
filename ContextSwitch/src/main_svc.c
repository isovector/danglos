/**
 * @file:   main_sv.c
 * @brief:  To demonstrate svc as a gateway to os functions
 * @author: Yiqing Huang
 * @date:   2013/01/12
 * NOTE: standard C library is not allowed in the final kernel code
 *       it is OK if you only use the C library for debugging purpose 
 *       during the development/prototyping phase
 */

#include <LPC17xx.h>
#ifdef DEBUG_0
#include <stdio.h>
#else
#define NULL 0
#endif  /* DEBUG_0 */
#include "uart.h"
#include "rtx.h"
#include "timer.h"

extern void initMemory(void);
extern void proc_init(void);
extern void system_proc_init(void);

int main() 
{
  volatile unsigned int ret_val = 1234;
  SystemInit();  /* initialize the system */
  __disable_irq();
  uart0_init();
	initMemory();
	system_proc_init();
  proc_init();
	timer_init();
  __enable_irq();
  
  /* transit to unprivileged level, default MSP is used */
  __set_CONTROL(__get_CONTROL() | BIT(0));  

  ret_val = release_processor();

  /* should never reach here!!! */
  return -1;  
}
