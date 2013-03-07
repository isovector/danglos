#ifndef _CMD_H_
#define _CMD_H_

#include <string.h>
#include "config.h"

#define NUM_COMMANDS 32

typedef void (*cmd_func)(const char *args);

extern bool k_cmd_register(const char *tag, cmd_func func);

#endif
