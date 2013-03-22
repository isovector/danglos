#include "config.h"
#include "cmd.h"
#include "debug_print.h"
#include "msg.h"
#include "process.h"
#include <string.h>

static msg_envelope_t* reserved_hotkey_envelope;

typedef struct {
    char tag[5];
    int pid;
} command_t;

static command_t COMMANDS[NUM_COMMANDS];

command_t *lookup(const char *tag, bool insert) {
    size_t i = 0;
    command_t *cmd;
    
    for (; i < NUM_COMMANDS; i++) {
        cmd = &COMMANDS[i];
        if (!cmd->tag[0]) {
            if (!insert) {
                return NULL;
            }
            
            strcpy(cmd->tag, tag);
            return cmd;
        }
        
        if (strcmp(tag, cmd->tag) == 0) {
            return cmd;
        }
    }
    
    return NULL;
}

void cmd_init(void)
{
    int i;
    reserved_hotkey_envelope = (msg_envelope_t*)s_request_memory_block();
    for (i = 0; i < NUM_COMMANDS; ++i) {
        COMMANDS[i].tag[0] = 0;
        COMMANDS[i].pid = -1;
    }
}

int cmd_get(const char *tag) {
    command_t *cmd = lookup(tag, false);
    
    return cmd ? cmd->pid : -1;
}

void cmd_put(const char *tag, int pid)
{
    command_t *cmd = lookup(tag, true);
    if (cmd->pid == pid || cmd->pid == -1) {
        cmd->pid = pid;
    } else {
        debugPrint("re-registering cmd tag with new pid");
    }
}

char* cmd_parse(char* c) {
    int wasSpace;

    while (*c && *c != ' ') {
        ++c;
    }
    wasSpace = *c == ' ';
		if (wasSpace) {
			wasSpace = wasSpace;
		}
    *c = 0;
    return c + (wasSpace ? 1 : 0);
}

void k_cmd_send(char *buffer)
{
	msg_envelope_t* msg;
	msg = (msg_envelope_t*)s_request_memory_block();
	
	msg->data[0] = NOTIFY;
	msg->header.dest = CMD_DECODER_PID;
	msg->header.next = NULL;
	msg->header.src = -1;
	strcpy(&(msg->data[1]), buffer);
	msg_send_message(msg, 1);
}

void k_cmd_hotkey(char hotkey)
{
	reserved_hotkey_envelope->header.ctrl = hotkey;
	send_kernel_message(HOTKEY_PROC, proc_get_pid(), reserved_hotkey_envelope);
	reserved_hotkey_envelope = (msg_envelope_t *)s_request_memory_block();
}
