//
// Created by Aquib Nawaz on 22/09/24.
//

#include <iostream>
#include "../minunit.h"
#include "pagedb.h"
#include <fcntl.h>
#include <cstdio>
#include "convertor.h"
using namespace std;

int tests_run = 0;
class DiskPageDBMemoryTest {
    static const char *PageDBInititalizationTest() {
        const char *fileName = "test.db";
        std::remove(fileName);
        DiskPageDBMemory db(fileName);
        return nullptr;
    }
    static const char *PageDBInsertTest() {
        auto node = new BNode(1);
        node->_setHeader(BTREE_LEAF,10);
        std::vector<uint8_t>key(1), val(1);
        for(int i=0;i<10;i++){
            key[0]=i;
            val[0]=i+10;
            node->nodeAppendKV(i, key, val);
        }
        const char* fileName = "test.db";
        remove(fileName);
        DiskPageDBMemory db(fileName);
        uint64_t ptr = db.insert(node);
        mu_assert("Page ptr match", ptr==1);
        mu_assert("File Updated error", db.updateFile()==0);
        BNode* newNode = db.get(ptr);
        mu_assert("Size Match", newNode->nKeys()==10);
        for(int i=0;i<10;i++){
            mu_assert("Key Match", newNode->getKey(i)[0]==i);
            mu_assert("Val Match", newNode->getVal(i)[0]==i+10);
        }
        delete newNode;
        return nullptr;
    }
    static const char* PageDBPersistenceTest(){
        const char* fileName = "test.db";
        int keysize=10;
        DiskPageDBMemory db = DiskPageDBMemory(fileName);
        BNode* newNode = db.get(1);
        mu_assert("Size Match", newNode->nKeys()==keysize);
        for(int i=0;i<keysize;i++){
            mu_assert("Key Match", newNode->getKey(i)[0]==i);
            mu_assert("Val Match", newNode->getVal(i)[0]==i+10);
        }
        delete newNode;
        return nullptr;
    }

    static const char * PageInsertMultiLevelTest(){
        const char* fileName = "test.db";
        BTree *tree = new DiskPageDBMemory(fileName);
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

    public:
    static const char *all_tests() {
        mu_run_test(PageDBInititalizationTest);
        mu_run_test(PageDBInsertTest);
        mu_run_test(PageDBPersistenceTest);
        mu_run_test(PageInsertMultiLevelTest);
        return nullptr;
    }
};

int main(){
    printf("Executing test file: %s\n", __FILE_NAME__);
    const char * result = DiskPageDBMemoryTest::all_tests();
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != nullptr;
}