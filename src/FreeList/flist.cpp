//
// Created by Aquib Nawaz on 30/09/24.
//

#include "flist.h"
#include "convertor.h"
#define FREE_LIST_HEADER 8
#define FREE_LIST_PAGE_SIZE 16384
#define FREE_LIST_CAPACITY ((FREE_LIST_PAGE_SIZE-FREE_LIST_HEADER)/8)

FList::FList(uint64_t hp, uint64_t hs, uint64_t tp, uint64_t ts):
headPage(hp),headSeq(hs),tailPage(tp),tailSeq(ts),maxSeq(0){}

void FList::setMaxSeq() {maxSeq=tailSeq;}

uint16_t FList::seq2Idx(uint64_t seq){
    return (uint16_t)(seq%FREE_LIST_CAPACITY);
}

LNode::LNode(uint8_t *d):data(d){}

uint64_t LNode::getNext(){
    return littleEndianByteToInt64(data);
}

void LNode::setNext(uint64_t next){
    littleEndianInt64ToBytes(next,data);
}

uint64_t LNode::getPtr(uint16_t pos){
    return littleEndianByteToInt64(data+pos*8);
}

void LNode::setPtr(uint16_t pos,uint64_t ptr){
    littleEndianInt64ToBytes(ptr,data+pos*8);
}

LNode::~LNode(){delete [] data;}



