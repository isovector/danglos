/**
 *@file:  usr_proc.c
 *@brief: Two user processes proc1 and proc2
 *@author: Irene Huang
 *@date: 2013/01/12
 */

#include "rtx.h"
#include "uart.h"
#include "mmu.h"
#include "p_queue/p_queue.h"
#include "cmd.h"
#include "msg.h"
#include "process.h"

#ifdef DEBUG_0
#include <stdio.h>
#include <stdlib.h>
#endif  /* DEBUG_0 */

int TEST_MEM_BLOCK = 0;
int TEST_PRIORITY_SWAP = 0;

volatile int i = 0;
volatile int j = 0;

int num_successful_tests = 0;

extern void debugPrint(unsigned char* c);

void msg_print(const char* s) {
	  msg_envelope_t *output;
		output = (msg_envelope_t*)s_request_memory_block();
		strcpy(output->data, s);
		send_message(CRT_DISPLAY_PID, output);	/* Send the output to the CRT_DISPLAY */
}

void cmd_register(const char* tag)
{
	msg_envelope_t *msg;
	msg = (msg_envelope_t *)s_request_memory_block();
	msg->data[0] = REGISTER;
	msg->data[1] = proc_get_pid();
	strcpy(&(msg->data[2]), tag);
	send_message(CMD_DECODER_PID, msg);
}

void uproc_null(void)
{
    while (1) {
        release_processor();
    }
}

void uproc_print(void)
{
    while (1) {
        uart_send_string(0, "\t(1) ping\n\r");
        release_processor();
    }
}

static volatile size_t clock_h = 0, clock_m = 0, clock_s = 0;
void ucmd_set_time(const char *data)
{
    //TODO(sandy): jesus this is ugly
    clock_h = (data[0] - '0') * 10 + (data[1] - '0');
    clock_m = (data[3] - '0') * 10 + (data[4] - '0');
    clock_s = (data[6] - '0') * 10 + (data[7] - '0');
}

uint8_t time_str[10];

void ucmd_format_time()
{
    time_str[0] = '0' + clock_h / 10;
    time_str[1] = '0' + clock_h % 10;
    time_str[3] = '0' + clock_m / 10;
    time_str[4] = '0' + clock_m % 10;
    time_str[6] = '0' + clock_s / 10;
    time_str[7] = '0' + clock_s % 10;
}

void uproc_clock(void)
{
    msg_envelope_t *msg;
	  msg_envelope_t *result;
    cmd_register("%WR");
		cmd_register("%WT");
	
    time_str[2] = time_str[5] = ':';
    time_str[8] = '\r';
	  time_str[9] = 0;

    msg = (msg_envelope_t *)s_request_memory_block();
    
	
    for (;;) {
				msg->data[1] = 3;
        delayed_send(proc_get_pid(), msg, 100);
        ucmd_format_time();
				msg_print(time_str);
				
				while (1) {
					result = receive_message(NULL);
					result->data[4] = '\0';
					if (strcmp(&result->data[1], "%WR") == 0) {
						clock_s = 0;
						clock_m = 0;
						clock_h = 0;
						ucmd_format_time();
						msg_print(time_str);
					} else if (strcmp(&result->data[1], "%WT") == 0) {
						s_release_memory_block(result);
						msg_print("\r          \r");
						while (1) {
							set_my_priority(LOW);
							release_processor();
						}
					} else if(strcmp(&result->data[1], "%WS") == 0){
						ucmd_set_time(&result->data[5]);
						ucmd_format_time();
						msg_print(time_str);
					}	else if (result->data[1] == 3) {
						break;
					}
					s_release_memory_block(result);
				}

        if (++clock_s >= 60) {
            clock_s = 0;

            if (++clock_m >= 60) {
                clock_m = 0;

                if (++clock_h >= 24) {
                    clock_h = 0;
                }
            }
        }
    }
}
