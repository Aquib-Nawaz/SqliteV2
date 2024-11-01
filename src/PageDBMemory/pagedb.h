//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_PAGEDB_H
#define SQLITEV2_PAGEDB_H

#include "btree.h"
#include <vector>
#include <utility>
#include "mmap.h"

class DiskPageDBMemory: public BTree {
    class MMapChunk* mmapChunk;
public:
    ~DiskPageDBMemory() override;
     BNode * get(uint64_t) override;
    void del(uint64_t) override;
    uint64_t insert(BNode *) override;
    explicit DiskPageDBMemory(class MMapChunk*);
    void Insert(std::vector<uint8_t> &, std::vector<uint8_t> &, UpdateResult&) override;
    std::vector<uint8_t> Get(std::vector<uint8_t> &) override;
    bool Delete(std::vector<uint8_t >&, DeleteResult&) override;
};


#endif //SQLITEV2_PAGEDB_H
