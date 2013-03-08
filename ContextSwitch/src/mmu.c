#include "mmu.h"
#include "rtx.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

mmu_memtable_t table;


void set_bit(char *bitVector, size_t bit, int on)
{
    if (on != 0) {
        bitVector[bit / 8] |= 1 << (bit % 8);
    } else {
        bitVector[bit / 8] &= ~(1 << (bit % 8));
    }
}

char get_bit(char *bitVector, size_t bit)
{
    return bitVector[bit / 8] & (1 << (bit % 8));
}

size_t getBitFromAddress(void *address)
{
    size_t index;
    index = (char *)address - MMU_MEMORY_START;

    return index / MMU_BLOCK_SIZE;
}

int mmu_can_alloc_mem()
{
    return table.block != NULL;
}

void *s_request_memory_block()
{
    void *allocMem;
    mmu_blockdesc_t *b;

    if (!mmu_can_alloc_mem()) {
        block_and_release_processor();
    }

    if (table.block->next) {
        allocMem = table.block->start;
        b = table.block;
        table.block = b->next;
        b->next = NULL;
    } else {
        allocMem = table.block->start;
        table.block->start += MMU_BLOCK_SIZE;

        if (table.block->start >= table.block->end) {
            table.block = NULL;
        }
    }

    set_bit(table.bitVector, getBitFromAddress(allocMem), 1);

    return allocMem;
}

int s_release_memory_block(void *memory_block)
{
    size_t index;
    char *mem;
    mmu_blockdesc_t *b;

    mem = (char *)memory_block;

    if (mem < (char *)MMU_MEMORY_START) {
        return ERR_MMU_EARLY_OFFSET;
    }

    index = mem - MMU_MEMORY_START;

    if (index % MMU_BLOCK_SIZE != 0) {
        return ERR_MMU_UNALIGNED_PTR;
    }

    index = getBitFromAddress(memory_block);

    if (get_bit(table.bitVector, index) == 0) {
        return ERR_MMU_ALREADY_FREE;
    }

    set_bit(table.bitVector, index, 0);

    b = (mmu_blockdesc_t *)MMU_MEMORY_START + index;

    b->start = mem;
    b->end = mem + MMU_BLOCK_SIZE;
    b->next = table.block;
    table.block = b;

    return 0;
}

void initMemory(void)
{
    size_t requiredBytes;
    size_t requiredBlocks;
    size_t i;
    mmu_blockdesc_t *startBlock;

    requiredBlocks = 1;
    requiredBytes = sizeof(table) + MMU_NUM_BLOCKS * sizeof(mmu_blockdesc_t);

    while (requiredBytes > MMU_BLOCK_SIZE) {
        ++requiredBlocks;
        requiredBytes -= MMU_BLOCK_SIZE;
    }

    table.block = (mmu_blockdesc_t *)(MMU_MEMORY_START + requiredBlocks * MMU_BLOCK_SIZE);

    for (i = 0; i < MMU_NUM_BLOCKS / 8; ++i) {
        table.bitVector[i] = 0;
    }

    for (i = 0; i < requiredBlocks; ++i) {
        set_bit(table.bitVector, i, 1);
    }

    startBlock = table.block;
    startBlock->start = (char *)startBlock;
    startBlock->end = MMU_MEMORY_START + MMU_MEMORY_SIZE;
    startBlock->next = NULL;
}
