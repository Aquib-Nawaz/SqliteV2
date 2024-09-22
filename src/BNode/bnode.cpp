//
// Created by Aquib Nawaz on 21/09/24.
//

#include "bnode.h"
#include <cassert>
#include "../DataConversion/convertor.h"

BNode::BNode(uint8_t bytes[]) {
    data = new uint8_t[BTREE_PAGE_SIZE];
    memcpy(data, bytes, BTREE_PAGE_SIZE);
    btype = _btype();
    nkeys = _nkeys();
}

BNode::BNode( uint16_t btype, uint16_t nkeys) {
    data = new uint8_t[BTREE_PAGE_SIZE];
    _setHeader(btype, nkeys);
}

uint16_t BNode::_btype() {
    return littleEndianByteToInt16(data);
}

uint16_t BNode::_nkeys() {
    return littleEndianByteToInt16(data + 2);
}

uint16_t BNode::bType() const {
    return btype;
}

uint16_t BNode::nKeys() const {
    return nkeys;
}

void BNode::_setHeader(uint16_t type, uint16_t numkeys) {
    littleEndianInt16ToBytes(type, data);
    littleEndianInt16ToBytes(numkeys, data + 2);
    btype = type;
    nkeys = numkeys;
}

uint16_t BNode::offsetPos(uint16_t idx) {
    return HEADER_SIZE + ((idx-1)*OFFSET_ARRAY_ELEMENT_SIZE);
}

uint16_t BNode::getOffset(uint16_t idx){
    assert(idx<=nkeys && idx>=0);
    if(idx == 0)
        return 0;
    return littleEndianByteToInt16(data + offsetPos(idx));
}


void BNode::setOffset( uint16_t idx, uint16_t offset){
    assert(idx<=nkeys && idx>=1);
    littleEndianInt16ToBytes(offset, data + offsetPos(idx));
}

uint64_t BNode::getPtr(uint16_t idx) {
    assert(idx<nkeys && idx>=0);
    uint16_t pos = HEADER_SIZE + idx*POINTER_ARRAY_ELEMENT_SIZE +
            nkeys*OFFSET_ARRAY_ELEMENT_SIZE;
    return littleEndianByteToInt64(data + pos);
}

void BNode::setPtr(uint16_t idx, uint64_t ptr) {
    assert(idx<nkeys && idx>=0);
    uint16_t pos = HEADER_SIZE + idx*POINTER_ARRAY_ELEMENT_SIZE +
            nkeys*OFFSET_ARRAY_ELEMENT_SIZE;
    littleEndianInt64ToBytes(ptr, data + pos);
}

uint16_t BNode::kvPos(uint16_t idx) {
    return HEADER_SIZE + (btype==BTREE_LEAF?0:nkeys*POINTER_ARRAY_ELEMENT_SIZE) +
    nkeys*OFFSET_ARRAY_ELEMENT_SIZE + getOffset(idx);
}

std::vector<uint8_t> BNode::getKey(uint16_t idx) {
    assert(idx<nkeys && idx>=0);
    uint16_t pos = kvPos(idx);
    uint16_t klen = littleEndianByteToInt16(data + pos);
    std::vector<uint8_t> ret(klen);
    memcpy(ret.data(), data + pos + KLEN_NUM_BYTES + VLEN_NUM_BYTES, klen);
    return ret;
}

std::vector<uint8_t> BNode::getVal(uint16_t idx) {
    assert(idx<nkeys && idx>=0);
    uint16_t pos = kvPos(idx);
    uint16_t keyLen = littleEndianByteToInt16(data + pos);
    uint16_t valueLen = littleEndianByteToInt16(data + pos+2);
    std::vector<uint8_t> ret(valueLen);
    memcpy(ret.data(), data + pos + KLEN_NUM_BYTES +
        VLEN_NUM_BYTES+keyLen, valueLen);
    return ret;
}

uint16_t BNode::nBytes() {
    return kvPos(nkeys);
}

uint16_t BNode::nodeLookUpLE(std::vector<uint8_t >& key){
    uint16_t left = 0;
    uint16_t right = nkeys ;
    while (left < right) {
        uint16_t mid = (left + right) / 2;
        std::vector<uint8_t > k = getKey(mid);
        if (key <= k) {
            right = mid;
        } else {
            left = mid+1;
        }
    }
    if(left < nkeys) {
        std::vector<uint8_t > k = getKey(left);
        if(key < k)
            left++;
    }
    return left;
}

void BNode::modifyData(uint16_t pos, void * srcData, uint16_t len){
    memcpy(data+pos, srcData, len);
}

void BNode::nodeAppendKV(uint16_t idx, std::vector<uint8_t> &key, std::vector<uint8_t>& val, uint64_t ptr) {
    if(bType() == BTREE_INTERIOR) {
        setPtr(idx,ptr);
    }
    uint16_t pos = kvPos(idx);
    uint16_t klen = key.size();

    littleEndianInt16ToBytes(klen, data+pos);
    pos+=KLEN_NUM_BYTES;
    uint16_t vlen = val.size();
    littleEndianInt16ToBytes(vlen, data+pos);
    pos+=VLEN_NUM_BYTES;
    modifyData(pos, key.data(), klen);
    pos+=klen;
    modifyData(pos, val.data(), vlen);
    setOffset(idx+1, getOffset(idx) + klen + vlen +
            KLEN_NUM_BYTES+VLEN_NUM_BYTES);
}

void BNode::nodeAppendRange(BNode* oldNode,  uint16_t destStart, uint16_t start, uint16_t num) {
    std::vector<uint8_t >key,val;
    for(int i=0;i<num;i++){
        key = oldNode->getKey(start+i);
        val = oldNode->getVal(start+i);
        nodeAppendKV(destStart+i, key,val);
    }
}

BNode* leafInsert(BNode* oldNode, uint16_t idx, std::vector<uint8_t>&key,
                std::vector<uint8_t>&val) {
    auto newNode = new BNode(BTREE_LEAF, oldNode->nKeys()+1);
    newNode->nodeAppendRange(oldNode, 0, 0, idx);
    newNode->nodeAppendKV(idx, key, val);
    newNode->nodeAppendRange(oldNode, idx+1, idx, oldNode->nKeys()-idx);
    return newNode;
}
