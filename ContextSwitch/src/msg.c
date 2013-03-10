#include <LPC17xx.H>
#include "process.h"
#include "msg.h"
#include "queue.h"
#include "error.h"
#include "rtx.h"

extern volatile uint32_t g_clock;
static msg_envelope_t *delay_msg_list = NULL;
volatile int32_t g_min_msg = -1;

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
    msg->delay = delay + g_clock;
		if(g_min_msg == -1 || msg->delay < g_min_msg)
			g_min_msg = msg->delay;
		
    if (!delay_msg_list) {
        delay_msg_list = msg;

    } else {
        msg_envelope_t *prev = delay_msg_list;

        while (prev->header.next && prev->header.next->delay < delay) {
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

    msg_enqueue_msg(msg, recipient);

    if (recipient->state == MSG_BLOCKED) {
        proc_set_msg_blocked(recipient->pid, 0);

        if (recipient->priority < current_process->priority) {
					if (!kernelMode)
					{
						release_processor();
					}
        }
    }

    return 0;
}


int send_message(int pid, void *pmsg)
{
    msg_envelope_t *msg = (msg_envelope_t *)pmsg;
    msg->header.dest = pid;
    msg->header.src = proc_get_pid();
    msg_send_message(msg, 0);

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


void msg_tick(uint32_t delay)
{
    int ret = 0;

    while (!ret && delay_msg_list && delay_msg_list->delay <= delay) {
        ret = msg_send_message(delay_msg_list, 1);
        delay_msg_list = delay_msg_list->header.next;
				g_min_msg = delay_msg_list ? (int32_t)delay_msg_list->delay : -1;
    }
}

int delayed_send(int pid, void *pmsg, uint32_t delay)
{
    msg_envelope_t *msg = (msg_envelope_t *)pmsg;

    msg->header.dest = pid;
    msg->header.src = proc_get_pid();
    
    wait_enqueue_msg(msg, delay);

    return 0;
}
