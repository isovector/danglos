#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "mmu.h"

#define MAX_QUEUE_LENGTH ((MMU_BLOCK_SIZE - 2 * sizeof(size_t)) / sizeof(void*))

typedef struct {
    size_t start, end;
    void *elements[MAX_QUEUE_LENGTH];
} queue_t;

int q_enqueue(queue_t *q, void* element);
void *q_dequeue(queue_t *q);

#endif
