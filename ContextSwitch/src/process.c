#include <LPC17xx.h>
#include "uart_polling.h"
#include "process.h"
#include "p_queue/p_queue.h"
 
#ifdef DEBUG_0
#include <stdio.h>
#endif  /* DEBUG_0 */

#include "mmu.h"

void process_init(pcb_t * pcb, voidfunc func, priority p) 
{
	static int x = 0;
  	volatile int i;
	uint32_t * sp;
	
	if (pcb == NULL) {
		return;
	}

	/* initialize the first process	exception stack frame */
	pcb->m_pid = x++;
	pcb->m_state = NEW;
	pcb->p = p;

	sp  = (void*)((char *)s_request_memory_block() + MMU_BLOCK_SIZE);
    
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
	int next_process;
	int next_blocked;

	/* Check to see if there is a blocked process that we can move to the ready state */
	next_blocked = pq_front(&blocked_queue);
	if ( next_blocked != PQ_NOT_FOUND && mmu_can_alloc_mem() ) 
	{
		pq_enqueue(&priority_queue, next_blocked, rg_all_processes[next_blocked].p);
		rg_all_processes[next_blocked].m_state = RDY;
		pq_dequeue(&blocked_queue);
	}

	next_process = pq_front(&priority_queue);
	pq_dequeue(&priority_queue);
  	return next_process;
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
	
	 /* Move the old proc into a temp and move the new process into the current proccess pointer */
	 p_pcb_old = gp_current_process;
	 gp_current_process = &(rg_all_processes[pid]);

	/* Make sure to add the old process to the back of the pq */
	 if (p_pcb_old->m_state == BLOCKED) {
		 pq_enqueue(&blocked_queue, p_pcb_old->m_pid, p_pcb_old->p);
	 } else {
		pq_enqueue(&priority_queue, p_pcb_old->m_pid, p_pcb_old->p);
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

int k_block_and_release_processor(void) 
{
	gp_current_process->m_state = BLOCKED;
	return k_release_processor();
}

void initProcesses(void)
{
	volatile int i = 0;
	pq_init(&priority_queue);
	pq_init(&blocked_queue);
	
	
	/* Initialize the null process with the lowest priority */
	process_init(&rg_all_processes[i], null_proc, LOWEST);
	pq_enqueue(&priority_queue, i, rg_all_processes[i].p);
	gp_current_process = &rg_all_processes[i++];
	
	process_init(&rg_all_processes[i], proc_priority, MED);
	pq_enqueue(&priority_queue, i, rg_all_processes[i].p);
	gp_current_process = &rg_all_processes[i++];
	
	for(i; i < NUM_PROCESSES; ++i)
	{
		process_init(&rg_all_processes[i], proc_print, MED);
		pq_enqueue(&priority_queue, i, rg_all_processes[i].p);
	}
}

int k_set_priority(int p) {
	pq_move(&priority_queue, gp_current_process->m_pid, gp_current_process->p, p);
	gp_current_process->p = p;
	return 0;
}
