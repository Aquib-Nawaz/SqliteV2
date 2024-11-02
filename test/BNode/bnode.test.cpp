//
// Created by Aquib Nawaz on 22/09/24.
//

#include <cstdio>
#include "bnode.h"
#include <string>
#include "../minunit.h"
using namespace std;

int tests_run = 0;

const static char* setHeaderTest(){
    BNode bnode(1);
    bnode._setHeader(BTREE_INTERIOR,23);
    mu_assert("bnode.setHeader failed", bnode.bType() == BTREE_INTERIOR
    && bnode.nKeys() == 23);
    return nullptr;
}


static const char* setOffsetTest(){
    BNode bnode(1);
    bnode._setHeader(BTREE_INTERIOR,1);
    bnode.setOffset(1, 24);
    mu_assert("interior bnode.setOffset does not match bnode.getOffset", bnode.getOffset(1) == 24);
    BNode leaf(1);
    leaf._setHeader(BTREE_LEAF,3);
    leaf.setOffset(2,25);
    leaf.setOffset(1,33);
    mu_assert("leaf leaf.setOffset does not match leaf.getOffset", leaf.getOffset(2) == 25 && leaf.getOffset(1)==33);
    return nullptr;
}

static const char * setPtrTest(){
    BNode bnode(1);
    bnode._setHeader(BTREE_INTERIOR, 3);
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
    BNode bnode(1);
    bnode._setHeader(BTREE_LEAF, 3);
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
    BNode bnode(1);
    bnode._setHeader(BTREE_LEAF, 10);
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
    BNode bnode(1);
    bnode._setHeader(BTREE_INTERIOR, 9);
    std::vector<uint8_t >data(1),v;
    for(uint8_t i=0;i<5;i++){
        data[0] = i;
        bnode.nodeAppendKV(i, data, v);
    }

    for(uint8_t i=6;i<10;i++){
        data[0] = i;
        bnode.nodeAppendKV(i-1, data, v);
    }
    BNode newNode(1);
    data[0]=5;
    UpdateResult res;
    res.type = UPDATE_INSERT;
    newNode.leafInsert(&bnode, 4, data, v ,res);
    mu_assert("Size Match", newNode.nKeys() == 10);
    mu_assert("Key Inserted", res.added&&!res.updated);
    for(int i=0;i<10;i++){
        data[0] = i;
        mu_assert("key mismatch", newNode.getKey(i) == data);
    }
    return nullptr;
}

static const char * leafDeleteTest(){
    BNode bnode(1);
    bnode._setHeader(BTREE_INTERIOR, 9);
    std::vector<uint8_t >data(1),v;
    for(uint8_t i=0;i<9;i++){
        data[0] = i;
        bnode.nodeAppendKV(i, data, v);
    }
    BNode newNode(1);
    DeleteResult delResult;
    newNode.leafDelete(&bnode, 5, delResult);
    mu_assert("Size Match", newNode.nKeys() == 8);
    for(int i=0;i<5;i++){
        data[0] = i;
        mu_assert("Key Mismatch", newNode.getKey(i) == data);
    }
    for(int i=6;i<9;i++){
        data[0] = i;
        mu_assert("Key Mismatch", newNode.getKey(i-1) == data);
    }
    return nullptr;
}

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

    auto splits = BNode::nodeSplit3(node);
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
    auto splits = BNode::nodeSplit3(node);
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


static const char* all_tests(){
    mu_run_test(setHeaderTest);
    mu_run_test(setOffsetTest);
    mu_run_test(setPtrTest);
    mu_run_test(nodeAppendKVLeafTest);
    mu_run_test(nodeLookUpLETest);
    mu_run_test(leafInsertTest);
    mu_run_test(leafDeleteTest);
    mu_run_test(nodeSplit3Test_1);
    mu_run_test(nodeSplit3Test_2);
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