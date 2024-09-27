//
// Created by Aquib Nawaz on 24/09/24.
//

#ifndef SQLITEV2_DBMEMORY_H
#define SQLITEV2_DBMEMORY_H
#include <cstdint>
#include <unordered_map>

class DBMemory {

    public:

    virtual uint8_t * get(uint64_t)=0;
    virtual void del(uint64_t)=0;
    virtual uint64_t insert(uint8_t *,int)=0;
    virtual ~DBMemory() = default;
};


#endif //SQLITEV2_DBMEMORY_H
