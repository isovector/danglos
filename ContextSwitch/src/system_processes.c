/* system_processes.c 
 * This file defines a set of system processes that the system needs to function.
 * Generally, since system processes run at HIGH priority, they issue a blocking 
 * Receive call and wait to be invoked by a user process
 */
 
 
 #include "process.h"
 #include "uart.h"
 
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

 
 void sysproc_hotkeys(void)
 {
     msg_envelope_t *msg;
     
     while (1) {
         msg = receive_message(NULL);
         switch (msg->header.ctrl) {
             case 'z': {
                 // print priorities
             } break;
             
             case 'x': {
                 // print blocked
             } break;
             
             case 'c': {
                 // print msg blocked
             } break;
         }
         s_release_memory_block(msg);
     }
 }