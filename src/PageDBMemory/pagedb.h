//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_PAGEDB_H
#define SQLITEV2_PAGEDB_H

#include "btree.h"
#include <vector>
#include <utility>
#define OUTPUT_ERROR(x,y) do{if((x)==-1){perror(y);exit(1);}}while(0);
#define PAGE_SIZE 4096
#define DB_VERSION "SQLITEV2.0"

struct MMapChunk{
    std::vector<std::pair<uint8_t*, size_t >> chunks;
    size_t size;
};

class DiskPageDBMemory: public BTree {
    int fd;
    std::vector<uint8_t *> pagesToAppend;
    MMapChunk mmapChunk;
public:
    ~DiskPageDBMemory();
     BNode * get(uint64_t) override;
    void del(uint64_t) override;
    uint64_t insert(BNode *) override;
    DiskPageDBMemory(const char* );
};


#endif //SQLITEV2_PAGEDB_H
