#include "config.h"
#include "cmd.h"
#include "debug_print.h"
#include "msg.h"
#include "process.h"
#include <string.h>

static msg_envelope_t* reserved_hotkey_envelope;
static msg_envelope_t* reserved_send_envelope;

typedef struct {
    char tag[6];
    int pid;
} cmd_t;

static cmd_t COMMANDS[NUM_COMMANDS];

cmd_t *lookup(const char *tag, bool insert) {
    size_t i = 0;
    cmd_t *cmd;
    
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
    reserved_hotkey_envelope = alloc_message(true);
		reserved_send_envelope = alloc_message(true);
    for (i = 0; i < NUM_COMMANDS; ++i) {
        COMMANDS[i].tag[0] = 0;
        COMMANDS[i].pid = -1;
    }
}

int cmd_get(const char *tag) {
    cmd_t *cmd = lookup(tag, false);
    
    return cmd ? cmd->pid : -1;
}

void cmd_put(const char *tag, int pid)
{
    cmd_t *cmd = lookup(tag, true);
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

void cmd_register(const char* tag)
{
	msg_envelope_t * msg = alloc_message(false);
  msg->header.type = CMD_REGISTER_MSG;
	strcpy(msg->data, tag);
	send_message(CMD_DECODER_PID, msg);
}


void k_cmd_send(char *buffer)
{
  reserved_send_envelope->header.type = CMD_NOTIFY_MSG;
	strcpy(reserved_send_envelope->data, buffer);
    
	send_kernel_message(CMD_DECODER_PID, -1, reserved_send_envelope);
}

void k_cmd_hotkey(char hotkey)
{
    reserved_hotkey_envelope->header.type = CMD_HOTKEY_MSG;
	reserved_hotkey_envelope->header.ctrl = hotkey;
	send_kernel_message(HOTKEY_PROC, -1, reserved_hotkey_envelope);
    
}
