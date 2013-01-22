#ifndef HARNESS_H
#define HARNESS_H

#include <cstring>

#define EMULATE

#ifdef EMULATE
#define MEMORY_SIZE 65536000
#else
#endif

static const size_t BLOCK_SIZE = 65535;

struct layout {
    #ifdef EMULATE
    char memory[MEMORY_SIZE];
    const size_t size = MEMORY_SIZE;
    #else
    #endif
} memory_layout;

struct block {
    size_t pid;
    bool owned;
    void *ptr;
};

struct memoryTable {
    size_t blocks;
    block *table;
};

#endif
