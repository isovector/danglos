#include "harness.h"

#include <iostream>
using namespace std;

memoryTable table;

void* s_request_memory_block() {
    for (size_t i = 0; i < table.blocks; ++i) {
        block &b = table.table[i];
        
        if (!b.owned) {
            b.owned = true;
            return &b;
        }
    }
    
    return NULL;
}

int s_release_memory_block(void* memory_block) {
    if (!(memory_block >= memory_layout.memory && memory_block < memory_layout.memory + memory_layout.size)) {
        return 1;
    }
    
    block &b = *reinterpret_cast<block*>(memory_block);
    b.owned = false;
    
    return 0;
}

void init() {
    size_t availableBlocks = memory_layout.size / BLOCK_SIZE;
    size_t sizeWeNeed = availableBlocks * sizeof(block);
    size_t blocks = 1;
    
    while (sizeWeNeed > BLOCK_SIZE) {
        ++blocks;
        sizeWeNeed -= BLOCK_SIZE;
    }
    
    table.blocks = availableBlocks - blocks;
    table.table = reinterpret_cast<block*>(&memory_layout.memory[0]);
    for (size_t i = 0; i < table.blocks; ++i) {
        block &b = table.table[i];
        b.pid = 0;
        b.owned = false;
        b.ptr = &memory_layout.memory[(blocks + i) * sizeof(block)];
    }
}

int main() {
    init();
    
    void *a = s_request_memory_block();
    void *b = s_request_memory_block();
    
    cout << reinterpret_cast<size_t>(a) << endl; 
    cout << reinterpret_cast<size_t>(b) << endl; 
    cout << s_release_memory_block(a) << endl;
    cout << s_release_memory_block(NULL) << endl;
    cout << reinterpret_cast<size_t>(s_request_memory_block()) << endl; 
}
