#ifndef _PROCESS_H_
#define _PROCESS_H_

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 2048B = 0x200*4 */
#else
#define USR_SZ_STACK 0x080         /* user proc stack size 512B  = 0x80*4  */
#endif /* DEBUG_0 */

#define NUM_PROCESSES 7

#ifndef NULL
#define NULL 0
#endif

#define INITIAL_xPSR 0x01000000    /* user process initial xPSR value */

#include <stdint.h>
#include "p_queue/p_queue.h"

#include "msg.h"

/* process states, note we only assume three states in this example */
typedef enum {NEW = 0, ZOMBIE, RDY, RUN, BLOCKED, MSG_BLOCKED} proc_state_t;  

/*
  PCB data structure definition.
  You may want to add your own member variables
  in order to finish P1 and the entire project 
*/
typedef struct pcb { 
    //struct pcb *mp_next;     /* next pcb, not used in this example, RTX project most likely will need it, keep here for reference */  
    uint32_t *mp_sp;         /* stack pointer of the process */
    uint32_t m_pid;          /* process id */
    proc_state_t m_state;    /* state of the process */     
    priority p;
    msg_envelope_t *msg_head, *msg_tail;
} pcb_t;

typedef void (*voidfunc)(void);

/*
An array of pointers to all of the processses
*/
extern pcb_t  *gp_current_process; /* always point to the current process */

extern pcb_t rg_all_processes[NUM_PROCESSES]; /* Array of all processes */

extern p_queue priority_queue;	/* Priority Queue for scheduling processes */
extern p_queue blocked_queue;

extern int process_valid_pid(int pid);

extern int process_get_pid(void);
extern void process_init(pcb_t *, voidfunc, priority p);  /* Initialize the a given process */  
extern void initProcesses(void);		/* initialize all procs in the system */
int scheduler(void);               /* pick the pid of the next to run process */

int k_release_processor(void);       /* kernel release_process function */
int k_set_msg_blocked(int pid, int block);
int k_set_priority(int, int);
int k_set_my_priority(int);
int k_get_priority(int);

extern void proc_print(void);           /*user process 1 */
extern void proc_clock(void);
extern void null_proc(void);
extern void proc_alloc1(void);
extern void proc_priority_one(void);
extern void proc_priority_two(void);
extern void proc_allocAll(void);
extern void __rte(void);           /* pop exception stack frame */

#endif /* ! _PROCESS_H_ */
