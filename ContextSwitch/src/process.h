#ifndef _PROCESS_H_
#define _PROCESS_H_

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 2048B = 0x200*4 */
#else
#define USR_SZ_STACK 0x080         /* user proc stack size 512B  = 0x80*4  */
#endif /* DEBUG_0 */

#define NUM_PROCESSES 20
#define NUM_USER_PROCESSES 7

#ifndef NULL
#define NULL 0
#endif

#define INITIAL_xPSR 0x01000000    /* user process initial xPSR value */

#include <stdint.h>
#include "p_queue/p_queue.h"

#include "msg.h"

/* System processes need fixed pids so that they can be referenced consistently */
#define CRT_DISPLAY_PID 10
#define CMD_DECODER_PID 11
#define HOTKEY_PROC     12
#define KBD_IPROC_PID   13
#define UART_IPROC_PID  14
#define TIMER_IPROC_PID 15


/* process states, note we only assume three states in this example */
typedef enum {NEW = 0, ZOMBIE, RDY, RUN, BLOCKED, MSG_BLOCKED} proc_state_t;

/*
  PCB data structure definition.
  You may want to add your own member variables
  in order to finish P1 and the entire project
*/
typedef struct pcb {
    uint32_t *stackptr;         /* stack pointer of the process */
    uint32_t pid;          /* process id */
    proc_state_t state;    /* state of the process */
    priority_t priority;
	  int iproc;
    msg_envelope_t *msg_head, *msg_tail;
} pcb_t;

typedef void (*uproc_func)(void);

/*
An array of pointers to all of the processses
*/
extern pcb_t  *current_process; /* always point to the current process */
extern int old_pid;
extern pcb_t processes[NUM_PROCESSES]; /* Array of all processes */

extern p_queue priority_queue;	/* Priority Queue for scheduling processes */
extern p_queue blocked_queue;

extern int proc_is_valid_pid(int pid);

extern int proc_get_pid(void);
extern void process_init(pcb_t *, uproc_func, priority_t p, int target_pid);  /* Initialize the a given process */
extern void proc_init(void);		/* initialize all procs in the system */
int scheduler(void);               /* pick the pid of the next to run process */

int k_release_processor(void);       /* kernel release_process function */
int proc_set_msg_blocked(int pid, int block);
int k_set_priority(int, int);
int k_set_my_priority(int);
int k_get_priority(int);

void proc_set_iproc(int);
void proc_reset_iproc(void);

extern void sysproc_crt_display(void);
extern void sysproc_command_decoder(void);
extern void sysproc_hotkeys(void);
void proc_print(msg_envelope_t *, proc_state_t);

extern void uproc_print(void);           /*user process 1 */
extern void uproc_clock(void);
extern void uproc_null(void);

extern void processA(void);
extern void processB(void);
extern void processC(void);

extern void __rte(void);           /* pop exception stack frame */

#endif /* ! _PROCESS_H_ */
