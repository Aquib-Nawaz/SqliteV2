//
// Created by Aquib Nawaz on 22/09/24.
//

#include <iostream>
#include "../minunit.h"
#include "mmap.h"
#include <cstdio>
#include <fstream>
#include <string>
using namespace std;

int tests_run = 0;
class MMapTest {
    static const char *PageDBInititalizationTest() {
        const char *fileName = "test.db";
        std::remove(fileName);
        MMapChunk mmap = MMapChunk(fileName);
        return nullptr;
    }
    static const char *PageDBInsertTest() {
        auto data = new uint8_t[16384];
        for (uint8_t i = 0; i < 0xFF; i++) {
            data[i] = i;
        }
        const char* fileName = "test.db";
        remove(fileName);
        MMapChunk db(fileName);
        uint64_t ptr = db.insert(data);
        mu_assert("Page ptr match", ptr==2);
        db.setRoot(0);
        mu_assert("File Updated error", db.updateFile()==0);
        data = db.get(ptr);
        for(uint8_t i=0;i<0xFF;i++){
            mu_assert("Key Match", data[i]==i);
        }
        delete[] data;
        return nullptr;
    }

    static const char* PageDBPersistenceTest(){
        const char* fileName = "test.db";

        MMapChunk db(fileName);
        auto data = db.get(2);
        for(uint8_t i=0;i<0xFF;i++){
            mu_assert_iter(i,"Key Match", data[i]==i);
        }
        delete data;
        std::remove(fileName);
        return nullptr;
    }

    static const char* LNodeTest(){
        LNode node = LNode(new uint8_t[16384]);
        for(uint8_t i=0;i<0xFF;i++){
            node.setPtr(i,i*10);
        }
        uint64_t next=100;
        node.setNext(next);
        for(uint8_t i=0;i<0xFF;i++){
            mu_assert_iter(i, "Ptr Match", node.getPtr(i)==i*10);
        }
        mu_assert("Next Match", node.getNext()==next);
        return nullptr;
    }

    static const char * PushBackTest(){
        const char * filePath = "test.db";
        std::remove(filePath);
        MMapChunk mmap = MMapChunk(filePath);
        mmap.writePages();
        mmap.flushed = 12;
        for(int i=0;i<10;i++){
            mmap.pushBack(i+2);
            mu_assert_iter(i, "Tail Seq Match", mmap.freeList->tailSeq==i+1);
        }
        mmap.updateFile();
        MMapChunk mmap2(filePath);
        for(int i=0;i<10;i++){
            mu_assert_iter(i, "Pages Match" , mmap2.popFront()==i+2);
        }
        return nullptr;
    }

    static const char* AllocaTeNewPageForFreeList(){
        const char * filePath = "test.db";
        std::remove(filePath);
        MMapChunk mmap = MMapChunk(filePath);
        auto data = new uint8_t [16384];
        uint64_t ptr = mmap.insert( data );
        mmap.updateFile();
        mmap.del(ptr);
        mmap.freeList->maxSeq=1;
        mmap.flushed = FREE_LIST_CAPACITY+2;
        for(int i=1;i<FREE_LIST_CAPACITY;i++){
            mu_assert_iter(i,"Tail Page", mmap.freeList->tailPage==1);
            mmap.pushBack(i+2);
        }
        mu_assert("Tail Page",
                  mmap.freeList->tailPage == ptr);
        return nullptr;
    }

    static const char * InsertInFreedPageTest(){
        const char * filePath = "test.db";
        std::remove(filePath);
        MMapChunk mmap = MMapChunk(filePath);
        auto data = new uint8_t [16384];
        uint64_t ptr = mmap.insert( data );
        mmap.updateFile();
        mmap.del(ptr);
        mmap.freeList->maxSeq=1;
        data = new uint8_t [16384];
        mu_assert( "Insert in freed page",mmap.insert(data)==ptr);
        return nullptr;
    }


    public:
    static const char *all_tests() {
        mu_run_test(PageDBInititalizationTest);
        mu_run_test(PageDBInsertTest);
        mu_run_test(PageDBPersistenceTest);
        mu_run_test(LNodeTest);
        mu_run_test(PushBackTest);
        mu_run_test(AllocaTeNewPageForFreeList);
        mu_run_test(InsertInFreedPageTest);
        return nullptr;
    }
};

int main(){
    printf("Executing test file: %s\n", __FILE_NAME__);
    const char * result = MMapTest::all_tests();
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != nullptr;
}