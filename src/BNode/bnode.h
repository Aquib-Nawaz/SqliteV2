//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_BNODE_H
#define SQLITEV2_BNODE_H

#include <cstdint>
#include <vector>

#define HEADER_SIZE 4
#define BTREE_PAGE_SIZE 4096
#define KLEN_NUM_BYTES 2
#define VLEN_NUM_BYTES 2
#define POINTER_ARRAY_ELEMENT_SIZE 8
#define OFFSET_ARRAY_ELEMENT_SIZE 2

enum BTreeNodeType {
    BTREE_LEAF = 0,
    BTREE_INTERIOR = 1
};


class BNode {
    uint64_t btype, nkeys;
    uint8_t *data;
    static uint16_t offsetPos(uint16_t);

public:
    BNode(uint8_t []);
    BNode(uint16_t , uint16_t);
    uint16_t bType() const;
    uint16_t nKeys() const;
    void _setHeader(uint16_t,uint16_t);
    void shrink(uint16_t ns);
    uint16_t nBytes();
    uint16_t getOffset(uint16_t);
    void setOffset( uint16_t, uint16_t);
    uint64_t getPtr(uint16_t);
    void setPtr(uint16_t , uint64_t );
    uint16_t kvPos(uint16_t);
    std::vector<uint8_t> getKey(uint16_t);
    std::vector<uint8_t> getVal(uint16_t);
    uint16_t nodeLookUpLE(std::vector<uint8_t >&);
    void modifyData(uint16_t srcPos, void *, uint16_t );
    void nodeAppendRange( BNode* ,  uint16_t , uint16_t , uint16_t);
    void nodeAppendKV( uint16_t , std::vector<uint8_t>&, std::vector<uint8_t>&, uint64_t ptr=0);
    void leafInsert( BNode* , uint16_t , std::vector<uint8_t>&,
                    std::vector<uint8_t>&);
    ~BNode();
};

#endif //SQLITEV2_BNODE_H
