//
// Created by Aquib Nawaz on 01/11/24.
//

#ifndef SQLITEV2_BTREEIO_H
#define SQLITEV2_BTREEIO_H
#include "bnode.h"
class BTreeIO {
public:
    virtual ~BTreeIO()= default;
    virtual BNode* get(uint64_t pageNum)=0;
    virtual void del(uint64_t pageNum)=0;
    virtual uint64_t insert(BNode* node)=0;
};

#endif //SQLITEV2_BTREEIO_H
