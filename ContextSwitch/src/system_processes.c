/* system_processes.c 
 * This file defines a set of system processes that the system needs to function.
 * Generally, since system processes run at HIGH priority, they issue a blocking 
 * Receive call and wait to be invoked by a user process
 */
 
 
 #include "process.h"
 #include "uart.h"
 #include "cmd.h"
 
 void sysproc_crt_display(void) 
{
	msg_envelope_t *msg;
	
	while (1) 
	{
		msg = receive_message(NULL);
		uart0_send_string((uint8_t*)msg->data);
		s_release_memory_block(msg);	
	}
}

/* Accepts messages and routes them to their respective registered processes */
void sysproc_command_decoder(void)
{
	msg_envelope_t *msg;
	int message_target;
    int source;
	
	while (1) 
	{
		msg = receive_message(&source);
        
        switch (msg->header.type) {
            case CMD_REGISTER_MSG: {
                cmd_put(msg->data, source);
                s_release_memory_block(msg);
            } break;
            
            case CMD_NOTIFY_MSG: {
                cmd_parse(msg->data);
                message_target = cmd_get(msg->data);
                send_message(message_target, msg);
            } break;
        }
	}
}

int atoi(char *buf) {
	int result = 0;
	while (buf && *buf) {
		if (*buf < '0' || *buf > '9') {
			return -1;
		}
		
		result *= 10;
		result += *buf - '0';
		++buf;
	}
	
	return result;
}

void sysproc_hotkeys(void)
{
	char *newPid, *newPrio;
	int pid, prio;
	
    msg_envelope_t *msg;
	cmd_register("%C");

    while (1) {
        msg = receive_message(NULL);
				if (msg->header.type == CMD_NOTIFY_MSG) {
					newPid = &msg->data[3];
					newPrio = cmd_parse(newPid);
					pid = atoi(newPid);
					prio = atoi(newPrio);
					if (proc_is_valid_pid(pid) && prio >= HIGH && prio <= LOWEST) {
						set_priority(prio, pid);
					}
				} else {
					switch (msg->header.ctrl) {
							case 'v': {
									proc_print(msg, RDY);
							} break;

							case 'x': {
									proc_print(msg, BLOCKED);
							} break;

							case 'c': {
									proc_print(msg, MSG_BLOCKED);
							} break;
					}
				}
        s_release_memory_block(msg);
    }
}
