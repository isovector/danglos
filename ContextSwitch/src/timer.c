#include <LPC17xx.h>
#include "timer.h"
#include "process.h"
//#define DEBUG_TIMER

#ifdef DEBUG_TIMER
#define CLOCK_FREQUENCY 1249
#else
#define CLOCK_FREQUENCY 12499
#endif


volatile uint32_t g_clock = 0;
extern void msg_tick(uint32_t);
extern volatile int32_t g_min_msg;

void timer_init()
{
	LPC_TIM_TypeDef  * pTimer = (LPC_TIM_TypeDef *)LPC_TIM0;
	pTimer->PR = CLOCK_FREQUENCY;
	pTimer->MR0 = 1;
	pTimer->MCR = 3;
	g_clock = 0;
	NVIC_EnableIRQ(TIMER0_IRQn);
	pTimer->TCR = 1;
}
__asm void TIMER0_IRQHandler(void)
{
	PRESERVE8
	IMPORT c_TIMER0_IRQHandler
	PUSH{r4-r11, lr}
	BL c_TIMER0_IRQHandler
	POP{r4-r11, pc}
} 

void c_TIMER0_IRQHandler(void)
{
	/* ack inttrupt, see section  21.6.1 on pg 493 of LPC17XX_UM */
	LPC_TIM0->IR = 1;  
	
	++g_clock;
	if(g_min_msg != -1 && g_clock >= g_min_msg)
		msg_tick(g_clock);
}
