//
// Created by Aquib Nawaz on 27/09/24.
//

#ifndef SQLITEV2_PAGEDB_H
#define SQLITEV2_PAGEDB_H

#include "btree.h"
#include <vector>
#include <utility>
#define OUTPUT_ERROR(x,y) do{if((x)==-1){perror(y);exit(1);}}while(0);
#define RETURN_ON_ERROR(x,y)do{if((x)<0){perror(y);return x;}}while(0);
#define DB_VERSION "SQLITEV2.0"

struct MMapChunk{
    std::vector<std::pair<uint8_t*, long long >> chunks;
    long long size;
};
class DiskPageDBMemoryTest;

class DiskPageDBMemory: public BTree {
    int fd;
    uint64_t flushed;
    std::vector<uint8_t *> pagesToAppend;
    struct MMapChunk mmapChunk;
    int updateFile();
    int writePages();
    void extendMMap(long long);
    void getMetaData(uint8_t*);
    void loadMeta(uint8_t*);
    int updateRoot();
    int updateOrRevert(uint8_t*);
public:
    ~DiskPageDBMemory() override;
     BNode * get(uint64_t) override;
    void del(uint64_t) override;
    uint64_t insert(BNode *) override;
    DiskPageDBMemory(const char* );
    void Insert(std::vector<uint8_t> &, std::vector<uint8_t> &) override;
    friend class DiskPageDBMemoryTest;
};


#endif //SQLITEV2_PAGEDB_H
