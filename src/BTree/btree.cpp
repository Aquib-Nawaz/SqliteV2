//
// Created by Aquib Nawaz on 21/09/24.
//

#include "btree.h"
#include <cassert>

BTree::~BTree()= default;
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
BNode* BTree::treeDelete(BNode *oldNode, std::vector<uint8_t> & key) {
    auto * newNode = new BNode(1);
    uint16_t idx = oldNode->nodeLookUpLE(key);
    switch (oldNode->bType()) {
        case BTREE_LEAF:
            if(key!=oldNode->getKey(idx)){
                delete oldNode;
                delete newNode;
                return nullptr;
            }
            newNode->leafDelete(oldNode, idx);
            break;
        case BTREE_INTERIOR:
            nodeDelete(oldNode, idx, key);
            break;
        default:
            assert(false);
    }
//    delete oldNode;
    return newNode;
}
BNode* BTree::treeInsert(BNode* oldNode, std::vector<uint8_t>& key, std::vector<uint8_t>& val, UpdateResult& result){
    auto* newNode = new BNode(2);
    uint16_t idx = oldNode->nodeLookUpLE(key);

    switch(oldNode->bType()){
        case BTREE_LEAF:
            newNode->leafInsert(oldNode, idx, key, val,result);
            break;
        case BTREE_INTERIOR:
            nodeInsert(oldNode, newNode, idx, key, val, result);
            break;
        default:
            assert(false);
    }
    if(!result.updated && !result.added) {
        delete newNode;
        return nullptr;
    }
    return newNode;
}

std::vector<uint8_t >BTree::treeGet(BNode* node, std::vector<uint8_t >&key){
    uint16_t idx = node->nodeLookUpLE(key);
    std::vector<uint8_t> ret;
    if(node->bType()== BTREE_INTERIOR) {
        uint64_t ptr = node->getPtr(idx);
        ret = treeGet(get(ptr), key);
    }
    else if(node->bType() == BTREE_LEAF)
        ret = idx!=node->nKeys() && node->getKey(idx) == key ? node->getVal(idx) : std::vector<uint8_t>();
    else
        assert(false);
    delete node;
    return ret;
}

void BTree::nodeInsert(BNode* old,BNode* newNode,uint16_t idx,
                         std::vector<uint8_t >& key, std::vector<uint8_t>& value, UpdateResult& result){
    uint64_t kptr = old->getPtr(idx);
    auto prevChild = get(kptr);
    auto knode = treeInsert( prevChild, key, value, result);
    delete prevChild;
    if(knode== nullptr){
        return;
    }
    auto splits = nodeSplit3(knode);
    del(kptr);
    nodeReplaceKidN(old, newNode, idx, splits);
}

BNode* BTree::nodeDelete(BNode* par,uint16_t idx,std::vector<uint8_t >&key){
    uint64_t kptr = par->getPtr(idx);
    auto prevChild = get(kptr);
    auto updated = treeDelete(prevChild, key);
//    delete prevChild;
    if(updated == nullptr){
        return nullptr;
    }
    del(kptr);
    auto newNode = new BNode(1);
    auto [mergeDir, sibling] = shouldMerge(par, idx, updated);
    switch (mergeDir) {
        case -1:
        {
            auto merged = new BNode(1);
            nodeMerge(merged, sibling, updated);
            del(par->getPtr(idx-1));
            auto fkey = merged->getKey(0);
            nodeReplace2Kid(newNode, par, idx-1, insert(merged), fkey);
            break;
        }
        case 1:
        {
            auto merged = new BNode(1);
            nodeMerge(merged, updated, sibling);
            del(par->getPtr(idx+1));
            auto fkey = merged->getKey(0);
            nodeReplace2Kid(newNode, par, idx, insert(merged), fkey);
            break;
        }
        case 0:
            if(updated->nKeys()==0){
                assert(par->nKeys()==1 && idx==0);
                newNode->_setHeader(BTREE_INTERIOR, 0);
            }
            else{
                nodeReplaceKidN(par, newNode, idx, {updated});
            }
        default:
            assert(false);
    }
    return newNode;
}

