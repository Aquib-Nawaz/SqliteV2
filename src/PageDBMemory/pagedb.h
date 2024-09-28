//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_PAGEDB_H
#define SQLITEV2_PAGEDB_H

#include "dbmemory.h"
#include <vector>
#include <utility>
#define OUTPUT_ERROR(x,y) do{if((x)==-1){perror(y);exit(1);}}while(0);
#define PAGE_SIZE 4096
#define DB_VERSION "SQLITEV2.0"

struct MMapChunk{
    std::vector<std::pair<uint8_t*, size_t >> chunks;
    size_t size;
};

class DiskPageDBMemory: public DBMemory {
    int fd;
    std::vector<uint8_t *> pagesToAppend;
    MMapChunk mmapChunk;
public:
    ~DiskPageDBMemory();
     uint8_t * get(uint64_t);
    void del(uint64_t);
    uint64_t insert(uint8_t *,int);
    DiskPageDBMemory(const char* );
    uint64_t getRoot() override;
};


#endif //SQLITEV2_PAGEDB_H
