//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_HASHMAP_H
#define SQLITEV2_HASHMAP_H

#include "btree.h"
#include <unordered_map>
#include "keyvalue.h"

class HashMapDBMemory: public  KeyValue{
    BTree* btree;
public:
    HashMapDBMemory();
    ~HashMapDBMemory() override;
    void Insert(std::vector<uint8_t> &, std::vector<uint8_t> &, UpdateResult&) override;
    std::vector<uint8_t> Get(std::vector<uint8_t> &) override;
    bool Delete(std::vector<uint8_t >&, DeleteResult&) override;
};

#endif //SQLITEV2_HASHMAP_H
