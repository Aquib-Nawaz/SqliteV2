//
// Created by Aquib Nawaz on 01/11/24.
//

#include "inmemorybtreeio.h"
#include <cassert>

BNode* InMemoryBTreeIO ::get(uint64_t key) {
    assert(memory.count(key));
    auto ret = new uint8_t [BTREE_PAGE_SIZE];
    memcpy(ret, memory[key], BTREE_PAGE_SIZE);
    return new BNode(ret);
}

void InMemoryBTreeIO::del(uint64_t key) {
    assert(memory.count(key));
    delete memory[key];
    memory.erase(key);
}

uint64_t InMemoryBTreeIO::insert(BNode  *val) {
    memory[++count] = val->getData();
    val->resetData();
    delete val;
    return count;
}

InMemoryBTreeIO::InMemoryBTreeIO()  {
    count = 0;
}

InMemoryBTreeIO::~InMemoryBTreeIO() {
    for(auto it : memory){
        delete it.second;
    }
}