#ifndef HARNESS_H
#define HARNESS_H

#include <string.h>

#define EMULATE

#ifdef EMULATE
#define MEMORY_SIZE 65536000
#define BLOCK_SIZE 65535
#define NUM_BLOCKS MEMORY_SIZE / BLOCK_SIZE
char MEMORY_START[MEMORY_SIZE];
#else
#endif


typedef struct structBlock {
    char *start, *end;
    struct structBlock *next;
} block;

typedef struct structMemoryTable {
    char bitVector[NUM_BLOCKS / 8];
    block *block;
} memoryTable;

#endif
