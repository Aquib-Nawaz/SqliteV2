//
// Created by Aquib Nawaz on 30/09/24.
//

#ifndef SQLITEV2_MMAP_H
#define SQLITEV2_MMAP_H

#include <vector>
#include <cstdint>
#include <utility>
#include <unordered_map>

#define DB_VERSION "SQLITEV2.0"
#define FREE_LIST_HEADER 8
#define FREE_LIST_PAGE_SIZE 16384
#define FREE_LIST_CAPACITY ((FREE_LIST_PAGE_SIZE-FREE_LIST_HEADER)/8)
#define OUTPUT_ERROR(x,y) do{if((x)==-1){perror(y);exit(1);}}while(0);
#define RETURN_ON_ERROR(x,y)do{if((x)<0){perror(y);return x;}}while(0);

class MMapTest;
class MMapChunk;

class FList {

    uint64_t headPage;
    uint64_t headSeq;
    uint64_t tailPage;
    uint64_t tailSeq;
    uint64_t maxSeq;
    friend class MMapChunk;
public:
    FList(uint64_t , uint64_t, uint64_t, uint64_t);
    void setMaxSeq();
    ~FList() = default;
    friend class MMapTest;
};

class MMapChunk{
    int fd;
    uint64_t root;
    uint64_t nAppend;
    uint64_t flushed;
    FList* freeList;
    std::unordered_map<uint64_t, std::pair<uint8_t*, bool>> pagesToSet;
    void extendMMap(long long);
    long long size;
    void pushBack(uint64_t );
    uint8_t *set(uint64_t );
    std::pair<uint64_t,uint64_t> popHead();
    uint64_t popFront();
    std::pair<size_t ,uint64_t> getPtrLocation(uint64_t);
    void clearPendingUpdates();
    int writePages();
    int updateFile();
    int updateRoot();
    void loadMeta(uint8_t*);
public:

    std::vector<std::pair<uint8_t*, long long >> chunks;
    ~MMapChunk();
    explicit MMapChunk(const char*);
    uint64_t insert(uint8_t *);
    void del(uint64_t);
    uint8_t* get(uint64_t);
    uint8_t* getMetaData();
    uint64_t getRoot() const;
    void setRoot(uint64_t);
    int updateOrRevert(uint8_t *);
    friend class MMapTest;
};

class LNode {
    uint8_t *data;
public:
    explicit LNode(uint8_t*);
    uint64_t getNext();
    void setNext(uint64_t);
    uint64_t getPtr(uint16_t);
    void setPtr(uint16_t,uint64_t);
    void resetData();
    ~LNode();
};

#endif
