#include "cmd.h"
#include "debug_print.h"
#include "msg.h"
#include "process.h"

static int COMMANDS[NUM_COMMANDS];
static msg_envelope_t* reserved_hotkey_envelope;

void cmd_init(void)
{
    int i;
	  reserved_hotkey_envelope = (msg_envelope_t*)s_request_memory_block();
    for (i = 0; i < NUM_COMMANDS; ++i) {
        COMMANDS[i] = -1;
    }
}

size_t hash(const char *tag)
{
    int h = 1;
    const char *c = tag;

    while (c && *c) {
        h <<= (*c % 4);
        h |= *c;
        h = ~(h ^ -h);

        ++c;
    }

    return h % NUM_COMMANDS;
}

int cmd_get(const char *tag) {
    return COMMANDS[hash(tag)];
}

void cmd_put(const char *tag, int pid)
{
    int *cmd = &COMMANDS[hash(tag)];

    if (*cmd == -1) {
        *cmd = pid;
    } else {
        debugPrint("FUCK THAT HASH IS BAD");
        *((int *)NULL) = 0;
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
	
	/*
    char *c = &buffer[0];
    int cmd;
    //int wasSpace;

    if (*c != '%') {
        return;
    }

    while (*c && *c != ' ') {
        ++c;
    }

    //wasSpace = *c == ' ';
    *c = 0;

    cmd = COMMANDS[hash(buffer + 1)];

    if (cmd != -1) {
        // c + (wasSpace ? 1 : 0) is the the message payload
    }
		*/
}

void k_cmd_hotkey(char hotkey)
{
	reserved_hotkey_envelope->header.ctrl = hotkey;
	send_kernel_message(HOTKEY_PROC, proc_get_pid(), reserved_hotkey_envelope);
	reserved_hotkey_envelope = (msg_envelope_t *)s_request_memory_block();
}
