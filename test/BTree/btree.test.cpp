//
// Created by Aquib Nawaz on 22/09/24.
//

#include <cstdio>
#include "btree.h"
#include "../minunit.h"
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
    std::vector<uint8_t>key(1), val(100);
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
    BTree tree;
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    tree.Insert(key, val);
    mu_assert_iter(10, "key mismatch", tree.Get(key) == val);
    return nullptr;
}

static const char * insertAndGetTest_KeyNotFound(){
    BTree tree;
    std::vector<uint8_t>key(1), val(1);
    key[0]=10,val[0]=20;
    tree.Insert(key, val);
    key[0]=1;
    mu_assert_iter(10, "key mismatch", tree.Get(key).empty());
    return nullptr;
}

static const char * insertAndGetTest_2Level(){
    BTree tree;
    std::vector<uint8_t >key(1000,0), val(3000,0);
    key[0]=5, val[0]=8;
    tree.Insert(key,val);
    key[0]=10, val[0]=12;
    tree.Insert(key,val);
    key[0]=10, val[0]=12;
    mu_assert_iter(5, "Value Mismatch", tree.Get(key) == val);
    return nullptr;
}

static const char* all_tests(){
    mu_run_test(nodeSplit3Test_1);
    mu_run_test(nodeSplit3Test_2);
    mu_run_test(insertAndGetTest_1);
    mu_run_test(insertAndGetTest_KeyNotFound);
    mu_run_test(insertAndGetTest_2Level);
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