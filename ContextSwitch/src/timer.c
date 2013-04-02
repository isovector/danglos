#include <LPC17xx.h>
#include "timer.h"
#include "process.h"
#define DEBUG_TIMER

#define TIME_EVERYTHING

#ifdef DEBUG_TIMER
#define CLOCK_FREQUENCY 1249
#else
#define CLOCK_FREQUENCY 12499
#endif


volatile uint32_t g_clock = 0;

static volatile uint32_t t_start = 0;
static volatile uint32_t t_end = 0;

extern void msg_tick(uint32_t);

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



#ifdef TIME_EVERYTHING
void start_timer(void)
{
	t_start = g_clock;
}
void stop_timer(void)
{
	t_end = g_clock;
}
#else

void start_timer(void){}
void stop_timer(void){}
#endif

uint32_t get_elapsed_time(void){ return t_end - t_start;}

void c_TIMER0_IRQHandler(void)
{
	/* ack inttrupt, see section  21.6.1 on pg 493 of LPC17XX_UM */
	LPC_TIM0->IR = 1;  
	
	++g_clock;
	msg_tick(g_clock);
}
