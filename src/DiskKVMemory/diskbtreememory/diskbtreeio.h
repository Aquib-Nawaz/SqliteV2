//
// Created by Aquib Nawaz on 01/11/24.
//

#ifndef SQLITEV2_DISKBTREEIO_H
#define SQLITEV2_DISKBTREEIO_H

#include "mmap.h"
#include "btreeio.h"
#include "bnodefactory.h"

class DiskBTreeIO: public BTreeIO {
    MMapChunk *mmap;
    BNodeFactory* nodeFactory;

    public:
    DiskBTreeIO(MMapChunk *mmap, BNodeFactory *factory);

    BNode * get(uint64_t key) override;
    void del(uint64_t key) override;
    uint64_t insert(BNode *val) override;
    void commit() override;
    ~DiskBTreeIO() override;
};


#endif //SQLITEV2_DISKBTREEIO_H
