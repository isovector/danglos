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

enum { WAKEUP = 1, COUNT_REPORT };


extern void debugPrint(unsigned char* c);

static msg_envelope_t * print_msg = NULL;

void msg_print(const char* s) {
	  if(print_msg == NULL)
			print_msg = alloc_message(true);
		strcpy(print_msg->data, s);
		send_message(CRT_DISPLAY_PID, print_msg);	/* Send the output to the CRT_DISPLAY */
}

void processA(void) {
    msg_envelope_t *p;
    int num;
    
    cmd_register("%Z");
    
    p = alloc_message(false);
    
    while (1) {
        p = receive_message(NULL);
        if (strcmp(p->data, "%Z") == 0) {
            free_message(p);
            break;
        } else {
            free_message(p);
        }
    }
    
    num = 0;
    while(1) {
        p = alloc_message(false);
        p->header.type = USER_MSG;
        p->header.ctrl = COUNT_REPORT;
        p->data[0] = num;
        send_message(PROCB_PID, p);
        num++;
        release_processor();
    } 
}	

void processB(void) {
	msg_envelope_t *p;
	while(1) {
		p = receive_message(NULL);
		send_message(PROCC_PID, p);
	}		
}

void processC(void) {
	msg_envelope_t *head = NULL;
	msg_envelope_t *tail = NULL;
	msg_envelope_t *p = NULL;
	msg_envelope_t *q = NULL;
    
	while (1) {
		if (!head) {
			p = receive_message(NULL);
		} else {
			p = head;
			if (!head->header.next) {
				tail = NULL;
      }
			head = head->header.next;
		}
        
		if (p->header.ctrl == COUNT_REPORT) {
			if (p->data[0] % 20 == 0) {
				msg_print("Process C");
				q = alloc_message(false);
                
                q->header.type = USER_MSG;
                q->header.ctrl = WAKEUP;
				delayed_send(proc_get_pid(), q, 10000);
				while(1) {
					p = receive_message(NULL);
					if (p->header.ctrl == WAKEUP)
						break;
					else {
						if (!head) {
							head = tail = p;
						} else {
							tail->header.next = p;
							tail = p;
						}
					}		
				}					
			}
		}
	  
        free_message(p);
		release_processor();
	}
}

void print_val(int val)
{
	char digit[] = {'0', 0};
	digit[0] += val % 10;
	if(val > 0)
		print_val(val / 10);
	msg_print(digit);
}

void uproc_time_everything(void)
{
	msg_envelope_t * msg;
	uint32_t trials = 0;
	uint32_t memory_time = 0, send_time = 0, receive_time = 0;
	
	
	cmd_register("%S");
	while(1)
	{
		msg = receive_message(NULL);
		if(strcmp(msg->data, "%S") == 0)
		{
			if(trials != 0)
			{
				trials = 0;
				free_message(msg);
				msg_print("\r\nREQUEST_MEMORY: ");
				print_val(memory_time / trials);
				msg_print("\r\nSEND_MESSAGE: ");
				print_val(send_time / trials);
				msg_print("\r\nRECEIVE_MESSAGE: ");
				print_val(receive_time / trials);
				memory_time = send_time = receive_time = 0;
			}
		}
		else
		{
			stop_timer();
			receive_time += get_elapsed_time();
		}
		free_message(msg);
		++trials;
		start_timer();
		msg = alloc_message(false);
		stop_timer();
		memory_time += get_elapsed_time();
		
		start_timer();
		send_message(proc_get_pid(), msg);
		stop_timer();
		send_time += get_elapsed_time();
		
		start_timer();
	}
	
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
    clock_h = (data[0] - '0') * 10 + (data[1] - '0');
    clock_m = (data[3] - '0') * 10 + (data[4] - '0');
    clock_s = (data[6] - '0') * 10 + (data[7] - '0');
}

char time_str[] = 
    {   0x1B, '[', 's', 
        0x1B, '[', '0', ';', '7', '0', 'H',
        '0', '0', ':', '0', '0', ':', '0', '0',
        0x1B, '[', 'u', 0
    };

void ucmd_format_time()
{
    time_str[10]  = '0' + clock_h / 10;
    time_str[11] = '0' + clock_h % 10;
    time_str[13] = '0' + clock_m / 10;
    time_str[14] = '0' + clock_m % 10;
    time_str[16] = '0' + clock_s / 10;
    time_str[17] = '0' + clock_s % 10;
}

void uproc_clock(void)
{
    int enabled = 1;
    msg_envelope_t *msg;
    msg_envelope_t *result;
    
    cmd_register("%WR");
    cmd_register("%WT");
		cmd_register("%WS");
	
    msg = alloc_message(true);
    
	
    for (;;) {
        msg->header.type = USER_MSG;
        delayed_send(proc_get_pid(), msg, 1000);
        ucmd_format_time();
        msg_print(time_str);

        while (1) {
            result = receive_message(NULL);
            if (result->header.type == CMD_NOTIFY_MSG) {
                result->data[3] = '\0';
                
                if (strcmp(result->data, "%WR") == 0) {
                    clock_s = 0;
                    clock_m = 0;
                    clock_h = 0;
                    ucmd_format_time();
                    msg_print(time_str);
                    if (enabled == 0) {
											enabled = 1;
											break;
										}
                    
                } else if (strcmp(result->data, "%WT") == 0) {
                    msg_print("\r          \r");
                    enabled = 0;
                } else if(strcmp(result->data, "%WS") == 0) {
                    ucmd_set_time(&result->data[4]);
                    ucmd_format_time();
                    msg_print(time_str);
                    
                    if (enabled == 0) {
											enabled = 1;
											break;
										}
                }
            } else if (result->header.type == USER_MSG && enabled) {
                break;
            }
            free_message(result);
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

void uproc_pong1(void) {
	msg_envelope_t *msg;
	msg = alloc_message(false);
	msg->header.type = USER_MSG;
	msg->header.ctrl = 0;
	delayed_send(UPROC_PONG2_PID, msg, 10);
	
	while (true) {
		msg = receive_message(NULL);
		if (msg->header.ctrl < 10) {
			++msg->header.ctrl;
			delayed_send(UPROC_PONG2_PID, msg, 10);
		} else {
			break;
		}
	}
	
	strcpy(msg->data, "passed pong test successfully\r\n");
	send_message(CRT_DISPLAY_PID, msg);
	
	while (true) {
		release_processor();
	}
}


void uproc_pong2(void) {
	msg_envelope_t *msg;

	while (true) {
		msg = receive_message(NULL);
		++msg->header.ctrl;
		delayed_send(UPROC_PONG1_PID, msg, 10);
	}
}

