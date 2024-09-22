//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_BTREE_H
#define SQLITEV2_BTREE_H


#include <cstdint>

class BTree {
    uint64_t root;
    public:
        uint8_t * get(uint64_t);
        void del(uint64_t);
        uint64_t insert(uint8_t []);
};

#endif //SQLITEV2_BTREE_H
