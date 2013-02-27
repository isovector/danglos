#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mmu.h"

#define MAX_QUEUE_SIZE (MMU_BLOCK_SIZE - 2 * sizeof(size_t))
#define MAX_QUEUE_LENGTH (MAX_QUEUE_SIZE / sizeof(void*))

typedef struct {
    size_t start, end;
    
    union {
        char bytes[MAX_QUEUE_SIZE];
        void **elements;
    } data;
} queue_t;

int q_enqueue(queue_t *q, void* element);
void *q_dequeue(queue_t *q);

#endif
