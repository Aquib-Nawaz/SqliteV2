//
// Created by Aquib Nawaz on 21/09/24.
//

#include "btree.h"
#include <cassert>

void BTree::nodeReplaceKidN( BNode* oldNode, BNode* newNode, uint16_t idx,
        std::vector<BNode*> kids){
    uint16_t inc = kids.size();
    newNode->_setHeader(BTREE_INTERIOR, oldNode->nKeys()+inc-1);
    newNode->nodeAppendRange(oldNode, 0, 0, idx);
    std::vector<uint8_t> key ,v;
    for(uint16_t i=0;i<(uint16_t)kids.size();i++){
        key = kids[i]->getKey(0);
        newNode->nodeAppendKV(idx+i,key, v, insert(kids[i]));
    }
    newNode->nodeAppendRange(oldNode, idx+inc, idx+1, oldNode->nKeys()-(idx+1));
}

void nodeSplit2(BNode* left, BNode* right, BNode* old){
    uint16_t offSize = OFFSET_ARRAY_ELEMENT_SIZE + ( BTREE_INTERIOR?
            POINTER_ARRAY_ELEMENT_SIZE:0);
    int i;
    uint16_t lenReq = HEADER_SIZE;

    for(i=old->nKeys()-1; lenReq<=BTREE_PAGE_SIZE; i--){
        lenReq += offSize + old->getOffset(i+1)-old->getOffset(i);
    }
    right->_setHeader(old->bType(), old->nBytes()-i-1);

}

std::vector<BNode*> nodeSplit3(BNode* old){
    if(old->nBytes()<=BTREE_PAGE_SIZE){
        old->shrink(1);
        return { old};
    }
    auto *newData = new uint8_t[2*BTREE_PAGE_SIZE];
    auto * left = new BNode(newData);
    newData = new uint8_t [BTREE_PAGE_SIZE];
    auto* right = new BNode(newData);
    nodeSplit2(left, right, old);
    if(left->nBytes()<=BTREE_PAGE_SIZE) {
        left->shrink(1);
        return {left, right};
    }
    newData = new uint8_t [BTREE_PAGE_SIZE];
    BNode* leftleft = new BNode(newData);
    newData = new uint8_t [BTREE_PAGE_SIZE];
    BNode* middle = new BNode(newData);
    nodeSplit2(leftleft, middle, left);
    delete left;
    assert(leftleft->nBytes()<=BTREE_PAGE_SIZE);
    return {leftleft, middle, right};
}