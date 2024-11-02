//
// Created by Aquib Nawaz on 27/09/24.
//

#include "pagekv.h"
#include "diskbtreememory/diskbtreeio.h"
void DiskKV::Insert(std::vector<uint8_t> & key, std::vector<uint8_t> & val,
                    UpdateResult& res) {
    btree->setRoot( mmapChunk->getRoot());
    //tree root and memory root are in sync now
    auto meta = mmapChunk->getMetaData();
    btree->Insert(key, val, res);
    //updated the memory root
    mmapChunk->setRoot(btree->getRoot());
    mmapChunk->updateOrRevert(meta);
    delete []meta;
}

std::vector<uint8_t> DiskKV::Get(std::vector<uint8_t> &key) {
    btree->setRoot(mmapChunk->getRoot());
    return btree->Get(key);
}

bool DiskKV::Delete(std::vector<uint8_t> & key, DeleteResult &res) {
    btree->setRoot(mmapChunk->getRoot());
    auto meta = mmapChunk->getMetaData();
    bool ret = btree->Delete(key, res);
    mmapChunk->setRoot(btree->getRoot());
    mmapChunk->updateOrRevert(meta);
    delete []meta;
    return ret;
}

DiskKV::DiskKV(class MMapChunk* _mmapChunk, BNodeFactory *factory) {
    mmapChunk = _mmapChunk;
    btree = new BTree(new DiskBTreeIO(mmapChunk, factory));
}

DiskKV::~DiskKV() = default;
