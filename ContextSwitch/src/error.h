#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum {
    SUCCESS = 0,
    
#define error(m) m,
#include "error_list.h"
#undef error
    
} error_t;


const char *err_lookup(error_t err) {
    switch(err) {
#define error(m) case m: return #m + 4;
#include "error_list.h"
#undef error
        default: return "SUCCESS";
    }
}

#endif
