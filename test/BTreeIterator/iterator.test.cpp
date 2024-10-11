

#include "../minunit.h"
#include <iostream>
#include "iterator.h"
#include "pagedb.h"
#include <filesystem>
#include "convertor.h"

int tests_run = 0;


static const char * BTreeIterator_initialization() {
    const char *filename = "test.db";
    std::filesystem::remove(filename);
    MMapChunk chunk(filename);
    BTree *btree = new DiskPageDBMemory(&chunk);
    BTreeIterator it(btree);
    mu_assert("Iterator should be invalid initially", !it.valid());
    std::vector<uint8_t> key(1);
    UpdateResult res;
    res.type = UPDATE_INSERT;
    for(int i=0;i<10;i++){
        key[0] = i;
        btree->Insert(key, key, res);
    }
    key[0] = 5;
    it.seekLE(key);
    mu_assert("Iterator should be valid after seekLE", it.valid());
    mu_assert("Key should be 5", it.key()[0] == 5);
    mu_assert("Value should be 5", it.value()[0] == 5);
    delete btree;
    return nullptr;
}

static const char * BTreeLessThan(){
    const char *filename = "test.db";
    std::filesystem::remove(filename);
    MMapChunk chunk(filename);
    BTree *btree = new DiskPageDBMemory(&chunk);
    BTreeIterator it(btree);
    std::vector<uint8_t> key(1);
    UpdateResult res;
    res.type = UPDATE_INSERT;

    for(int i=0;i<10;i++){
        if(i==5)continue;
        key[0] = i;
        btree->Insert(key, key, res);
    }
    key[0] = 5;
    it.seekLE(key);
    mu_assert("Iterator should be valid after seekLE", it.valid());
    mu_assert("Key should be 5", it.key()[0] == 4);
    delete btree;
    return nullptr;
}

static const char* BTreeIterator_next(){
    const char *filename = "test.db";
    std::filesystem::remove(filename);
    MMapChunk chunk(filename);
    BTree *btree = new DiskPageDBMemory(&chunk);
    BTreeIterator it(btree);
    std::vector<uint8_t> key(1);
    UpdateResult res;
    res.type = UPDATE_INSERT;
    for(int i=0;i<10;i++){
        key[0] = i;
        btree->Insert(key, key, res);
    }
    key[0] = 2;
    it.seekLE(key);
    int i;
    for(i=2;i<10;i++){
        mu_assert_iter(i,"Iterator should be valid after seekLE", it.valid());
        mu_assert_iter(i,"Key should be equal to inserted", it.key()[0] == i);
        it.next();
    }
    mu_assert("Iterator should be invalid after last key", !it.valid());
    delete btree;
    return nullptr;
}

static const char* BTreeIterator_prev(){
    const char *filename = "test.db";
    std::filesystem::remove(filename);
    MMapChunk chunk(filename);
    BTree *btree = new DiskPageDBMemory(&chunk);
    BTreeIterator it(btree);
    std::vector<uint8_t> key(1);
    UpdateResult res;
    res.type = UPDATE_INSERT;

    for(int i=0;i<10;i++){
        key[0] = i;
        btree->Insert(key, key, res);
    }
    key[0] = 8;
    it.seekLE(key);
    int i;
    for(i=8;i>=0;i--){
        mu_assert_iter(i,"Iterator should be valid after seekLE", it.valid());
        mu_assert_iter(i,"Key should be equal to inserted", it.key()[0] == i);
        it.prev();
    }
    mu_assert("Iterator should be invalid after last key", !it.valid());
    delete btree;
    return nullptr;
}

static const char * BTreeMultiLevelTreeTest(){
    const char *filename = "test.db";
    std::filesystem::remove(filename);
    MMapChunk chunk(filename);
    BTree *btree = new DiskPageDBMemory(&chunk);
    BTreeIterator it(btree);
    std::vector<uint8_t> key(4);
    int numKeys = 10000;
    UpdateResult res;
    res.type = UPDATE_INSERT;

    for(int i=0;i<numKeys;i++){
        bigEndianInt32ToBytes(i, key.data());
        btree->Insert(key, key, res);
    }
    bigEndianInt32ToBytes(500, key.data());
    it.seekLE(key);
    for(int i=500;i<numKeys;i++){
        mu_assert_iter(i,"Iterator should be valid after seekLE", it.valid());
        mu_assert_iter(i,"Key should be equal to inserted", bigEndianByteToInt32( it.key().data()) == i);
        it.next();
    }
    mu_assert("Iterator should be invalid after last key", !it.valid());

    delete btree;
    return nullptr;
}

static const char * all_tests(){
    mu_run_test(BTreeIterator_initialization);
    mu_run_test(BTreeLessThan);
    mu_run_test(BTreeIterator_next);
    mu_run_test(BTreeIterator_prev);
    mu_run_test(BTreeMultiLevelTreeTest);
    return nullptr;
}

int main(){

    printf("Executing test file: %s\n", __FILE_NAME__);
    const char * result = all_tests();
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != nullptr;
}