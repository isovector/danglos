#include "harness.h"

#include <stdlib.h>

memoryTable table;


void set_bit(char *bitVector, size_t bit) {
    bitVector[bit / 8] |= 1 << (bit % 8);
}

void* s_request_memory_block() {
    void *allocMem;
    
    if (!table.block) {
        return NULL;
    }
    else if (table.block->next) {
        allocMem = table.block->start;
        table.block = table.block->next;
    }
    else
    {
        allocMem = table.block->start;
        table.block->start += BLOCK_SIZE;
    }
    
    return allocMem;
}

int s_release_memory_block(void* memory_block) {
    return 0;
}

void init() {
    size_t requiredBytes;
    size_t requiredBlocks;
    size_t i;
    block *startBlock;

    requiredBlocks = 1;
    requiredBytes = sizeof(table) + NUM_BLOCKS * sizeof(block);
    
    while (requiredBytes > BLOCK_SIZE) {
        ++requiredBlocks;
        requiredBytes -= BLOCK_SIZE;
    }
    
    table.block = (block*)(MEMORY_START + requiredBlocks * BLOCK_SIZE);
    
    for (i = 0; i < NUM_BLOCKS / 8; ++i)
        table.bitVector[i] = 0;
    
    
    for (i = 0; i < requiredBlocks; ++i)
        set_bit(table.bitVector, i);
    
    startBlock = table.block;
    startBlock->start = (char*)&startBlock;
    startBlock->end = MEMORY_START + MEMORY_SIZE;
    startBlock->next = NULL;
}

int main() {
    
}
