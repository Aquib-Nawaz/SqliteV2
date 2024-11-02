//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_BTREE_H
#define SQLITEV2_BTREE_H


#include <cstdint>
#include "bnode.h"
#include <utility>
#include "btreeio.h"

class BTreeIterator;
class BTree {
    friend class BTreeIterator;

    BTreeIO* inputOutput;
    void nodeReplaceKidN(
            BNode* oldNode, BNode* newNode, uint16_t idx,
            std::vector<BNode*>kids);
    BNode* treeInsert(BNode* oldNode, std::vector<uint8_t >& key, std::vector<uint8_t>& value, UpdateResult& result);
    BNode* treeDelete(BNode* oldNode,std::vector<uint8_t >& key, DeleteResult& result);
    std::vector<uint8_t > treeGet(BNode* node,std::vector<uint8_t >& key);
    void nodeInsert(BNode* ,BNode*,uint16_t , std::vector<uint8_t >&, std::vector<uint8_t>& , UpdateResult& result);
    std::pair<int,BNode*> shouldMerge(BNode*, uint16_t , BNode*);
    BNode* nodeDelete(BNode*,uint16_t ,std::vector<uint8_t >&, DeleteResult& result);
    protected:
    uint64_t root{};
    public:
        BTree(BTreeIO* _inputOutput);
        virtual ~BTree();
        virtual void Insert(std::vector<uint8_t >& key, std::vector<uint8_t >& value, UpdateResult& result);
        virtual bool Delete(std::vector<uint8_t >&, DeleteResult&);
        virtual std::vector<uint8_t > Get(std::vector<uint8_t >&);
        uint64_t getRoot() const;
        void setRoot(uint64_t _root);
};


#endif //SQLITEV2_BTREE_H
