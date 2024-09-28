//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_HASHMAP_H
#define SQLITEV2_HASHMAP_H

#include "btree.h"
#include <unordered_map>

class HashMapDBMemory: public BTree {
    uint64_t count;
    std::unordered_map<uint64_t, uint8_t*> memory;
public:
    HashMapDBMemory();
    ~HashMapDBMemory();

    BNode * get(uint64_t) override;
    void del(uint64_t) override;
    uint64_t insert(BNode*) override;
};

#endif //SQLITEV2_HASHMAP_H
