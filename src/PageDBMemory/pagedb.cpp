//
// Created by Aquib Nawaz on 27/09/24.
//

#include "pagedb.h"

void DiskPageDBMemory::Insert(std::vector<uint8_t> & key, std::vector<uint8_t> & val,
                              UpdateResult& res) {
    root = mmapChunk->getRoot();
    //tree root and memory root are in sync now
    auto meta = mmapChunk->getMetaData();
    BTree::Insert(key, val, res);
    //updated the memory root
    mmapChunk->setRoot(root);
    mmapChunk->updateOrRevert(meta);
    delete []meta;
}

std::vector<uint8_t> DiskPageDBMemory::Get(std::vector<uint8_t> &key) {
    root = mmapChunk->getRoot();
    return BTree::Get(key);
}

bool DiskPageDBMemory::Delete(std::vector<uint8_t> & key) {
    root = mmapChunk->getRoot();
    bool ret = BTree::Delete(key);
    mmapChunk->setRoot(root);
    return ret;
}

DiskPageDBMemory::DiskPageDBMemory(class MMapChunk* _mmapChunk) {
    mmapChunk = _mmapChunk;
}

uint64_t DiskPageDBMemory::insert(BNode * node) {
    uint64_t ptr = mmapChunk->insert( node->getData());
    node->resetData();
    delete node;
    return ptr;
}

BNode *DiskPageDBMemory::get(uint64_t ptr) {
    auto data = mmapChunk->get(ptr);
    return new BNode(data, false);
}
void DiskPageDBMemory::del(uint64_t ptr) {
    mmapChunk->del(ptr);
}

DiskPageDBMemory::~DiskPageDBMemory() = default;
