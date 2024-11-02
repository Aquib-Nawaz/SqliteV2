//
// Created by Aquib Nawaz on 01/11/24.
//

#ifndef SQLITEV2_INMEMORYBTREEIO_H
#define SQLITEV2_INMEMORYBTREEIO_H
#include <unordered_map>
#include "btreeio.h"
#include "bnodefactory.h"

class InMemoryBTreeIO: public BTreeIO {
    std::unordered_map<uint64_t, uint8_t*> memory;
    uint64_t count;
    BNodeFactory* nodeFactory;
public:
    BNode * get(uint64_t key) override;
    void del(uint64_t key) override;
    uint64_t insert(BNode *val) override;
    void commit(uint64_t ) override;
    InMemoryBTreeIO(BNodeFactory *factory);
    ~InMemoryBTreeIO() override;
};



#endif //SQLITEV2_INMEMORYBTREEIO_H
