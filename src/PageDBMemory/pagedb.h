//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_PAGEDB_H
#define SQLITEV2_PAGEDB_H

#include "dbmemory.h"
class DiskPageDBMemory: public DBMemory {
    int fd;

public:
    ~DiskPageDBMemory();
    virtual uint8_t * get(uint64_t);
    virtual void del(uint64_t);
    virtual uint64_t insert(uint8_t *,int);
};


#endif //SQLITEV2_PAGEDB_H
