//
// Created by Aquib Nawaz on 01/11/24.
//

#include "diskbtreeio.h"

DiskBTreeIO::DiskBTreeIO(MMapChunk *_mmap, BNodeFactory *_factory)
    : mmap(_mmap), nodeFactory(_factory) {}

uint64_t DiskBTreeIO::insert(BNode *node) {
    uint64_t ptr = mmap->insert( node->getData());
    node->resetData();
    delete node;
    return ptr;
}

BNode *DiskBTreeIO::get(uint64_t ptr) {
    auto data = mmap->get(ptr);
    return nodeFactory->createNode(data, false);
}

void DiskBTreeIO::del(uint64_t ptr) {
    mmap->del(ptr);
}

DiskBTreeIO::~DiskBTreeIO() = default;