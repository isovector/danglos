#include "error.h"

const char *err_lookup(error_t err)
{
    switch (err) {
#define error(m) case m: return #m + 4;
#include "error_list.h"
#undef error

        default:
            return "SUCCESS";
    }
}
