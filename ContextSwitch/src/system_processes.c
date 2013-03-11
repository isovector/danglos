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
	
	while (1) 
	{
		msg = receive_message(NULL);
		
		if (msg->data[0] == REGISTER) {
			cmd_put(&(msg->data[2]), msg->data[1]);
			s_release_memory_block(msg);
		} else if (msg->data[0] == NOTIFY) {
			cmd_parse(msg->data + 1);
			message_target = cmd_get(msg->data + 1);
			send_message(message_target, msg);
	  }
	}
}

void sysproc_hotkeys(void)
{
	 msg_envelope_t *msg;
	 
	 while (1) {
			 msg = receive_message(NULL);
			 switch (msg->header.ctrl) {
					 case 'z': {
							 proc_print(msg, RDY);
					 } break;
					 
					 case 'x': {
							 proc_print(msg, BLOCKED);
					 } break;
					 
					 case 'c': {
							 proc_print(msg, MSG_BLOCKED);
					 } break;
			 }
			 s_release_memory_block(msg);
	 }
}
