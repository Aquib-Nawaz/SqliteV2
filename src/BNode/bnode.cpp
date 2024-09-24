//
// Created by Aquib Nawaz on 21/09/24.
//

#include "bnode.h"
#include <cassert>
#include "../DataConversion/convertor.h"

BNode::BNode(uint8_t nm) {
    data = new uint8_t[BTREE_PAGE_SIZE*nm];
}

BNode::BNode(uint8_t * _data){
    data = _data;
    btype = littleEndianByteToInt16(data);
    nkeys = littleEndianByteToInt16(data+2);
}

BNode::BNode( uint16_t btype, uint16_t nkeys) {
    data = new uint8_t[BTREE_PAGE_SIZE];
    _setHeader(btype, nkeys);
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

    memcpy(&ret[0], data + pos + KLEN_NUM_BYTES +
        VLEN_NUM_BYTES+keyLen, valueLen*sizeof(ret[0]));
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
        if (key >= k) {
            left = mid+1;
        } else {
            right = mid;
        }
    }
    if(left < nkeys) {
        std::vector<uint8_t > k = getKey(left);
        if(k<=key)
            left++;
    }
    return left-1;
}

void BNode::modifyData(uint16_t pos, void * srcData, uint16_t len){
    memcpy(data+pos, srcData, len);
}

void BNode::nodeAppendKV(uint16_t idx, std::vector<uint8_t> &key, std::vector<uint8_t>& val, uint64_t ptr) {
    if(bType() == BTREE_INTERIOR) {
        assert(val.empty());
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
    uint16_t dataLen = oldNode->getOffset(start+num) - oldNode->getOffset(start);
    memcpy(data + kvPos(destStart), oldNode->data + oldNode->kvPos(start), dataLen);
    uint16_t offSetOffSet = getOffset(destStart)-oldNode->getOffset(start);
    for (uint16_t i = 1; i <= num; ++i) {
        setOffset(destStart+i, oldNode->getOffset(start+i) + offSetOffSet);
    }
    if(bType() == BTREE_INTERIOR) {
        for (uint16_t i = 0; i < num; ++i) {
            setPtr(destStart+i, oldNode->getPtr(start+i));
        }
    }
}

void BNode::leafInsert( BNode* oldNode, uint16_t idx, std::vector<uint8_t>&key,
                        std::vector<uint8_t>&val) {
    bool skipOriginalKey = (idx<nkeys && key == oldNode->getKey(idx));
    _setHeader(BTREE_LEAF, oldNode->nKeys() + 1 - skipOriginalKey);
    nodeAppendRange(oldNode, 0, 0, idx+1-skipOriginalKey);
    nodeAppendKV(idx+1-skipOriginalKey, key, val);
    nodeAppendRange(oldNode, idx+2-skipOriginalKey, idx + 1, oldNode->nKeys() - idx - 1);
}

void BNode::shrink(uint16_t ns) {
    auto *newData = new uint8_t[ns*BTREE_PAGE_SIZE];
    memcpy(newData, data, ns*BTREE_PAGE_SIZE);
    delete[] data;
    data = newData;
}

void BNode::leafDelete(BNode* oldNode, uint16_t idx) {
    _setHeader(BTREE_LEAF, oldNode->nKeys() - 1);
    nodeAppendRange(oldNode, 0, 0, idx);
    nodeAppendRange(oldNode, idx, idx+1, oldNode->nKeys() - idx - 1);
}


uint8_t* BNode::getData() {
    assert(nBytes() <= BTREE_PAGE_SIZE);
    auto ret = new uint8_t[BTREE_PAGE_SIZE];
    memcpy(ret, data, nBytes());
    return ret;
}
BNode::~BNode() {
    delete[] data;
}
