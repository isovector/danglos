#ifndef _CMD_H_
#define _CMD_H_

#include <string.h>
#include "config.h"

#define NUM_COMMANDS 32

extern bool k_cmd_register(const char *tag, int func);
extern void k_cmd_send(char *buffer);

#endif
