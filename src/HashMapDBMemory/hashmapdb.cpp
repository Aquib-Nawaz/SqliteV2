//
// Created by Aquib Nawaz on 24/09/24.
//

#include <cassert>
#include "hashmapdb.h"
#include <cstring>

BNode * HashMapDBMemory::get(uint64_t key) {
    assert(memory.count(key));
    auto ret = new uint8_t [BTREE_PAGE_SIZE];
    memcpy(ret, memory[key], BTREE_PAGE_SIZE);
    return new BNode(ret);
}

void HashMapDBMemory::del(uint64_t key) {
    assert(memory.count(key));
    delete memory[key];
    memory.erase(key);
}

uint64_t HashMapDBMemory::insert(BNode  *val) {
    memory[++count] = val->getData();
    val->resetData();
    delete val;
    return count;
}

HashMapDBMemory::HashMapDBMemory() {
    count = 0;
}

HashMapDBMemory::~HashMapDBMemory() {
    for(auto it : memory){
        delete it.second;
    }
}


