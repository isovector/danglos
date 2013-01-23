#ifndef HARNESS_H
#define HARNESS_H

#include <string.h>

#ifdef EMULATE
#define MEMORY_SIZE 0x8000
char MEMORY_START[MEMORY_SIZE];
#else
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
#define MEMORY_SIZE 0x8000
#define MEMORY_START ((char*)&Image$$RW_IRAM1$$ZI$$Limit)
#endif

#define BLOCK_SIZE 512
#define NUM_BLOCKS (MEMORY_SIZE / BLOCK_SIZE)

typedef struct structBlock {
    char *start, *end;
    struct structBlock *next;
} block;

typedef struct structMemoryTable {
    char bitVector[NUM_BLOCKS / 8];
    block *block;
} memoryTable;

extern void initMemory(void);
extern void * s_request_memory_block(void);
#endif
