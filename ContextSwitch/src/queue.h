#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mmu.h"

#define MAX_QUEUE_LENGTH (MMU_BLOCK_SIZE - 2 * sizeof(size_t))

typedef struct {
    size_t start, end;
    
    union {
        char bytes[MAX_QUEUE_LENGTH];
        void **elements;
    } data;
} queue_t;

int q_enqueue(queue_t *q, void* element);
void *q_dequeue(queue_t *q);

#endif
