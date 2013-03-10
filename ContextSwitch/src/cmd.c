#include "cmd.h"
#include "debug_print.h"
static int COMMANDS[NUM_COMMANDS] = { -1 };

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

bool cmd_register(const char *tag, int pid)
{
    cmd_put(tag, pid);

    return true;
}

void k_cmd_send(char *buffer)
{
    char *c = &buffer[0];
    int cmd;
    int wasSpace;

    if (*c != '%') {
        return;
    }

    while (*c && *c != ' ') {
        ++c;
    }

    wasSpace = *c == ' ';
    *c = 0;

    cmd = COMMANDS[hash(buffer + 1)];

    if (cmd != -1) {
        // c + (wasSpace ? 1 : 0) is the the message payload
    }
}

void k_cmd_hotkey(char hotkey)
{
    // send message to sysproc_hotkeys
}
