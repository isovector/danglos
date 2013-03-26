#include <LPC17xx.H>
#include "process.h"
#include "msg.h"
#include "queue.h"
#include "error.h"
#include "rtx.h"

extern volatile uint32_t g_clock;
static msg_envelope_t *delay_msg_list = NULL;

void msg_enqueue_msg(msg_envelope_t *msg, pcb_t *pcb)
{
    if (!pcb->msg_head) {
        pcb->msg_head = pcb->msg_tail = msg;
    } else {
        pcb->msg_tail->header.next = msg;
        pcb->msg_tail = msg;
    }
}

void wait_enqueue_msg(msg_envelope_t *msg, int delay)
{
		//Search through for duplicates
		//because pointers are retarded
		msg_envelope_t * cur = delay_msg_list;
    msg->header.tick = delay + g_clock;
		while(cur)
		{
			if(cur == msg)
				return;
			cur = cur->header.next;
		}
    if (!delay_msg_list || delay_msg_list->header.tick > msg->header.tick) {
				msg->header.next = delay_msg_list;
				delay_msg_list = msg;
		}else{
        msg_envelope_t *prev = delay_msg_list;
        while (prev->header.next != NULL && prev->header.next->header.tick <= msg->header.tick) {
            prev = prev->header.next;
        }

        msg->header.next = prev->header.next;
        prev->header.next = msg;
    }

}


msg_envelope_t *msg_dequeue_msg(pcb_t *pcb)
{
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


int msg_send_message(void *pmsg, int kernelMode)
{
    msg_envelope_t *msg = (msg_envelope_t *)pmsg;
    pcb_t *recipient;

    if (!proc_is_valid_pid(msg->header.dest)) {
        return ERR_PROC_BAD_PID;
    }

    recipient = &processes[msg->header.dest];
		msg->header.next = NULL;
    msg_enqueue_msg(msg, recipient);

    if (recipient->state == MSG_BLOCKED) {
        proc_set_msg_blocked(recipient->pid, 0);

        if (recipient->priority < current_process->priority && !kernelMode) {
            release_processor();
        }
    }

    return 0;
}

void msg_init_envelope(void *pmsg, int src, int dest)
{
    msg_envelope_t *msg = (msg_envelope_t *)pmsg;
    
    msg->header.dest = dest;
    msg->header.src = src;
    msg->header.next = NULL;
}

int send_kernel_message(int dest, int src, void *pmsg)
{
    msg_init_envelope(pmsg, src, dest);
    msg_send_message((msg_envelope_t *)pmsg, 1);
    
    return 0;
}

int send_message(int pid, void *pmsg)
{
    msg_init_envelope(pmsg, proc_get_pid(), pid);
    msg_send_message((msg_envelope_t *)pmsg, 0);

    return 0;
}

void *receive_message(int *sender)
{
    msg_envelope_t *msg = msg_dequeue_msg(current_process);

    while (!msg) {
			proc_set_msg_blocked(current_process->pid, 1);
			release_processor();
			msg = msg_dequeue_msg(current_process);
    }
    if (sender) {
        *sender = msg->header.src;
    }

    return msg;
}


void msg_tick(uint32_t tick)
{
    int ret = 0;

    while (!ret && delay_msg_list && delay_msg_list->header.tick <= tick) {
				msg_envelope_t * msg = delay_msg_list;
        delay_msg_list = delay_msg_list->header.next;
        ret = msg_send_message(msg, true);
    }
}

int delayed_send(int pid, void *pmsg, uint32_t delay)
{
    msg_init_envelope(pmsg, proc_get_pid(), pid);
    wait_enqueue_msg((msg_envelope_t *)pmsg, delay);

    return 0;
}

void free_message(void *pmsg)
{
	msg_envelope_t * msg;
	if(!pmsg)
		return;
	msg = (msg_envelope_t *)pmsg;
	if(msg->header.memory_type == SYSTEM_MANAGED)
		s_release_memory_block(pmsg);
}

msg_envelope_t * alloc_message(bool managed)
{
	msg_envelope_t * msg = (msg_envelope_t *)s_request_memory_block();
	msg->header.memory_type = managed ? USER_MANAGED : SYSTEM_MANAGED;
	return msg;
}
