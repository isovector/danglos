#include "queue.h"

int q_enqueue(queue_t *q, void* element) {
    int next = (q->end + 1) % MAX_QUEUE_LENGTH;
    
    if (next == q->start) {
        return 1;
    }
    
    q->data.elements[next] = element;
    q->end = next;
    
    return 0;
}

void *q_dequeue(queue_t *q) {
    void *element;
    
    if (q->start == q->end) {
        return NULL;
    }
    
    element = q->data.elements[q->start];
    q->start = (q->start + 1) % MAX_QUEUE_LENGTH;
    
    return element;
}
