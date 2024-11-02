//
// Created by Aquib Nawaz on 08/10/24.
//

#include "iterator.h"
#include <cassert>

BTreeIterator::BTreeIterator(BTree *btree) {
    _btree = btree;
}

void BTreeIterator::iterNext(bool dir) {
    if(_stack.empty())
        return;
    BNode* node = _stack.top().first;
    uint16_t index = _stack.top().second;
    _stack.pop();
    if ((dir && index + 1 < node->nKeys()) || (!dir && index > 0)) {
        _stack.emplace(node, index + (dir?1:-1)); //same node next key
    } else {
        delete node;
        if (_stack.empty()) { //Reached root
            return;
        } else {
            iterNext(dir); //See if parents have sibling
            if(!_stack.empty()){
                uint16_t idx = dir?0:_stack.top().first->nKeys()-1;
                _stack.emplace(_btree->inputOutput->get(_stack.top().first->getPtr(_stack.top().second)), idx);
            }
        }
    }
}

bool BTreeIterator::valid() {
    if(_stack.empty() || _stack.top().first->bType() != BTREE_LEAF)
        return false;
    if(_stack.top().second==0 && _stack.top().first->getKey(0).empty())
        return false;
    return true;
}

void BTreeIterator::next() {
    iterNext(true);
}

void BTreeIterator::prev() {
    iterNext(false);
}

std::vector<uint8_t> BTreeIterator::key() {
    assert(valid());
    return _stack.top().first->getKey(_stack.top().second);
}

std::vector<uint8_t> BTreeIterator::value() {
    assert(valid());
    return _stack.top().first->getVal(_stack.top().second);
}

void BTreeIterator::seekLE(std::vector<uint8_t> &key) {
    BNode* node = _btree->inputOutput->get(_btree->root);
    while(node->bType() == BTREE_INTERIOR){
        uint16_t idx = node->nodeLookUpLE(key);
        _stack.emplace(node, idx);
        node = _btree->inputOutput->get(node->getPtr(idx));
    }
    _stack.emplace(node, node->nodeLookUpLE(key));
}

BTreeIterator::~BTreeIterator() {
    while(!_stack.empty()){
        delete _stack.top().first;
        _stack.pop();
    }
}