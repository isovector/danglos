#ifndef _PROCESS_H_
#define _PROCESS_H_

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 2048B = 0x200*4 */
#else
#define USR_SZ_STACK 0x080         /* user proc stack size 512B  = 0x80*4  */
#endif /* DEBUG_0 */

#define NUM_PROCESSES 7
#define NUM_PRIORITIES 4

#define NULL 0
#define INITIAL_xPSR 0x01000000    /* user process initial xPSR value */

#include <stdint.h>
#include "p_queue/p_queue.h"

/* process states, note we only assume three states in this example */
typedef enum {NEW = 0, RDY, RUN} proc_state_t;  

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
} pcb_t;

typedef void (*voidfunc)(void);

/*
An array of pointers to all of the processses
*/
pcb_t  *gp_current_process = NULL; /* always point to the current process */

pcb_t rg_all_processes[NUM_PROCESSES]; /* Array of all processes */

p_queue priority_queue;	/* Priority Queue for scheduling processes */
p_queue blocked_queue;

extern void process_init(pcb_t *, voidfunc, priority p);  /* Initialize the a given process */  
extern void initProcesses(void);		/* initialize all procs in the system */
int scheduler(void);               /* pick the pid of the next to run process */

int k_release_process(void);       /* kernel release_process function */

extern void proc1(void);           /* user process 1 */
extern void proc2(void);           /* user process 2 */
extern void null_proc(void);
extern void procMemory(void);
extern void __rte(void);           /* pop exception stack frame */

#endif /* ! _PROCESS_H_ */
