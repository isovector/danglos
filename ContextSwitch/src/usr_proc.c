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
        p = (msg_envelope_t *)s_request_memory_block();
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
				q = (msg_envelope_t *)s_request_memory_block();
                
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
    int enabled;
    msg_envelope_t *msg;
    msg_envelope_t *result;
    
    cmd_register("%WR");
    cmd_register("%WT");
	
    msg = alloc_message(false);
    
	
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
                    enabled = 1;
                    clock_s = 0;
                    clock_m = 0;
                    clock_h = 0;
                    ucmd_format_time();
                    msg_print(time_str);
                    
                    break;
                } else if (strcmp(result->data, "%WT") == 0) {
                    free_message(result);
                    msg_print("\r          \r");
                    enabled = 0;
                } else if(strcmp(result->data, "%WS") == 0) {
                    enabled = 1;
                    ucmd_set_time(&result->data[4]);
                    ucmd_format_time();
                    msg_print(time_str);
                    
                    break;
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
