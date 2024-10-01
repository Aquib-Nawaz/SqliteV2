//
// Created by Aquib Nawaz on 30/09/24.
//

#ifndef SQLITEV2_FLIST_H
#define SQLITEV2_FLIST_H


#include <cstdint>

class LNode {
    uint8_t *data;
public:
    explicit LNode(uint8_t*);
    uint64_t getNext();
    void setNext(uint64_t);
    uint64_t getPtr(uint16_t);
    void setPtr(uint16_t,uint64_t);
    ~LNode();
};

class FList {
    uint64_t headPage;
    uint64_t headSeq;
    uint64_t tailPage;
    uint64_t tailSeq;
    uint64_t maxSeq;
    static uint16_t seq2Idx(uint64_t);

public:

    FList(uint64_t , uint64_t, uint64_t, uint64_t);
    void setMaxSeq();
    ~FList() = default;
};


#endif //SQLITEV2_FLIST_H
