#ifndef _TIMER_H_
#define _TIMER_H_
#define TIME_EVERYTHING

extern void timer_init(void);

extern void start_timer(void);
extern void stop_timer(void);
extern uint32_t get_elapsed_time(void);

#define TIMER_SLICE 1
#endif //_TIMER_H_
