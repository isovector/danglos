#ifndef _MMU_H_
#define _MMU_H_

#include "config.h"

#include <string.h>

#define MMU_MAX_MEMORY_ADDR 0x10008000
#if EMULATION_MODE
#define MMU_MEMORY_SIZE 0x8000
#define MMU_MAX_MEMORY_SIZE 0x8000
char MMU_MEMORY_START[MMU_MEMORY_SIZE];
#else
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;
#define MMU_MEMORY_START ((char*)&Image$$RW_IRAM1$$ZI$$Limit)
#define MMU_MEMORY_SIZE ((char*)MMU_MAX_MEMORY_ADDR - MMU_MEMORY_START)
#define MMU_MAX_MEMORY_SIZE 0x4000
#endif

#define MMU_BLOCK_SIZE 512
#define MMU_NUM_BLOCKS (MMU_MEMORY_SIZE / MMU_BLOCK_SIZE)

typedef struct memory_block_descriptor {
    char *start, *end;
    struct memory_block_descriptor *next;
} mmu_blockdesc_t;

typedef struct memory_table {
    char bitVector[(MMU_MAX_MEMORY_SIZE / MMU_BLOCK_SIZE) / 8];
    mmu_blockdesc_t *block;
} mmu_memtable_t;

extern void mmu_init(void);
extern void *s_request_memory_block(void);
extern int mmu_can_alloc_mem(void);
extern int s_release_memory_block(void *);

#endif
