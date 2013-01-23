#include <LPC17xx.h>
#include "uart_polling.h"
#include "process.h"


 
#ifdef DEBUG_0
#include <stdio.h>
#endif  /* DEBUG_0 */



#include "mmu.h"



void process_init(pcb_t * pcb, voidfunc func) 
{
	static int x = 0;
  volatile int i;
	uint32_t * sp;

	/* initialize the first process	exception stack frame */
	pcb->m_pid = x++;
	pcb->m_state = NEW;

	sp  = (void*)((char *)s_request_memory_block() + BLOCK_SIZE);
    
	/* 8 bytes alignement adjustment to exception stack frame */
	if (!(((uint32_t)sp) & 0x04)) {
	    --sp; 
	}
	
	*(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */ 
	*(--sp)  = (uint32_t) func;  /* PC contains t/he entry point of the process */

	for (i = 0; i < 6; i++) { /* R0-R3, R12 are cleared with 0 */
		*(--sp) = 0x0;
	}
	
	pcb->mp_sp = sp;


}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: pid of the next to run process
 *         -1 if error happens
 *POST: if gp_current_process was NULL, then it gets set to &pcb1.
 *      No other effect on other global variables.
 */
int scheduler(void)
{
  volatile int pid;

	if (gp_current_process == NULL) {
	  gp_current_process = &(rg_all_processes[0]);
	  return 1;
	}

	for (int i = 0; i < NUM_PRIORITIES; i++) {
		pcb_t* next_in_queue = next(&all_queues[i]);
		if (next_in_queue != NULL) {
			gp_current_process = next_in_queue;
			return next_in_queue->pid;
		}
	}
	return -1;
}
/**
 * @brief release_processor(). 
 * @return -1 on error and zero on success
 * POST: gp_current_process gets updated
 */
int k_release_processor(void)
{
	 volatile int pid;
	 volatile proc_state_t state;
	 pcb_t *p_pcb_old = NULL;

	 pid = scheduler();
	 if (gp_current_process == NULL) {
	   return -1;  
	 }

	 p_pcb_old = gp_current_process;


	 if (pid == 1) {
	   gp_current_process = &pcb1;
	 } else if (pid ==2){
	   gp_current_process = &pcb2;
	 } else {
	   return -1;
	 }					 

	 state = gp_current_process->m_state;

     if (state == NEW) {
	     if (p_pcb_old->m_state != NEW) {
		     p_pcb_old->m_state = RDY;
			 p_pcb_old->mp_sp = (uint32_t *) __get_MSP();
		 }
		 gp_current_process->m_state = RUN;
		 __set_MSP((uint32_t) gp_current_process->mp_sp);
		 __rte();  /* pop exception stack frame from the stack for a new process */
	 } else if (state == RDY){     
		 p_pcb_old->m_state = RDY; 
		 p_pcb_old->mp_sp = (uint32_t *) __get_MSP(); /* save the old process's sp */
		 
		 gp_current_process->m_state = RUN;
		 __set_MSP((uint32_t) gp_current_process->mp_sp); /* switch to the new proc's stack */		
	 } else {
	     gp_current_process = p_pcb_old; /* revert back to the old proc on error */
	     return -1;
	 }	 	 
	 return 0;
}

void initProcesses(void)
{
	volatile int x = 0;
	process_init(rg_all_processes, null_proc);
	for(x = 1; x < NUM_PROCESSES; ++x)
	{
		process_init(rg_all_processes + x, procMemory);
	}
}

void enqueue(queue_t * queue, pcb_t * ppcb)
{
	if((queue->end - queue->start) % NUM_PROCESSES == NUM_PROCESSES - 1)
	{
		//queue is full?
		return;
	}
	
	queue->end = (queue->end + 1) % NUM_PROCESSES;
	queue->procs[queue->end] = ppcb;
}

pcb_t * next(queue_t * queue)
{
	if (queue->end == queue->start) return NULL;
	int oldIndex = queue->curIndex;
	queue->curIndex = (queue->curIndex + 1) % (queue->end - queue->start) + queue->start;
	return queue->procs[oldIndex];
}
