//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_BTREE_H
#define SQLITEV2_BTREE_H


#include <cstdint>
#include "bnode.h"
#include <utility>

#define BTREE_MAX_KEY_SIZE 4000
#define BTREE_MAX_VAL_SIZE 12000

class BTreeIterator;
class BTree {
    friend class BTreeIterator;
    virtual BNode * get(uint64_t pageNum)=0;
    virtual void del(uint64_t pageNum)=0;
    virtual uint64_t insert(BNode* node)=0;
    void nodeReplaceKidN(
            BNode* oldNode, BNode* newNode, uint16_t idx,
            std::vector<BNode*>kids);
    BNode* treeInsert(BNode* oldNode, std::vector<uint8_t >& key, std::vector<uint8_t>& value, UpdateResult& result);
    BNode* treeDelete(BNode* oldNode,std::vector<uint8_t >& key);
    std::vector<uint8_t > treeGet(BNode* node,std::vector<uint8_t >& key);
    void nodeInsert(BNode* ,BNode*,uint16_t , std::vector<uint8_t >&, std::vector<uint8_t>& , UpdateResult& result);
    std::pair<int,BNode*> shouldMerge(BNode*, uint16_t , BNode*);
    BNode* nodeDelete(BNode*,uint16_t ,std::vector<uint8_t >&);
    protected:
    uint64_t root;
    public:
        virtual ~BTree();
        virtual void Insert(std::vector<uint8_t >& key, std::vector<uint8_t >& value, UpdateResult& result);
        virtual bool Delete(std::vector<uint8_t >&);
        virtual std::vector<uint8_t > Get(std::vector<uint8_t >&);
};
std::vector<BNode*> nodeSplit3(BNode* );
void nodeSplit2(BNode* , BNode* , BNode* );
void checkLimit(std::vector<uint8_t >&,std::vector<uint8_t >&);
void nodeMerge(BNode*,BNode*,BNode*);
void nodeReplace2Kid(BNode*,BNode*,uint16_t, uint64_t , std::vector<uint8_t>&);



#endif //SQLITEV2_BTREE_H