void BTree::Insert(std::vector<uint8_t> &key, std::vector<uint8_t> & val, UpdateResult& result) {
    checkLimit(key, val);

    if (root == 0){
        result.updated = false;
        if(result.type==UPDATE_UPDATE){
            result.added = false;
            return;
        }
        result.added = true;
        auto rootNode = new BNode(1);
        rootNode->_setHeader(BTREE_LEAF, 2);
        std::vector<uint8_t>k;
        rootNode->nodeAppendKV(0,k,k);
        rootNode->nodeAppendKV(1, key, val);
        root = insert(rootNode);
        return;
    }

    auto rootNode = get(root);
    auto node = treeInsert(rootNode, key, val, result);
    delete rootNode;
    if(node == nullptr)
        return;
    std::vector<BNode*> splits = nodeSplit3(node);
    del(root);

    if(splits.size()>1){
        rootNode = new BNode(1);
        rootNode->_setHeader(BTREE_INTERIOR, splits.size());
        std::vector<uint8_t> k,v;
        for(uint16_t i=0;i<(uint16_t)splits.size();i++){
            k = splits[i]->getKey(0);
            rootNode->nodeAppendKV(i, k, v, insert(splits[i]));
        }
        root = insert(rootNode);
    }
    else {
        root = insert(splits[0]);
    }
}

bool BTree::Delete(std::vector<uint8_t> & key) {
    if(root==0 || key.empty())
        return false;
    auto rootNode = get(root);
    auto updatedRoot = treeDelete(rootNode, key);
    delete rootNode;
    if(updatedRoot==nullptr)
        return false;
    del(root);
    root = insert(updatedRoot);
    return true;
}

std::vector<uint8_t> BTree:: Get(std::vector<uint8_t> & key) {
    if(root==0)
        return {};
    auto rootNode = get(root);
    return treeGet(rootNode, key);
}

void nodeSplit2(BNode* left, BNode* right, BNode* old){
    uint16_t offSize = OFFSET_ARRAY_ELEMENT_SIZE + ( old->bType()==BTREE_INTERIOR?
            POINTER_ARRAY_ELEMENT_SIZE:0);
    int i;
    uint16_t lenReq = HEADER_SIZE;

    for(i=old->nKeys(); lenReq<BTREE_PAGE_SIZE/2; i--){
        lenReq += offSize + old->getOffset(i)-old->getOffset(i-1);
    }
    i++;
    if(i==old->nKeys())i--;
    right->_setHeader(old->bType(), old->nKeys()-i);
    right->nodeAppendRange(old, 0,i, old->nKeys()-i);

    left->_setHeader(old->bType(), i);
    left->nodeAppendRange(old, 0, 0, i);
    delete old;
}

std::vector<BNode*> nodeSplit3(BNode* old){
    if(old->nBytes()<=BTREE_PAGE_SIZE){
        old->shrink(1);
        return { old};
    }
    auto * left = new BNode(2);
    auto* right = new BNode(1);
    nodeSplit2(left, right, old);

    if(left->nBytes()<=BTREE_PAGE_SIZE) {
        left->shrink(1);
        return {left, right};
    }
    auto* leftleft = new BNode(1);

    auto* middle = new BNode(1);

    nodeSplit2(leftleft, middle, left);

    assert(leftleft->nBytes()<=BTREE_PAGE_SIZE);
    return {leftleft, middle, right};
}

void checkLimit(std::vector<uint8_t >&key, std::vector<uint8_t >&val){
    assert(key.size()<=BTREE_MAX_KEY_SIZE && val.size()<=BTREE_MAX_VAL_SIZE && !key.empty());
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
    merged->nodeAppendRange(right, left->nKeys(), 0, right->nKeys());
    delete left;
    delete right;
}

void nodeReplace2Kid(BNode* newNode, BNode* oldNode, uint16_t idx, uint64_t ptr,
                std::vector<uint8_t > &key){
    //delete idx and idx+1
    newNode->_setHeader(BTREE_INTERIOR, oldNode->nKeys()-1);
    newNode->nodeAppendRange(oldNode,0,0,idx);
    std::vector<uint8_t >val;
    newNode->nodeAppendKV(idx, key, val, ptr);
    newNode->nodeAppendRange(oldNode, idx+1, idx+2, oldNode->nKeys()-idx-2);
}