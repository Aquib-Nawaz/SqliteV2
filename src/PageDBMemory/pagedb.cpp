//
// Created by Aquib Nawaz on 27/09/24.
//

#include <fcntl.h>
#include "pagedb.h"
#include <filesystem>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "convertor.h"
#include <cassert>

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

void DiskPageDBMemory::extendMMap(long long size) {
    if(size <= mmapChunk.size)
        return;
    long long alloc = std::max(mmapChunk.size, 64ll<<20);
    while (mmapChunk.size+alloc<size)alloc<<=1;
    OUTPUT_ERROR(ftruncate(fd,mmapChunk.size+alloc), "ftruncate extendMMap")
    auto map = (uint8_t*)mmap(nullptr, alloc,PROT_READ|PROT_WRITE, MAP_SHARED, fd, mmapChunk.size);
    mmapChunk.chunks.emplace_back(map, alloc);
    mmapChunk.size+=alloc;
}

DiskPageDBMemory::DiskPageDBMemory(const char *_path) {
    fd = createFileSync(_path);
    struct stat st;
    fstat(fd, &st);
    bool empty = st.st_size==0;
    size_t versionSize = strlen(DB_VERSION);
    if(empty){
        mmapChunk.size = 0;
        flushed = 1;
        root=0;
    }
    else{
        uint8_t sizeData[8];
        pread(fd, sizeData, 8, versionSize+sizeof root);
        mmapChunk.size = littleEndianByteToInt64(sizeData)*BTREE_PAGE_SIZE;
        auto map =(uint8_t *) mmap(nullptr, mmapChunk.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(map==MAP_FAILED){
            perror("Unmap: ");
            exit(1);
        }
        mmapChunk.chunks.emplace_back(map, st.st_size);

        flushed = littleEndianByteToInt64(map + versionSize+sizeof root);
        root = littleEndianByteToInt64(map + versionSize);
        if(memcmp(DB_VERSION, map, versionSize) != 0){
            perror("Version Mismatch");
            exit(1);
        }
    }
}

uint64_t DiskPageDBMemory::insert(BNode * node) {
    pagesToAppend.push_back(node->getData());
    node->resetData();
    delete node;
    return flushed+pagesToAppend.size()-1;
}

BNode *DiskPageDBMemory::get(uint64_t ptr) {
    size_t chunkNum ;
    uint64_t cur=0;
    for(chunkNum=0;chunkNum<mmapChunk.chunks.size()&&cur+
    mmapChunk.chunks[chunkNum].second/BTREE_PAGE_SIZE<=ptr;chunkNum++)
        cur+=mmapChunk.chunks[chunkNum].second/BTREE_PAGE_SIZE;
    assert(chunkNum<mmapChunk.chunks.size());
    auto *data = new uint8_t [BTREE_PAGE_SIZE];
    memcpy(data ,mmapChunk.chunks[chunkNum].first+(ptr-cur)*BTREE_PAGE_SIZE,BTREE_PAGE_SIZE);
    return new BNode(data);
}
void DiskPageDBMemory::del(uint64_t ptr) {
    assert(ptr>0 && (long long)ptr < mmapChunk.size/BTREE_PAGE_SIZE);
}

DiskPageDBMemory::~DiskPageDBMemory() {
    for(auto & i : pagesToAppend){
        delete[] i;
    }
    for(auto & i : mmapChunk.chunks){
        OUTPUT_ERROR(munmap(i.first, i.second), "destructor unmap")
    }
    OUTPUT_ERROR( close(fd),"destructor file close")
}

int DiskPageDBMemory::writePages() {
    long long size = (long long)(flushed+pagesToAppend.size())*BTREE_PAGE_SIZE;
    extendMMap(size);
    long long offset = (long long)flushed*BTREE_PAGE_SIZE;
    size_t chunkNum = 0;
    while (chunkNum<mmapChunk.chunks.size() &&
        offset>=mmapChunk.chunks[chunkNum].second)
    {
        offset-=mmapChunk.chunks[chunkNum++].second;
    }
    long long startOffset = offset;
    for(auto page:pagesToAppend){
        assert(chunkNum<mmapChunk.chunks.size());
        memcpy(mmapChunk.chunks[chunkNum].first+offset,page,BTREE_PAGE_SIZE);
        offset+=BTREE_PAGE_SIZE;
        if(offset==mmapChunk.chunks[chunkNum].second){
            int ret = msync(mmapChunk.chunks[chunkNum].first+startOffset,
                  mmapChunk.chunks[chunkNum].second-startOffset, MS_SYNC);
            RETURN_ON_ERROR(ret, "MSYNC writePages")
            startOffset=offset=0;
            chunkNum++;
        }
    }
    if(offset!=startOffset){
        int ret = msync(mmapChunk.chunks[chunkNum].first+startOffset,
              offset-startOffset, MS_SYNC);
        RETURN_ON_ERROR(ret, "MSYNC writePages")
    }
    return 0;
}

int DiskPageDBMemory::updateFile() {
    int ret = writePages();
    RETURN_ON_ERROR( ret, "writePages")
    ret = updateRoot();
    RETURN_ON_ERROR(ret,"updateRoot")
    return 0;
}

int DiskPageDBMemory::updateRoot() {
    auto metadata = new uint8_t[strlen(DB_VERSION)+sizeof root+sizeof flushed];
    getMetaData(metadata);
    OUTPUT_ERROR( write(fd, metadata, strlen(DB_VERSION)+sizeof root+sizeof flushed), "write updateRoot")
    delete[] metadata;
    OUTPUT_ERROR( fsync(fd), "fsync updateRoot")
    return 0;
}

void DiskPageDBMemory:: getMetaData(uint8_t* data){
    memcpy(data, DB_VERSION, strlen(DB_VERSION));
    littleEndianInt64ToBytes(root, data+ strlen(DB_VERSION));
    littleEndianInt64ToBytes(flushed, data+ strlen(DB_VERSION)+sizeof root);
}

int DiskPageDBMemory::updateOrRevert(uint8_t *data) {
    int err = updateFile();
    for(auto page:pagesToAppend)
        delete []page;
    pagesToAppend.clear();
    if(err){
        loadMeta(data);
    }
    return err;
}

void DiskPageDBMemory::loadMeta(uint8_t * data) {
    root = littleEndianByteToInt64(data+strlen(DB_VERSION));
    flushed = littleEndianByteToInt64(data+strlen(DB_VERSION)+sizeof root);
}

void DiskPageDBMemory::Insert(std::vector<uint8_t> & key, std::vector<uint8_t> & val) {
    auto meta = new uint8_t[strlen(DB_VERSION)+sizeof root+sizeof flushed];
    getMetaData(meta);
    BTree::Insert(key, val);
    updateOrRevert(meta);
    delete []meta;
}
