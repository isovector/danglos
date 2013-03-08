#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum {
    SUCCESS = 0,

#define error(m) m,
#include "error_list.h"
#undef error

} error_t;


extern const char *err_lookup(error_t err);

#endif
