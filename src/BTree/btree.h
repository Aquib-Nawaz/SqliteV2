//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_BTREE_H
#define SQLITEV2_BTREE_H


#include <cstdint>
#include "bnode.h"

#define BTREE_MAX_KEY_SIZE 1000
#define BTREE_MAX_VAL_SIZE 3000


class BTree {
    uint64_t root;
    public:
        BNode * get(uint64_t);
        void del(uint64_t);
        uint64_t insert(BNode*);
        void Insert(std::vector<uint8_t >&, std::vector<uint8_t >&);
        bool Delete(std::vector<uint8_t >&);
        void nodeReplaceKidN(
            BNode* , BNode*, uint16_t ,
            std::vector<BNode*>);
        BNode* treeInsert(BNode* , std::vector<uint8_t >&, std::vector<uint8_t>& );
        void nodeInsert(BNode* ,BNode*,uint16_t , std::vector<uint8_t >&, std::vector<uint8_t>& );
};
std::vector<BNode*> nodeSplit3(BNode* );
void nodeSplit2(BNode* , BNode* , BNode* );
void checkLimit(std::vector<uint8_t >&,std::vector<uint8_t >&);



#endif //SQLITEV2_BTREE_H
