#include "queue.h"
#include "error.h"

int q_enqueue(queue_t *q, void* element) {
    int next = (q->end + 1) % MAX_QUEUE_LENGTH;
    
    if (next == q->start) {
        return ERR_Q_NO_SPACE;
    }
    
    q->elements[next] = element;
    q->end = next;
    
    return 0;
}

void *q_dequeue(queue_t *q) {
    void *element;
    
    if (q->start == q->end) {
        return NULL;
    }
    
    element = q->elements[q->start];
    q->start = (q->start + 1) % MAX_QUEUE_LENGTH;
    
    return element;
}
