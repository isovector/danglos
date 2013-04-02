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

static volatile uint32_t s_timer = 0;
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
	
	pTimer = (LPC_TIM_TypeDef *)LPC_TIM1;
	pTimer->PR = CLOCK_FREQUENCY;
	pTimer->MR1 = 1;
	pTimer->MCR = 3;
	s_timer = 0;
	NVIC_EnableIRQ(TIMER1_IRQn);
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

__asm void TIMER1_IRQHandler(void)
{
	PRESERVE8
	IMPORT c_TIMER1_IRQHandler
	PUSH{r4-r11, lr}
	BL c_TIMER1_IRQHandler
	POP{r4-r11, pc}
} 

void c_TIMER1_IRQHandler(void)
{
	/* ack inttrupt, see section  21.6.1 on pg 493 of LPC17XX_UM */
	LPC_TIM0->IR = 1;  
	
	++s_timer;
}

#ifdef TIME_EVERYTHING
void start_timer(void)
{
	t_start = s_timer;
}
void stop_timer(void)
{
	t_end = s_timer;
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
