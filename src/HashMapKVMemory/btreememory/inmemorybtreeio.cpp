//
// Created by Aquib Nawaz on 01/11/24.
//

#include "inmemorybtreeio.h"
#include <cassert>

BNode* InMemoryBTreeIO ::get(uint64_t key) {
    assert(memory.count(key));
    return nodeFactory->createNode(memory[key], false);
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

void InMemoryBTreeIO::commit(uint64_t ) {}

InMemoryBTreeIO::InMemoryBTreeIO(BNodeFactory *factory) {
    count = 0;
    nodeFactory = factory;
}

InMemoryBTreeIO::~InMemoryBTreeIO() {
    for(auto it : memory){
        delete it.second;
    }
}
