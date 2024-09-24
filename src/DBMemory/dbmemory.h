//
// Created by Aquib Nawaz on 24/09/24.
//

#ifndef SQLITEV2_DBMEMORY_H
#define SQLITEV2_DBMEMORY_H
#include <cstdint>
#include <unordered_map>

class DBMemory {
    std::unordered_map<uint64_t , uint8_t *> memory;
    uint64_t count;
    public:
    DBMemory();
    ~DBMemory();
    uint8_t * get(uint64_t);
    void del(uint64_t);
    uint64_t insert(uint8_t *,int);
};


#endif //SQLITEV2_DBMEMORY_H
