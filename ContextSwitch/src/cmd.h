#ifndef _CMD_H_
#define _CMD_H_

#include <string.h>
#include "config.h"

#define NUM_COMMANDS 32

extern void cmd_init(void);

//extern bool cmd_register(const char *tag, int func);
extern void k_cmd_send(char *buffer);
extern void k_cmd_hotkey(char hotkey);
extern int cmd_get(const char *tag);
extern void cmd_put(const char *tag, int pid);

#endif
