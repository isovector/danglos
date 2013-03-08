#include "cmd.h"
#include "debug_print.h"
static cmd_func COMMANDS[NUM_COMMANDS] = { NULL };

size_t hash(const char *tag)
{
    int h = 1;
    const char *c = &tag[0];

    while (c) {
        h <<= (*c % 4);
        h |= *c;
        h = ~(h ^ -h);

        ++c;
    }

    return h % NUM_COMMANDS;
}

bool k_cmd_register(const char *tag, cmd_func func)
{
    cmd_func *cmd = &COMMANDS[hash(tag)];

    if (!cmd) {
        *cmd = func;
    } else {
        debugPrint("FUCK THAT HASH IS BAD");
        *((int *)NULL) = 0;
    }

    return true;
}

void k_cmd_send(char *buffer)
{
    char *c = &buffer[0];
    cmd_func cmd;
    int wasSpace;

    if (*c != '%') {
        return;
    }

    while (c && *c != ' ') {
        ++c;
    }

    wasSpace = *c == ' ';
    *c = 0;

    cmd = COMMANDS[hash(buffer + 1)];

    if (cmd) {
        //TODO(sandy): disable kernel mode
        cmd(c + (wasSpace ? 1 : 0));
        //TODO(sandy): enable kernel mode
    }
}
