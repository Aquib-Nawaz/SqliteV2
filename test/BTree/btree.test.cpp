//
// Created by Aquib Nawaz on 22/09/24.
//

#include <cstdio>
#include "btree.h"
#include "../minunit.h"
#include "hashmapdb.h"
#include "mmap.h"
#include "pagedb.h"
#include <fstream>
#include <filesystem>

using namespace std;

int tests_run = 0;

static const char * nodeSplit3Test_1(){
    auto node = new BNode(1);
    uint16_t elemSize=20;
    node->_setHeader(BTREE_LEAF, elemSize);
    std::vector<uint8_t>key(1), val(1);
    for(int i=0;i<elemSize;i++){
        key[0]=i;
        val[0]=i+10;
        node->nodeAppendKV(i, key, val);
    }

    auto splits = nodeSplit3(node);
    mu_assert("splits.size() == 1", splits.size() == 1);
    for(int i=0;i<elemSize;i++){
        key[0]=i, val[0]=i+10;
        mu_assert_iter(i, "key mismatch", splits[0]->getKey(i) == key);
        mu_assert_iter(i, "val mismatch", splits[0]->getVal(i) == val);
    }
    delete splits[0];
    return nullptr;
}


static const char * nodeSplit3Test_2(){
    uint16_t elemSize=50;
    std::vector<uint8_t>key(1), val(400);
    auto node = new BNode(2);
    node->_setHeader(BTREE_LEAF, elemSize);
    for(int i=0;i<elemSize;i++){
        key[0]=i;
        val[0]=i+10;
        node->nodeAppendKV(i, key, val);
    }
    mu_assert("Page Size > MAXPAGESIZE", node->nBytes() > BTREE_PAGE_SIZE);
    auto splits = nodeSplit3(node);
    mu_assert("splits.size() == 2", splits.size() == 2);
    mu_assert("Element Size sum", elemSize == splits[0]->nKeys() + splits[1]->nKeys());
    uint16_t firstNodeSize = splits[0]->nKeys();
    for(int i=0;i<elemSize;i++){
        key[0]=i;
        mu_assert_iter(i,"key mismatch", splits[i>=firstNodeSize]->getKey(i>=firstNodeSize?i-firstNodeSize:i) == key);
        mu_assert_iter(i,"val mismatch", splits[i>=firstNodeSize]->getVal(i>=firstNodeSize?i-firstNodeSize:i)[0] == i+10);
    }
    delete splits[0];
    delete splits[1];
    return nullptr;
}

static const char * insertAndGetTest_1(){
    BTree *tree = new HashMapDBMemory();
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    tree->Insert(key, val);
    mu_assert_iter(10, "key mismatch", tree->Get(key) == val);
    delete tree;
    return nullptr;
}

static const char * insertAndGetTest_KeyNotFound(){
    BTree *tree = new HashMapDBMemory();
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    tree->Insert(key, val);
    key[0]=1;
    mu_assert_iter(10, "key mismatch", tree->Get(key).empty());
    delete tree;
    return nullptr;
}

static const char * insertAndGetTest_2Level(){
    BTree *tree = new HashMapDBMemory();
    std::vector<uint8_t >key(4000,0), val(3000,0);
    key[0]=5, val[0]=8;
    tree->Insert(key,val);
    key[0]=10, val[0]=12;
    tree->Insert(key,val);
    mu_assert_iter(10, "Value Mismatch", tree->Get(key) == val);
    key[0]=5, val[0]=8;
    mu_assert_iter(5, "Value Mismatch", tree->Get(key) == val);
    delete tree;
    return nullptr;
}

static const char * insertAndGetTest_3Level(){
    BTree *tree = new HashMapDBMemory();
    std::vector<uint8_t >key(4000,0), val(12000,0);
    for(int i=1;i<=5;i++){
        key[0]=i, val[0]=i+10;
        tree->Insert(key,val);
        for(int j=1;j<=i;j++){
            key[0]=j, val[0]=j+10;
            mu_assert_iter(i, "Value Mismatch", tree->Get(key) == val);
        }
    }
    delete tree;
    return nullptr;
}

static const char * deleteLevel3_Test(){
    BTree *tree = new HashMapDBMemory();
    std::vector<uint8_t >key(1,0), val(1,0);
    for(int i=1;i<=10;i++){
        key[0]=i, val[0]=i+10;
        tree->Insert(key,val);
    }
    for(int i=3;i<=7;i+=2) {
        key[0] = i;
        tree->Delete(key);
        mu_assert_iter(i, "Value Mismatch", tree->Get(key).empty());
    }
    for(int i=1;i<=10;i++){
        key[0]=i, val[0]=i+10;
        if(i==3 || i==5 || i==7)
            continue;
        mu_assert_iter(i, "Value Mismatch", tree->Get(key) == val);
    }
    delete tree;

    return nullptr;
}


static const char * EndToEndInsertTest(){
    const char * filePath = "test.db";
    std::remove(filePath);
    MMapChunk mmap = MMapChunk(filePath);
    std::vector<uint8_t>key,val;
    const char* csvFile = "../../../data/wine-quality.csv";
    std::filesystem::path path(csvFile);
    mu_assert("File Exist", std::filesystem::exists(path));
    std::ifstream csv(csvFile);
    std::string line;
    std::string keyPref = "WineQuality-";
    int idx=0;
    BTree *tree = new DiskPageDBMemory(&mmap);

    while(std::getline(csv,line)) {
        val = std::vector<uint8_t >(line.begin(), line.end());
        std::string temp = keyPref + std::to_string(idx++);
        key = std::vector<uint8_t>(temp.begin(), temp.end());
        tree->Insert(key, val);
        mu_assert_iter(idx, "Value Mismatch", tree->Get(key) == val);
    }
    delete tree;
    return nullptr;
}

static const char * EndToEndPersistenceTest(){
    const char * filePath = "test.db";
    MMapChunk mmap = MMapChunk(filePath);
    std::vector<uint8_t>key,val;
    const char* csvFile = "../../../data/wine-quality.csv";
    std::filesystem::path path(csvFile);
    mu_assert("File Exist", std::filesystem::exists(path));
    std::ifstream csv(csvFile);
    std::string line;
    std::string keyPref = "WineQuality-";
    int idx=0;
    BTree *tree = new DiskPageDBMemory(&mmap);
    while(std::getline(csv,line)) {
        val = std::vector<uint8_t >(line.begin(), line.end());
        std::string temp = keyPref + std::to_string(idx++);
        key = std::vector<uint8_t>(temp.begin(), temp.end());
        mu_assert_iter(idx, "Value Mismatch", tree->Get(key) == val);
    }
    delete tree;
    std::remove(filePath);
    return nullptr;
}

static const char* all_tests(){
    mu_run_test(nodeSplit3Test_1);
    mu_run_test(nodeSplit3Test_2);
    mu_run_test(insertAndGetTest_1);
    mu_run_test(insertAndGetTest_KeyNotFound);
    mu_run_test(insertAndGetTest_2Level);
    mu_run_test(insertAndGetTest_3Level);
    mu_run_test(deleteLevel3_Test);
    mu_run_test(EndToEndInsertTest);
    mu_run_test(EndToEndPersistenceTest);
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