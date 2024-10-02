//
// Created by Aquib Nawaz on 30/09/24.
//

#include "mmap.h"
#include "convertor.h"
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cassert>
#include <fcntl.h>
#include <filesystem>

static const int BTREE_PAGE_SIZE = 16384;

uint16_t seq2Idx(uint64_t seq){
    return (uint16_t)(seq%FREE_LIST_CAPACITY);
}

FList::FList(uint64_t hp, uint64_t hs, uint64_t tp, uint64_t ts):
headPage(hp),headSeq(hs),tailPage(tp),tailSeq(ts),maxSeq(0){}

void FList::setMaxSeq() {maxSeq=tailSeq;}

LNode::LNode(uint8_t *d):data(d){}

void LNode::resetData() {data=nullptr;}

uint64_t LNode::getNext(){
    return littleEndianByteToInt64(data);
}

void LNode::setNext(uint64_t next){
    littleEndianInt64ToBytes(next,data);
}

uint64_t LNode::getPtr(uint16_t pos){
    return littleEndianByteToInt64(data+pos*8+FREE_LIST_HEADER);
}

void LNode::setPtr(uint16_t pos,uint64_t ptr){
    littleEndianInt64ToBytes(ptr,data+pos*8+FREE_LIST_HEADER);
}

LNode::~LNode(){delete [] data;}


int createFileSync(const char * filepath){
    std::filesystem::path path(filepath);
    std::filesystem::path parentPath = path.parent_path();
    if(parentPath.empty())parentPath = ".";
    std::filesystem::create_directories(parentPath);
    int dirfd = open(parentPath.c_str(), O_RDONLY|O_DIRECTORY);
    OUTPUT_ERROR(dirfd, "Directory Open")
    int fd = openat(dirfd,path.filename().c_str(), O_RDWR | O_CREAT , 0644);
    OUTPUT_ERROR(fd, "File Open")
    OUTPUT_ERROR(fsync(dirfd), "Directory Sync")
    close(dirfd);
    return fd;
}

void MMapChunk::clearPendingUpdates() {
    for(auto i : pagesToAppend){
        delete [] i;
    }
    for(auto i : pagesToSet){
        if(i.first<flushed)
            delete i.second;
    }
}

MMapChunk::~MMapChunk() {
    for(auto & i : chunks){
        OUTPUT_ERROR(munmap(i.first, i.second), "destructor unmap")
    }
    clearPendingUpdates();
    delete freeList;
    OUTPUT_ERROR( close(fd),"destructor file close")
}

void MMapChunk::extendMMap(long long _size) {
    if(_size <= size)
        return;
    long long alloc = std::max(size, 64ll<<20);
    while (size+alloc<_size)alloc<<=1;
    OUTPUT_ERROR(ftruncate(fd,size+alloc), "ftruncate extendMMap")
    auto map = (uint8_t*)mmap(nullptr, alloc,PROT_READ|PROT_WRITE, MAP_SHARED, fd, size);
    chunks.emplace_back(map, alloc);
    size+=alloc;
}

MMapChunk::MMapChunk(const char * _path) {
    fd = createFileSync(_path);
    struct stat st;
    fstat(fd, &st);
    bool empty = st.st_size==0;
    size_t versionSize = strlen(DB_VERSION);
    freeList = new FList(1, 0, 1, 0);
    if(empty){
        size = 0;
        flushed = 2;
        root=0;
    }
    else{
        uint8_t sizeData[8];
        pread(fd, sizeData, 8, versionSize+8);
        size = littleEndianByteToInt64(sizeData)*BTREE_PAGE_SIZE;
        auto map =(uint8_t *) mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(map==MAP_FAILED){
            perror("constructor mmap");
            exit(1);
        }
        chunks.emplace_back(map,  size);

        if(memcmp(DB_VERSION, map, versionSize) != 0){
            perror("Version Mismatch");
            exit(1);
        }
        loadMeta(map);
        freeList->setMaxSeq();
    }
}

