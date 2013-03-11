#include <LPC17xx.h>
#include "uart_polling.h"
#include "process.h"
#include "p_queue/p_queue.h"
#include "error.h"
#define MEMORY_BLOCKS_TEST

#ifdef DEBUG_0
#include <stdio.h>
#endif  /* DEBUG_0 */

#include "mmu.h"
#include "debug_print.h"

pcb_t  *current_process = NULL;
pcb_t processes[NUM_PROCESSES] = {0};
p_queue priority_queue;
p_queue blocked_queue;

int proc_is_valid_pid(int pid)
{
    return pid >= 0 && pid < NUM_PROCESSES;
}

int proc_get_pid(void)
{
    return current_process->pid;
}

uproc_func proc_funcs[NUM_PROCESSES];
void proc_wrapper(void)
{
    proc_funcs[proc_get_pid()]();
    current_process->state = ZOMBIE;
}

void process_init(pcb_t *pcb, uproc_func func, priority_t p, int target_pid)
{
    int i;
    uint32_t *sp;

    if (pcb == NULL) {
        return;
    }

    /* initialize the first process	exception stack frame */
    proc_funcs[target_pid] = func;

    pcb->pid = target_pid;
    pcb->state = NEW;
    pcb->priority = p;

    pcb->msg_head = pcb->msg_tail = NULL;

    sp  = (void *)((char *)s_request_memory_block() + MMU_BLOCK_SIZE);

    /* 8 bytes alignement adjustment to exception stack frame */
    if (!(((uint32_t)sp) & 0x04)) {
        --sp;
    }

    *(--sp)  = INITIAL_xPSR;      /* user process initial xPSR */
    *(--sp)  = (uint32_t) proc_wrapper;  /* PC contains t/he entry point of the process */

    for (i = 0; i < 6; i++) { /* R0-R3, R12 are cleared with 0 */
        *(--sp) = 0x0;
    }

    pcb->stackptr = sp;

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

    // TODO(sandy): we need logic if we are MSG_BLOCKED
    if (next_blocked != PQ_NOT_FOUND && mmu_can_alloc_mem()) {
        pq_enqueue(&priority_queue, next_blocked, processes[next_blocked].priority);
        processes[next_blocked].state = RDY;
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
    int pid;
    proc_state_t state;
    pcb_t *p_pcb_old = NULL;

    pid = scheduler();

    /* Move the old proc into a temp and move the new process into the current proccess pointer */
    p_pcb_old = current_process;
    current_process = &(processes[pid]);
    
    if (p_pcb_old == NULL) {
        p_pcb_old = current_process;
    }
		else if (p_pcb_old->state != MSG_BLOCKED) {
        if (p_pcb_old->state == BLOCKED) {
            pq_enqueue(&blocked_queue, p_pcb_old->pid, p_pcb_old->priority);
        } else {
            pq_enqueue(&priority_queue, p_pcb_old->pid, p_pcb_old->priority);
        }
    }

    state = current_process->state;

    if (p_pcb_old != current_process) {
        if (p_pcb_old->state == RUN) {
            p_pcb_old->state = RDY;
        }
        p_pcb_old->stackptr = (uint32_t *) __get_MSP();
    }
    
    current_process->state = RUN;
    if (state == NEW) {
			__set_MSP((uint32_t) current_process->stackptr);
        __rte();
    } else {
			__set_MSP((uint32_t) current_process->stackptr);
		}
    
    return 0;
}

int k_block_and_release_processor(void)
{
    current_process->state = BLOCKED;
    return k_release_processor();
}


void proc_init(void)
{
    int j = 0;

    process_init(&processes[j], uproc_null, LOWEST, j);
    pq_enqueue(&priority_queue, j, processes[j].priority);
    j++;
	
    process_init(&processes[j], uproc_clock, MED, j);
    pq_enqueue(&priority_queue, j, processes[j].priority);
    j++;


    for (; j < NUM_USER_PROCESSES; ++j) {
        process_init(&processes[j], uproc_null, LOWEST, j);
        pq_enqueue(&priority_queue, j, processes[j].priority);
    }
}

void system_proc_init(void) {
	pq_init(&priority_queue);
  pq_init(&blocked_queue);
	
  process_init(&processes[CRT_DISPLAY_PID], sysproc_crt_display, HIGH, CRT_DISPLAY_PID);
  pq_enqueue(&priority_queue, CRT_DISPLAY_PID, processes[CRT_DISPLAY_PID].priority);
	
	process_init(&processes[CMD_DECODER_PID], sysproc_command_decoder, HIGH, CMD_DECODER_PID);
  pq_enqueue(&priority_queue, CMD_DECODER_PID, processes[CMD_DECODER_PID].priority);
	
  process_init(&processes[HOTKEY_PROC], sysproc_hotkeys, HIGH, HOTKEY_PROC);
  pq_enqueue(&priority_queue, HOTKEY_PROC, processes[HOTKEY_PROC].priority);
}

int proc_set_msg_blocked(int target, int block)
{
    pcb_t *proc;

    if (!proc_is_valid_pid(target)) {
        return ERR_PROC_BAD_PID;
    }

    proc = &processes[target];

    if (block) {
        proc->state = MSG_BLOCKED;
    } else {
        proc->state = RDY;
        pq_enqueue(&priority_queue, target, proc->priority);
    }

    return 0;
}

int k_set_priority(int p, int target)
{
    priority_t prio = processes[target].priority;
    int ret = 0;

    ret = pq_move(&priority_queue, target, prio, (priority_t)p);

    if (ret) {
        ret = pq_move(&blocked_queue, target, prio, (priority_t)p);
    }

    processes[target].priority = (priority_t)p;

    if ((prio < p || (p < current_process->priority && target != current_process->pid)) && !ret) {
        return k_release_processor();
    }

    return ret;
}

int k_set_my_priority(int p)
{
    k_set_priority(p, current_process->pid);
    return 0;
}

int k_get_priority(int target)
{
    if (target < 0 || target >= NUM_PROCESSES) {
        return -1;
    }

    return processes[target].priority;
}


void proc_print(msg_envelope_t * msg, proc_state_t state)
{
	char header_txt [] = "\r\nPID\tPRIO\r\n";
	int header_len = sizeof(header_txt) - 1;
	int i;
	int lines = 0;
	char line_txt [] = "00\t0\r\n";
	int line_len = sizeof(line_txt) - 1;
	strcpy(msg->data, header_txt);
	for(i = 0; i < NUM_PROCESSES; ++i)
	{
		if(processes[i].state == state)
		{
			line_txt[0] = '0' + processes[i].pid / 10;
			line_txt[1] = '0' + processes[i].pid % 10;
			line_txt[3] = '0' + processes[i].priority;
			strcpy(msg->data + header_len + lines * line_len, line_txt);
			++lines;
		}
	}
	send_kernel_message(CRT_DISPLAY_PID, proc_get_pid(), msg);
}
