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

extern void debugPrint(unsigned char *);


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

/* Starts high */
void uproc_priority1(void)
{
		char cnum[2] = {0};
    while (i < 10) {
        i++;
        release_processor();

        if (j > 0) {
            set_my_priority(3);
        }
    }

    set_my_priority(3);
    set_priority(0, 4);
    release_processor();

    if (i == 10 && j == 10) {
        debugPrint("TEST 2 OK");
        num_successful_tests++;
    } else {
        debugPrint("TEST 2 FAIL");
    }

		cnum[0] = num_successful_tests + '0';
    uart0_send_string("G019_test: ");
    uart0_send_string((uint8_t *)cnum);
    uart0_send_string("/2 tests OK\r\n");
		cnum[0] = (2 - num_successful_tests) + '0';
    uart0_send_string("G019_test: ");
    uart0_send_string((uint8_t *)cnum);
    uart0_send_string("/2 tests FAIL\r\n");

    debugPrint("END");

    while (1) {
        release_processor();
    }
}

/* Starts low */
void uproc_priority2(void)
{
    while (j < 10) {
        j++;
        release_processor();

        if (i != 10) {
            set_my_priority(3);
        }
    }

    set_my_priority(3);
    set_priority(0, 3);
    release_processor();

    while (1) {
        release_processor();
    }

}

void uproc_alloc_all(void)
{
    void *mem[100];
    volatile int i = 0;

    debugPrint("START");
    debugPrint("total 2 tests");

    while (mmu_can_alloc_mem()) {
        mem[i++] = s_request_memory_block();
    }

    s_request_memory_block();
    TEST_MEM_BLOCK = 1;
    debugPrint("TEST 1 OK");
    num_successful_tests++;
    set_my_priority(3);
    set_priority(0, 3);

    while (i >= 0) {
        s_release_memory_block(mem[--i]);
    }

    for (;;) {
        release_processor();
    }
}

void uproc_alloc1(void)
{
    void *m = s_request_memory_block();

    set_my_priority(2);
    release_processor();
    s_release_memory_block(m);
    set_my_priority(3);

    for (;;) {
        release_processor();
    }
}



static volatile size_t clock_h = 0, clock_m = 0, clock_s = 0;
void ucmd_set_time(const char *data)
{
    //TODO(sandy): jesus this is ugly
    clock_h = (data[0] - '0') * 10 + (data[1] - '0');
    clock_m = (data[3] - '0') * 10 + (data[4] - '0');
    clock_s = (data[5] - '0') * 10 + (data[6] - '0');
}

uint8_t time_str[9];

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
    cmd_register("HS", ucmd_set_time);
	
    time_str[8] = 0;
    time_str[2] = time_str[4] = ':';

    msg = (msg_envelope_t *)s_request_memory_block();

    for (;;) {
        delayed_send(proc_get_pid(), msg, 1000);
        ucmd_format_time();
        uart0_send_string(time_str);
        uart0_send_string("\r");
			
        receive_message(NULL);

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
