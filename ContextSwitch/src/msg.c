#include "msg.h"
#include "process.h"

void msg_enqueue_msg(msg_envelope_t *msg) {
}

msg_envelope_t *msg_dequeue_msg(int pid) {
    return NULL;
}

void msg_send_message(msg_envelope_t *msg, int blocks) {
    pcb_t &recipient = rg_all_processes[msg->dest];
    msg->src = process_get_pid();

    if (recipient.m_state == MSG_BLOCKED)
    {
        recipient.m_state = RDY;
        
        if (blocks == 1 && recipient.p < gp_current_process->p)
        {
            release_processor();
        }
    }
}



void send_message(int pid, msg_envelope_t *msg) {
    msg->dest = pid;
    msg_send_message(msg, 0);
}

msg_envelope_t *receive_message(int *sender) {
    int pid = process_get_pid();
    msg_envelope_t *msg = msg_dequeue_msg(pid);
    
    if (msg)
    {
        if (sender) 
        {
            *sender = msg->src;
        }
        return msg;
    }
    
    gp_current_process->m_state = MSG_BLOCKED;
    release_processor();
    
    msg = msg_dequeue_msg(pid);
    if (sender) 
    {
        *sender = msg->src;
    }
    return msg;
}

void delayed_send(int pid, msg_envelope_t *msg, int delay) {
    msg->dest = pid;
    msg_send_message(msg, 1);
    // TODO(sandy): make this actually wait :D
}
