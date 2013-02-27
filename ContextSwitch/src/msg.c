#include "msg.h"
#include "process.h"

void msg_enqueue_msg(msg_envelope_t *msg) {
}

msg_envelope_t *msg_dequeue_msg(int pid) {
    return NULL;
}

int msg_send_message(void *pmsg, int blocks) {
    msg_envelope_t *msg = (msg_envelope_t*)pmsg;
    pcb_t *recipient = &rg_all_processes[msg->header.dest];
    msg->header.src = process_get_pid();

    msg_enqueue_msg(msg);
    
    if (recipient->m_state == MSG_BLOCKED)
    {
        recipient->m_state = RDY;
        
        if (blocks == 1 && recipient->p < gp_current_process->p)
        {
            release_processor();
        }
    }
    
    return 0;
}


int send_message(int pid, void *pmsg) {
    msg_envelope_t *msg = (msg_envelope_t*)pmsg;
    msg->header.dest = pid;
    msg_send_message(msg, 0);
    
    return 0;
}

void *receive_message(int *sender) {
    int pid = process_get_pid();
    msg_envelope_t *msg = msg_dequeue_msg(pid);
    
    if (msg)
    {
        if (sender) 
        {
            *sender = msg->header.src;
        }
        return msg;
    }
    
    gp_current_process->m_state = MSG_BLOCKED;
    release_processor();
    
    msg = msg_dequeue_msg(pid);
    if (sender) 
    {
        *sender = msg->header.src;
    }
    return msg;
}

int delayed_send(int pid, void *pmsg, int delay) {
    msg_envelope_t *msg = (msg_envelope_t*)pmsg;
    
    msg->header.dest = pid;
    msg_send_message(msg, 1);
    // TODO(sandy): make this actually wait :D
    
    return 0;
}
