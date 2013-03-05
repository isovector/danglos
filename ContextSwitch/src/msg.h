#ifndef _MSG_H_
#define _MSG_H_

#include "mmu.h"

struct message_envelope;

typedef struct {
    int type, dest, src, len;
    char ctrl;
    struct message_envelope *next;
} msg_header_t;

#define MAX_MESSAGE_LENGTH (MMU_BLOCK_SIZE - sizeof(msg_header_t))

typedef struct message_envelope {
    msg_header_t header;
    char data[MAX_MESSAGE_LENGTH];
} msg_envelope_t;


extern void *receive_message(int *sender);

// the lab manual says these should return an int, but doesn't say what
extern int send_message(int pid, void *msg);
extern int delayed_send(int pid, void *msg, int delay);

#endif
