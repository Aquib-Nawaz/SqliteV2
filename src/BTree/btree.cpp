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
//        delete kids[i];
    }

    newNode->nodeAppendRange(oldNode, idx+inc, idx+1, oldNode->nKeys()-(idx+1));
}

BNode* BTree::treeInsert(BNode* oldNode, std::vector<uint8_t>& key, std::vector<uint8_t>& val){
    auto *data = new uint8_t [2*BTREE_PAGE_SIZE];
    auto* newNode = new BNode(data);
    uint16_t idx = oldNode->nodeLookUpLE(key);

    switch(oldNode->bType()){
        case BTREE_LEAF:
            newNode->leafInsert(oldNode, idx, key, val);
            break;
        case BTREE_INTERIOR:
            nodeInsert(oldNode, newNode, idx, key, val);
            break;
        default:
            assert(false);
    }
    delete oldNode;
    return newNode;
}

void BTree::nodeInsert(BNode* old,BNode* newNode,uint16_t idx,
                         std::vector<uint8_t >& key, std::vector<uint8_t>& value){
    uint64_t kptr = old->getPtr(idx);
    auto prevChild = get(kptr);
    auto knode = treeInsert( prevChild, key, value);
    auto splits = nodeSplit3(knode);
    delete prevChild;
    del(kptr);
    nodeReplaceKidN(old, newNode, idx, splits);
}

BNode* BTree::nodeDelete(BNode* old,uint16_t idx,std::vector<uint8_t >&key){
    uint64_t kptr = old->getPtr(idx);
    auto prevChild = get(kptr);
    auto updated = treeDelete(prevChild, key);
    delete prevChild;
    if(updated == nullptr){
        return nullptr;
    }
    del(kptr);
    auto data = new uint8_t [BTREE_PAGE_SIZE];
    auto [mergeDir, sibling] = shouldMerge(old, idx, updated);
    switch (mergeDir) {
        case -1:

    }
}

void BTree::Insert(std::vector<uint8_t> &key, std::vector<uint8_t> & val) {
    checkLimit(key, val);
    if (root == 0){
        auto *data = new uint8_t [BTREE_PAGE_SIZE];
        auto rootNode = new BNode(data);
        rootNode->_setHeader(BTREE_LEAF, 2);
        std::vector<uint8_t>k;
        rootNode->nodeAppendKV(0,k,k);
        rootNode->nodeAppendKV(1, key, val);
        root = insert(rootNode);
        return;
    }
    auto rootNode = get(root);
    auto node = treeInsert(rootNode, key, val);
    std::vector<BNode*> splits = nodeSplit3(node);
    del(root);
    if(splits.size()>1){
        auto *data = new uint8_t [BTREE_PAGE_SIZE];
        rootNode = new BNode(data);
        rootNode->_setHeader(BTREE_INTERIOR, splits.size());
        std::vector<uint8_t> k,v;
        for(uint16_t i=0;i<(uint16_t)splits.size();i++){
            k = splits[i]->getKey(0);
            rootNode->nodeAppendKV(i, k, v, insert(splits[i]));
//            delete splits[i];
        }
        root = insert(rootNode);
    }
    else {
        root = insert(splits[0]);
    }
}

void nodeSplit2(BNode* left, BNode* right, BNode* old){
    uint16_t offSize = OFFSET_ARRAY_ELEMENT_SIZE + ( old->bType()==BTREE_INTERIOR?
            POINTER_ARRAY_ELEMENT_SIZE:0);
    int i;
    uint16_t lenReq = HEADER_SIZE;

    for(i=old->nKeys()-1; lenReq<=BTREE_PAGE_SIZE; i--){
        lenReq += offSize + old->getOffset(i+1)-old->getOffset(i);
    }
    right->_setHeader(old->bType(), old->nBytes()-i-1);
    right->nodeAppendRange(old, 0,i+1, old->nKeys()-i-1);

    left->_setHeader(old->bType(), i+1);
    left->nodeAppendRange(old, 0, 0, i+1);
    delete old;
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
    auto* leftleft = new BNode(newData);

    newData = new uint8_t [BTREE_PAGE_SIZE];
    auto* middle = new BNode(newData);

    nodeSplit2(leftleft, middle, left);

    assert(leftleft->nBytes()<=BTREE_PAGE_SIZE);
    return {leftleft, middle, right};
}

void checkLimit(std::vector<uint8_t >&key, std::vector<uint8_t >&val){
    assert(key.size()<=BTREE_MAX_KEY_SIZE && val.size()<=BTREE_MAX_VAL_SIZE);
}

std::pair<int, BNode*> BTree::shouldMerge(BNode* par, uint16_t idx, BNode* updated){
    if(updated->nBytes()>BTREE_PAGE_SIZE/4){
        return {0, nullptr};
    }
    if(idx>0){
        //Check left
        auto left = get(par->getPtr(idx-1));
        uint16_t merged_size = left->nBytes() + updated->nBytes()-HEADER_SIZE;
        if(merged_size<=BTREE_PAGE_SIZE){
            return {-1,left};
        }
    }
    if(idx<par->nKeys()-1){
        //Check right
        auto right = get(par->getPtr(idx+1));
        uint16_t merged_size = updated->nBytes() + right->nBytes()-HEADER_SIZE;
        if(merged_size<=BTREE_PAGE_SIZE){
            return {1,right};
        }
    }
    return {0, nullptr};
}

void nodeMerge(BNode* merged, BNode* left, BNode* right){
    assert(left->bType()==right->bType());
    merged->_setHeader(left->bType(), left->nKeys()+right->nKeys());
    merged->nodeAppendRange(left, 0, 0, left->nKeys());
    merged->nodeAppendRange(right, 0, 0, right->nKeys());
    delete left;
    delete right;
}

