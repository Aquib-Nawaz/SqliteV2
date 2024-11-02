//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_PAGEKV_H
#define SQLITEV2_PAGEKV_H

#include "btree.h"
#include <vector>
#include <utility>
#include "mmap.h"
#include "keyvalue.h"
#include "bnodefactory.h"

class DiskKV: public KeyValue {
    class MMapChunk* mmapChunk;
    class BTree* btree;
public:
    ~DiskKV() override;

    DiskKV(class MMapChunk*, BNodeFactory* factory);
    void Insert(std::vector<uint8_t> &, std::vector<uint8_t> &, UpdateResult&) override;
    std::vector<uint8_t> Get(std::vector<uint8_t> &) override;
    bool Delete(std::vector<uint8_t >&, DeleteResult&) override;
};


#endif //SQLITEV2_PAGEKV_H
