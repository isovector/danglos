#ifndef _CMD_H_
#define _CMD_H_

#include <string.h>
#include "config.h"

#define NUM_COMMANDS 32

extern bool cmd_register(const char *tag, int func);
extern void k_cmd_send(char *buffer);
extern void k_cmd_hotkey(char hotkey);

#endif
