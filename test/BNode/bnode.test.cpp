//
// Created by Aquib Nawaz on 22/09/24.
//

#include <cstdio>
#include "bnode.h"
#include "../minunit.h"
using namespace std;

int tests_run = 0;

const static char* setHeaderTest(){
    BNode bnode(BTREE_INTERIOR,23);
    mu_assert("bnode.setHeader failed", bnode.bType() == BTREE_INTERIOR
    && bnode.nKeys() == 23);
    return nullptr;
}


static const char* setOffsetTest(){
    BNode bnode(BTREE_INTERIOR,1);
    bnode.setOffset(1, 24);
    mu_assert("interior bnode.setOffset does not match bnode.getOffset", bnode.getOffset(1) == 24);
    BNode leaf(BTREE_LEAF,3);
    leaf.setOffset(2,25);
    leaf.setOffset(1,33);
    mu_assert("leaf leaf.setOffset does not match leaf.getOffset", leaf.getOffset(2) == 25 && leaf.getOffset(1)==33);
    return nullptr;
}

static const char * setPtrTest(){
    BNode bnode(BTREE_INTERIOR, 3);
    uint64_t ptr[3] = {0x1213141112131412, 0x21312312431, 0x213123};
    for(int i=0;i<3;i++)bnode.setPtr(i, ptr[i]);
    bool res= true;
    for(int i=0;i<3;i++){
        res&=bnode.getPtr(i)==ptr[i];
    }
    mu_assert("interior bnode.setPtr does not match bnode.getPtr", res);
    return nullptr;
}

static const char* nodeAppendKVLeafTest(){
    BNode bnode(BTREE_LEAF, 3);
    std::vector<uint8_t >data[3][2] = {
            {{1,2,3}, {4,5,6}},
            {{7,8}, {10,11,12}},
            {{13,14,15}, {16,17}}
    };
    for(int i=0;i<3;i++){
        bnode.nodeAppendKV(i, data[i][0], data[i][1]);
        mu_assert("key mismatch", bnode.getKey(i) == data[i][0]);
        mu_assert("key mismatch", bnode.getVal(i) == data[i][1]);
    }
    bool res= true;
    for(int i=0;i<3;i++){
        res&=bnode.getKey(i)==data[i][0];
        res&=bnode.getVal(i)==data[i][1];
    }
    mu_assert("leaf bnode.nodeAppendKV does not match bnode.getKey and bnode.getVal", res);
    return nullptr;
}

static const char *nodeLookUpLETest(){
    BNode bnode(BTREE_LEAF, 10);
    std::vector<uint8_t >data(1);
    for(uint8_t i=0;i<10;i++){
        data[0] = i;
        bnode.nodeAppendKV(i, data, data);
    }

    for(uint8_t i=0;i<10;i++){
        data[0]=i;
        mu_assert("idx to insert mismatch", bnode.nodeLookUpLE(data) == i);
    }
    return nullptr;
}

static const char* leafInsertTest() {
    BNode bnode(BTREE_INTERIOR, 9);
    std::vector<uint8_t >data(1),v;
    for(uint8_t i=0;i<5;i++){
        data[0] = i;
        bnode.nodeAppendKV(i, data, v);
    }

    for(uint8_t i=6;i<10;i++){
        data[0] = i;
        bnode.nodeAppendKV(i-1, data, v);
    }
    BNode newNode(BTREE_INTERIOR, 10);
    data[0]=5;
    newNode.leafInsert(&bnode, 5, data, v);
    mu_assert("Size Match", newNode.nKeys() == 10);
    for(int i=0;i<10;i++){
        data[0] = i;
        mu_assert("key mismatch", newNode.getKey(i) == data);
    }
    return nullptr;
}
static const char* all_tests(){
    mu_run_test(setHeaderTest);
    mu_run_test(setOffsetTest);
    mu_run_test(setPtrTest);
    mu_run_test(nodeAppendKVLeafTest);
    mu_run_test(nodeLookUpLETest);
    mu_run_test(leafInsertTest);
    return nullptr;
}

int main(){

    const char * result = all_tests();
    printf("Executing test file: %s\n", __FILE_NAME__);
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != nullptr;
}