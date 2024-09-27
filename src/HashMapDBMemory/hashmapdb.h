//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_HASHMAP_H
#define SQLITEV2_HASHMAP_H

#include "dbmemory.h"

class HashMapDBMemory: public DBMemory {
    uint64_t count;
    std::unordered_map<uint64_t, uint8_t*> memory;
public:
    HashMapDBMemory();
    ~HashMapDBMemory();

    uint8_t * get(uint64_t);
    void del(uint64_t);
    uint64_t insert(uint8_t*, int);
};

#endif //SQLITEV2_HASHMAP_H
