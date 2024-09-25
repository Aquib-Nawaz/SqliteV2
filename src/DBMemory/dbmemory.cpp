//
// Created by Aquib Nawaz on 24/09/24.
//

#include "dbmemory.h"
#include <cassert>

uint8_t * DBMemory::get(uint64_t key) {
    assert(memory.count(key));
    auto ret = new uint8_t [4096];
    memcpy(ret, memory[key], 4096);
    return ret;
}

void DBMemory::del(uint64_t key) {
    assert(memory.count(key));
    delete memory[key];
    memory.erase(key);
}

uint64_t DBMemory::insert(uint8_t  *val, int length) {
    auto ret = new uint8_t[length];
    memcpy(ret, val, length);
    memory[++count] = ret;
    delete val;
    return count;
}

DBMemory::DBMemory() {
    count = 0;
}

DBMemory::~DBMemory() {
    for(auto it : memory){
        delete it.second;
    }
}