uint64_t MMapChunk::insert(uint8_t * data) {
    uint64_t ptr = popFront();
    if(ptr != 0){
        pagesToSet[ptr] = data;
        return ptr;
    }
    pagesToAppend.push_back(data);
    return flushed+pagesToAppend.size()-1;
}

std::pair<size_t, uint64_t > MMapChunk::getPtrLocation(uint64_t  ptr){
    uint64_t cur=0;
    size_t chunkNum ;
    for(chunkNum=0;chunkNum<chunks.size()&&cur+
                                           chunks[chunkNum].second/BTREE_PAGE_SIZE<=ptr;chunkNum++)
        cur+=chunks[chunkNum].second/BTREE_PAGE_SIZE;
    assert(chunkNum<chunks.size());
    return {chunkNum,(ptr-cur)*BTREE_PAGE_SIZE};
}

uint8_t * MMapChunk::get(uint64_t ptr) {
    assert(ptr <= flushed);
    if(pagesToSet.count(ptr)!=0){
        auto data = new uint8_t [BTREE_PAGE_SIZE];
        memcpy(data ,pagesToSet[ptr],BTREE_PAGE_SIZE);
        return data;
    }
    auto [chunkNum,offset] = getPtrLocation(ptr);
    auto *data = new uint8_t [BTREE_PAGE_SIZE];
    memcpy(data ,chunks[chunkNum].first+offset,BTREE_PAGE_SIZE);
    return data;
}

uint64_t MMapChunk::getRoot() const {
    return root;
}

void MMapChunk::setRoot(uint64_t _root) {
    root = _root;
}

int MMapChunk::writePages() {
    long long _size = (long long)(flushed+pagesToAppend.size())*BTREE_PAGE_SIZE;
    extendMMap(_size);
    long long offset = (long long)flushed*BTREE_PAGE_SIZE;
    size_t chunkNum = 0;
    while (chunkNum<chunks.size() &&
           offset>=chunks[chunkNum].second)
    {
        offset-=chunks[chunkNum++].second;
    }
    long long startOffset = offset;
    for(auto page:pagesToAppend){
        assert(chunkNum<chunks.size());
        memcpy(chunks[chunkNum].first+offset,page,BTREE_PAGE_SIZE);
        offset+=BTREE_PAGE_SIZE;
        if(offset==chunks[chunkNum].second){
            int ret = msync(chunks[chunkNum].first+startOffset,
                            chunks[chunkNum].second-startOffset, MS_SYNC);
            RETURN_ON_ERROR(ret, "MSYNC writePages")
            startOffset=offset=0;
            chunkNum++;
        }
    }
    if(offset!=startOffset){
        int ret = msync(chunks[chunkNum].first+startOffset,
                        offset-startOffset, MS_SYNC);
        RETURN_ON_ERROR(ret, "MSYNC writePages")
    }
    for(auto it:pagesToSet){
        if(it.first<flushed){
            auto [chunk, chunkOffset] = getPtrLocation(it.first);
            memcpy(chunks[chunk].first+chunkOffset,it.second,BTREE_PAGE_SIZE);
            int ret = msync(chunks[chunk].first+chunkOffset, BTREE_PAGE_SIZE, MS_SYNC);
            RETURN_ON_ERROR(ret, "MSYNC updatePages")
        }
    }
    flushed += pagesToAppend.size();
    freeList->setMaxSeq();
    return 0;
}

int MMapChunk::updateFile() {
    int ret = writePages();
    RETURN_ON_ERROR( ret, "writePages")
    ret = updateRoot();
    RETURN_ON_ERROR(ret,"updateRoot")
    return 0;
}

int MMapChunk::updateRoot() {
    auto metadata = getMetaData();
    OUTPUT_ERROR( write(fd, metadata, strlen(DB_VERSION)+48), "write updateRoot")
    delete[] metadata;
    OUTPUT_ERROR( fsync(fd), "fsync updateRoot")
    return 0;
}

