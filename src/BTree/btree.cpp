//
// Created by Aquib Nawaz on 21/09/24.
//

#include "btree.h"
#include <cassert>

BTree::~BTree() {
    delete inputOutput;
}

BTree::BTree(BTreeIO *_inputOutput) {
    inputOutput = _inputOutput;
    root = 0;
}

uint64_t BTree::getRoot() const{
    return root;
}

void BTree::setRoot(uint64_t _root) {
    root = _root;
}

void BTree::nodeReplaceKidN( BNode* oldNode, BNode* newNode, uint16_t idx,
        std::vector<BNode*> kids){

    uint16_t inc = kids.size();

    newNode->_setHeader(BTREE_INTERIOR, oldNode->nKeys()+inc-1);
    newNode->nodeAppendRange(oldNode, 0, 0, idx);

    std::vector<uint8_t> key ,v;
    for(uint16_t i=0;i<(uint16_t)kids.size();i++){
        key = kids[i]->getKey(0);
        newNode->nodeAppendKV(idx+i,key, v, inputOutput->insert(kids[i]));
    }

    newNode->nodeAppendRange(oldNode, idx+inc, idx+1, oldNode->nKeys()-(idx+1));
}
BNode* BTree::treeDelete(BNode *oldNode, std::vector<uint8_t> & key, DeleteResult& result) {
    auto * newNode = new BNode(1);
    uint16_t idx = oldNode->nodeLookUpLE(key);
    switch (oldNode->bType()) {
        case BTREE_LEAF:
            if(key!=oldNode->getKey(idx)){
                delete oldNode;
                delete newNode;
                return nullptr;
            }
            newNode->leafDelete(oldNode, idx, result);
            break;
        case BTREE_INTERIOR:
            nodeDelete(oldNode, idx, key, result);
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
        ret = treeGet(inputOutput->get(ptr), key);
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
    auto prevChild = inputOutput->get(kptr);
    auto knode = treeInsert( prevChild, key, value, result);
    delete prevChild;
    if(knode== nullptr){
        return;
    }
    auto splits = BNode::nodeSplit3(knode);
    inputOutput->del(kptr);
    nodeReplaceKidN(old, newNode, idx, splits);
}

BNode* BTree::nodeDelete(BNode* par,uint16_t idx,std::vector<uint8_t >&key, DeleteResult& result){
    uint64_t kptr = par->getPtr(idx);
    auto prevChild = inputOutput->get(kptr);
    auto updated = treeDelete(prevChild, key, result);
//    delete prevChild;
    if(updated == nullptr){
        return nullptr;
    }
    inputOutput->del(kptr);
    auto newNode = new BNode(1);
    auto [mergeDir, sibling] = shouldMerge(par, idx, updated);
    switch (mergeDir) {
        case -1:
        {
            auto merged = new BNode(1);
            BNode::nodeMerge(merged, sibling, updated);
            inputOutput->del(par->getPtr(idx-1));
            auto fkey = merged->getKey(0);
            BNode::nodeReplace2Kid(newNode, par, idx-1, inputOutput->insert(merged), fkey);
            break;
        }
        case 1:
        {
            auto merged = new BNode(1);
            BNode::nodeMerge(merged, updated, sibling);
            inputOutput->del(par->getPtr(idx+1));
            auto fkey = merged->getKey(0);
            BNode::nodeReplace2Kid(newNode, par, idx, inputOutput->insert(merged), fkey);
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

    BNode::checkLimit(key, val);
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
        root = inputOutput->insert(rootNode);
        return;
    }

    auto rootNode = inputOutput->get(root);
    auto node = treeInsert(rootNode, key, val, result);
    delete rootNode;
    if(node == nullptr)
        return;
    std::vector<BNode*> splits = BNode::nodeSplit3(node);
    inputOutput->del(root);

    if(splits.size()>1){
        rootNode = new BNode(1);
        rootNode->_setHeader(BTREE_INTERIOR, splits.size());
        std::vector<uint8_t> k,v;
        for(uint16_t i=0;i<(uint16_t)splits.size();i++){
            k = splits[i]->getKey(0);
            rootNode->nodeAppendKV(i, k, v, inputOutput->insert(splits[i]));
        }
        root = inputOutput->insert(rootNode);
    }
    else {
        root = inputOutput->insert(splits[0]);
    }
}

bool BTree::Delete(std::vector<uint8_t> & key, DeleteResult& result) {
    if(root==0 || key.empty())
        return false;
    auto rootNode = inputOutput->get(root);
    auto updatedRoot = treeDelete(rootNode, key, result);
    delete rootNode;
    if(updatedRoot==nullptr)
        return false;
    inputOutput->del(root);
    root = inputOutput->insert(updatedRoot);
    return true;
}

std::vector<uint8_t> BTree:: Get(std::vector<uint8_t> & key) {
    if(root==0)
        return {};
    auto rootNode = inputOutput->get(root);
    return treeGet(rootNode, key);
}


std::pair<int, BNode*> BTree::shouldMerge(BNode* par, uint16_t idx, BNode* updated){
    if(updated->nBytes()>BTREE_PAGE_SIZE/4){
        return {0, nullptr};
    }
    if(idx>0){
        //Check left
        auto left = inputOutput->get(par->getPtr(idx-1));
        uint16_t merged_size = left->nBytes() + updated->nBytes()-HEADER_SIZE;
        if(merged_size<=BTREE_PAGE_SIZE){
            return {-1,left};
        }
    }
    if(idx<par->nKeys()-1){
        //Check right
        auto right = inputOutput->get(par->getPtr(idx+1));
        uint16_t merged_size = updated->nBytes() + right->nBytes()-HEADER_SIZE;
        if(merged_size<=BTREE_PAGE_SIZE){
            return {1,right};
        }
    }
    return {0, nullptr};
}



