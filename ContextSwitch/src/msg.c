#include "process.h"
#include "msg.h"
#include "queue.h"
#include "error.h"

extern volatile uint32_t g_clock;
static msg_envelope_t * delay_msg_list = NULL;

void msg_enqueue_msg(msg_envelope_t *msg, pcb_t *pcb) {
    if (!pcb->msg_head) {
        pcb->msg_head = pcb->msg_tail = msg;
    } else {
        pcb->msg_tail->header.next = msg;
        pcb->msg_tail = msg;
    }
}

void wait_enqueue_msg(msg_envelope_t * msg, pcb_t *pcb, int delay){
	msg->delay = delay + g_clock;
	if(!delay_msg_list){
		delay_msg_list = msg;
		
	} else {
		msg_envelope_t *prev = delay_msg_list;
		while(prev->header.next && prev->header.next->delay < delay)
		{
			prev = prev->header.next;
		}
		msg->header.next = prev->header.next;
		prev->header.next = msg;
	}
	
}


msg_envelope_t *msg_dequeue_msg(pcb_t *pcb) {
    msg_envelope_t *msg;
    
    if (!pcb->msg_head) {
        return NULL;
    }
    
    msg = pcb->msg_head;
    pcb->msg_head = msg->header.next;
    if (!msg->header.next) {
        pcb->msg_tail = NULL;
    }
    
    return msg;
}

int msg_send_message(void *pmsg, int blocks) {
    msg_envelope_t *msg = (msg_envelope_t*)pmsg;
    pcb_t *recipient;

    if (!process_valid_pid(msg->header.dest)) {
        return ERR_PROC_BAD_PID;
    }
    
    recipient = &rg_all_processes[msg->header.dest];
    msg->header.src = process_get_pid();

    msg_enqueue_msg(msg, recipient);
    
    if (recipient->m_state == MSG_BLOCKED)
    {
        k_set_msg_blocked(recipient->m_pid, 0);
        if (blocks == 1 && recipient->p < gp_current_process->p)
        {
            k_release_processor();
        }
    }
    
    return 0;
}


int k_send_message(int pid, void *pmsg) {
    msg_envelope_t *msg = (msg_envelope_t*)pmsg;
    msg->header.dest = pid;
    msg_send_message(msg, 0);
    
    return 0;
}

void *receive_message(int *sender) {
    msg_envelope_t *msg = msg_dequeue_msg(gp_current_process);
    
    if (msg)
    {
        if (sender) 
        {
            *sender = msg->header.src;
        }
        return msg;
    }
    
    k_set_msg_blocked(gp_current_process->m_pid, 1);
    k_release_processor();
    
    msg = msg_dequeue_msg(gp_current_process);
    if (sender) 
    {
        *sender = msg->header.src;
    }
    return msg;
}


void msg_tick(uint32_t delay)
{
	int ret = 0;
	while(!ret && delay_msg_list && delay_msg_list->delay <= delay)
	{
		ret = msg_send_message(delay_msg_list, 1);
		delay_msg_list = delay_msg_list->header.next; 
	}
}

int delayed_send(int pid, void *pmsg, uint32_t delay) {
    msg_envelope_t *msg = (msg_envelope_t*)pmsg;
    
    msg->header.dest = pid;
    msg_send_message(msg, 1);
    // TODO(sandy): make this actually wait :D
    
    return 0;
}
