#ifndef _MSG_H_
#define _MSG_H_

#include "mmu.h"

struct message_envelope;

typedef enum { USER_MSG, SYSTEM_MSG, CMD_REGISTER_MSG, CMD_NOTIFY_MSG, CMD_HOTKEY_MSG } msg_type_t;
typedef enum {SYSTEM_MANAGED, USER_MANAGED} mem_type_t;
typedef struct {
    msg_type_t type;
    char ctrl;
    
    int dest, src;
    size_t len;
    struct message_envelope *next;
    
    uint32_t tick;
		mem_type_t memory_type;
} msg_header_t;

#define MAX_MESSAGE_LENGTH (MMU_BLOCK_SIZE - sizeof(msg_header_t))

typedef struct message_envelope {
    msg_header_t header;
    char data[MAX_MESSAGE_LENGTH];
} msg_envelope_t;


extern void *receive_message(int *sender);

extern int send_kernel_message(int dest, int src, void *pmsg);
extern int send_message(int pid, void *msg);
extern int delayed_send(int pid, void *msg, uint32_t delay);
extern void msg_init_envelope(void *pmsg, int src, int dest);
extern msg_envelope_t * alloc_message(bool managed);
extern void free_message(void *pmsg);

void msg_tick(uint32_t);
#endif
