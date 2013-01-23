#include "harness.h"

#include <stdio.h>
#include <stdlib.h>

memoryTable table;


void set_bit(char *bitVector, size_t bit, int on) {
    if (on != 0)
        bitVector[bit / 8] |= 1 << (bit % 8);
    else
        bitVector[bit / 8] &= ~(1 << (bit % 8));
}

char get_bit(char *bitVector, size_t bit) {
    return bitVector[bit / 8] & (1 << (bit % 8));
}

size_t getBitFromAddress(void *address) {
    size_t index;
    index = (char*)address - MEMORY_START;
    
    return index / BLOCK_SIZE;
}

void* s_request_memory_block() {
    void *allocMem;
    block *b;
    
    if (!table.block) {
        return NULL;
    }
    else if (table.block->next) {
        allocMem = table.block->start;
        b = table.block;
        table.block = b->next;
        b->next = NULL;
    }
    else
    {
        allocMem = table.block->start;
        table.block->start += BLOCK_SIZE;
        
        if (table.block->start >= table.block->end) {
            table.block = NULL;
        }
    }
    
    set_bit(table.bitVector, getBitFromAddress(allocMem), 1);
    
    return allocMem;
}

int s_release_memory_block(void* memory_block) {
    size_t index;
    char *mem;
    block *b;
    
    mem = (char*)memory_block;
    
    if (mem < (char*)MEMORY_START) {
        return 1;
    }
    
    index = mem - MEMORY_START;
    
    if (index % BLOCK_SIZE != 0) {
        return 2;
    }
    
    index = getBitFromAddress(memory_block);
    
    if (get_bit(table.bitVector, index) == 0) {
        return 3;
    }
    
    set_bit(table.bitVector, index, 0);
    
    b = (block*)MEMORY_START + index;
    
    b->start = mem;
    b->end = mem + BLOCK_SIZE;
    b->next = table.block;
    table.block = b;
    
    return 0;
}

void initMemory(void) {
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
        set_bit(table.bitVector, i, 1);
    
    startBlock = table.block;
    startBlock->start = (char*)startBlock;
    startBlock->end = MEMORY_START + MEMORY_SIZE;
    startBlock->next = NULL;
}