uint8_t* MMapChunk:: getMetaData(){
    auto data = new uint8_t [strlen(DB_VERSION)+48];
    uint8_t *cur = data;
    memcpy(cur, DB_VERSION, strlen(DB_VERSION));
    cur+=strlen(DB_VERSION);
    littleEndianInt64ToBytes(root, cur);
    cur+=sizeof root;
    littleEndianInt64ToBytes(flushed, cur);
    cur+=sizeof flushed;
    littleEndianInt64ToBytes(freeList->headPage, cur);
    cur+=sizeof freeList->headPage;
    littleEndianInt64ToBytes(freeList->headSeq, cur);
    cur+=sizeof freeList->headSeq;
    littleEndianInt64ToBytes(freeList->tailPage, cur);
    cur+=sizeof freeList->tailPage;
    littleEndianInt64ToBytes(freeList->tailSeq, cur);
    return data;
}

int MMapChunk::updateOrRevert(uint8_t * curMetaData) {
    int err = updateFile();
    clearPendingUpdates();
    pagesToAppend.clear();
    pagesToSet.clear();
    if(err){
        loadMeta(curMetaData);
    }
    return err;
}

void MMapChunk::loadMeta(uint8_t * data) {
    uint8_t *cur = data+strlen(DB_VERSION);
    root = littleEndianByteToInt64(cur);
    cur+=sizeof root;
    flushed = littleEndianByteToInt64(cur);
    cur+=sizeof flushed;
    freeList->headPage = littleEndianByteToInt64(cur);
    cur+=sizeof freeList->headPage;
    freeList->headSeq = littleEndianByteToInt64(cur);
    cur+=sizeof freeList->headSeq;
    freeList->tailPage = littleEndianByteToInt64(cur);
    cur+=sizeof freeList->tailPage;
    freeList->tailSeq = littleEndianByteToInt64(cur);
}

void MMapChunk::del(uint64_t ptr) {
    assert(ptr<flushed);
    pushBack(ptr);
    pagesToSet.erase(ptr);
}

uint8_t * MMapChunk::set(uint64_t ptr) {
    if(pagesToSet.count(ptr)!=0){
        return pagesToSet[ptr];
    }
    return pagesToSet[ptr] = get(ptr);
}

std::pair<uint64_t,uint64_t> MMapChunk::popHead(){
    if(freeList->headSeq==freeList->maxSeq ){
        return {0,0};
    }
    LNode node(get(freeList->headPage));
    uint64_t ptr =  node.getPtr(seq2Idx(freeList->headSeq));
    uint64_t head=0;
    freeList->headSeq++;
    if(seq2Idx(freeList->headSeq)==0){
        head = freeList->headPage;
        freeList->headPage = node.getNext();
        assert(freeList->headPage!=0);
    }
    node.resetData();
    return {ptr,head};
}

uint64_t  MMapChunk::popFront(){
    auto ret = popHead();
    if(ret.second != 0){
        pushBack(ret.second);
    }
    return ret.first;
}

void MMapChunk::pushBack(uint64_t ptr) {
    LNode node(set(freeList->tailPage));
    node.setPtr(seq2Idx(freeList->tailSeq),ptr);
    freeList->tailSeq++;
    if(seq2Idx(freeList->tailSeq)==0){
        auto ret = popHead();
        if(ret.first==0){
            auto data = new uint8_t [BTREE_PAGE_SIZE];
            memset(data,0,BTREE_PAGE_SIZE);
            ret.first = insert(data);
        }
        node.setNext(ret.first);
        freeList->tailPage = ret.first;
        if(ret.second!=0){
            LNode node2(set(ret.first));
            node2.setPtr(seq2Idx(freeList->tailSeq++), ret.second);
            node2.resetData();
        }
    }
    node.resetData();
}