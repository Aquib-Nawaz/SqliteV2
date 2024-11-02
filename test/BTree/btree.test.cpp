//
// Created by Aquib Nawaz on 22/09/24.
//

#include <cstdio>
#include "btree.h"
#include "../minunit.h"
#include "hashmapkv.h"
#include "btreememory/inmemorybtreeio.h"

using namespace std;

int tests_run = 0;
BNodeFactory * factory;

BTree* createHashMemoryTree(){
    return new BTree(new InMemoryBTreeIO( factory));
}

static const char * insertAndGetTest_1(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    UpdateResult res;
    res.type = UPDATE_INSERT;
    tree->Insert(key, val, res);
    mu_assert("Insert Error", res.added&&!res.updated);
    mu_assert_iter(10, "key mismatch", tree->Get(key) == val);
    delete tree;
    return nullptr;
}

static const char * insertAndGetTest_KeyNotFound(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    UpdateResult res;
    res.type = UPDATE_INSERT;
    tree->Insert(key, val, res);
    mu_assert("Insert Error", res.added&&!res.updated);
    key[0]=1;
    mu_assert_iter(10, "key mismatch", tree->Get(key).empty());
    delete tree;
    return nullptr;
}

static const char * insertAndGetTest_2Level(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t >key(4000,0), val(3000,0);
    UpdateResult res;
    res.type = UPDATE_INSERT;
    key[0]=5, val[0]=8;
    tree->Insert(key,val, res);
    mu_assert("Insert Error", res.added&&!res.updated);
    key[0]=10, val[0]=12;
    tree->Insert(key,val, res);
    mu_assert("Insert Error", res.added&&!res.updated);
    mu_assert_iter(10, "Value Mismatch", tree->Get(key) == val);
    key[0]=5, val[0]=8;
    mu_assert_iter(5, "Value Mismatch", tree->Get(key) == val);
    delete tree;
    return nullptr;
}

static const char * insertAndGetTest_3Level(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t >key(4000,0), val(12000,0);
    UpdateResult res;
    res.type = UPDATE_INSERT;
    for(int i=1;i<=5;i++){
        key[0]=i, val[0]=i+10;
        tree->Insert(key,val, res);
        mu_assert("Insert Error", res.added&&!res.updated);
        for(int j=1;j<=i;j++){
            key[0]=j, val[0]=j+10;
            mu_assert_iter(i, "Value Mismatch", tree->Get(key) == val);
        }
    }
    delete tree;
    return nullptr;
}

static const char * deleteLevel3_Test(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t >key(1,0), val(1,0);
    UpdateResult res;
    res.type = UPDATE_INSERT;
    for(int i=1;i<=10;i++){
        key[0]=i, val[0]=i+10;
        tree->Insert(key,val, res);
    }
    for(int i=3;i<=7;i+=2) {
        key[0] = i;
        DeleteResult delResult;
        tree->Delete(key, delResult);
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

static  const char * UpdateTest(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    UpdateResult res;
    res.type = UPDATE_UPDATE;
    tree->Insert(key, val, res);
    mu_assert("Update Error", !res.updated&&!res.added);
    res.type = UPDATE_INSERT;
    tree->Insert(key, val, res);
    key[0]=1;
    res.type = UPDATE_UPDATE;
    tree->Insert(key, val, res);
    mu_assert("Update Error", !res.updated&&!res.added);
    key[0]=10, val[0]=30;
    res.type = UPDATE_UPDATE;
    tree->Insert(key, val, res);
    mu_assert("Update Error", res.updated&&!res.added);
    mu_assert_iter(val[0], "val mismatch", tree->Get(key) == val);
    val[0]=20;
    mu_assert_iter(res.oldValue[0], "Old value mismatch", res.oldValue == val);
    delete tree;
    return nullptr;
}

static  const char * UpsertTest(){
    BTree *tree = createHashMemoryTree();
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    UpdateResult res;
    res.type = UPDATE_UPSERT;
    tree->Insert(key, val, res);
    mu_assert("Update Error", !res.updated&&res.added);
    mu_assert_iter(val[0], "val mismatch", tree->Get(key) == val);
    key[0]=1;
    res.type = UPDATE_UPSERT;
    tree->Insert(key, val, res);
    mu_assert("Update Error", !res.updated&&res.added);
    mu_assert_iter(val[0], "val mismatch", tree->Get(key) == val);
    key[0]=10, val[0]=30;
    res.type = UPDATE_UPSERT;
    tree->Insert(key, val, res);
    mu_assert("Update Error", res.updated&&!res.added);
    mu_assert_iter(val[0], "val mismatch", tree->Get(key) == val);

    delete tree;
    return nullptr;
}

static const char* all_tests(){

    mu_run_test(insertAndGetTest_1);
    mu_run_test(insertAndGetTest_KeyNotFound);
    mu_run_test(insertAndGetTest_2Level);
    mu_run_test(insertAndGetTest_3Level);
    mu_run_test(deleteLevel3_Test);
    mu_run_test(UpdateTest);
    mu_run_test(UpsertTest);
    return nullptr;
}

int main(){
    factory = new BNodeFactory();
    printf("Executing test file: %s\n", __FILE_NAME__);
    const char * result = all_tests();
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    delete factory;
    return result != nullptr;
}