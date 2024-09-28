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

DiskPageDBMemory::DiskPageDBMemory(const char *_path) {
    fd = createFileSync(_path);
    struct stat st;
    fstat(fd, &st);
    bool empty = st.st_size==0;
    if(empty){
        ftruncate(fd, 4096);
        st.st_size = 4096;
    }
    auto map =(uint8_t *) mmap(nullptr, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(map==MAP_FAILED){
        perror("Unmap: ");
        exit(1);
    }
    if(empty){
        memcpy(map, DB_VERSION, 10);
        littleEndianInt64ToBytes(0, map+10);
        littleEndianInt64ToBytes(1, map+18);
        msync(map, 26, MS_SYNC);
        mmapChunk.size = PAGE_SIZE;
    }
    else{
        root = littleEndianByteToInt64(map+10);
        mmapChunk.size = littleEndianByteToInt64(map+18);
    }
    if(memcmp(DB_VERSION, map, 10) != 0){
        perror("Version Mismatch");
        exit(1);
    }
    mmapChunk.chunks.emplace_back(map, st.st_size);
}

uint64_t DiskPageDBMemory::insert(BNode *) {return 0;}
BNode *DiskPageDBMemory::get(uint64_t) {return nullptr;}
void DiskPageDBMemory::del(uint64_t) {}

DiskPageDBMemory::~DiskPageDBMemory() {
    OUTPUT_ERROR( munmap(mmapChunk.chunks[0].first, mmapChunk.chunks[0].second), "destructor");
    OUTPUT_ERROR( close(fd),"destructor file close");
}
