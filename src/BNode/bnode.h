//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_BNODE_H
#define SQLITEV2_BNODE_H

#include <cstdint>
#include <vector>
#include <unistd.h>

#define HEADER_SIZE 4
#define BTREE_PAGE_SIZE 16384
#define KLEN_NUM_BYTES 2
#define VLEN_NUM_BYTES 2
#define POINTER_ARRAY_ELEMENT_SIZE 8
#define OFFSET_ARRAY_ELEMENT_SIZE 2

/**
 * \enum BTreeNodeType
 * \brief Enum to represent the type of B-Tree node.
 */
enum BTreeNodeType {
    BTREE_LEAF = 0,      ///< Leaf node
    BTREE_INTERIOR = 1   ///< Interior node
};

/**
 * \enum UpdateType
 * \brief Enum to represent the type of update operation.
 */

enum UpdateType {
    UPDATE_INSERT = 0,
    UPDATE_UPDATE = 1,
    UPDATE_UPSERT = 2
};

/**
 * \struct UpdateResult
 * \brief Struct to represent the result of an update operation.
 */
struct UpdateResult {
    std::vector<uint8_t> oldValue;
    bool added;
    bool updated;
    UpdateType type;
};

/**
 * \class BNode
 * \brief Class representing a node in a B-Tree.
 */
class BNode {
    uint64_t btype, nkeys; ///< Type of the node and number of keys
    static uint16_t offsetPos(uint16_t); ///< Helper function to calculate offset position
    uint8_t *data; ///< Pointer to the node's data
    bool destroy; ///< Flag to indicate if the node should be destroyed
    uint16_t pointerLocation(uint16_t) const; ///< Helper function to get pointer location

public:
    /**
     * \brief Constructor for BNode.
     * \param num Size in number of pages of the node.
     * \param destroy Flag to indicate if the data would be destroyed at the end of node's in destructor.
     */
    explicit BNode(uint8_t num, bool destroy = true);

    /**
     * \brief Constructor for BNode with data.
     * \param data Pointer to the node's data.
     * \param destroy Flag to indicate if the data would be destroyed at the end of node's in destructor.
     */
    explicit BNode(uint8_t *data, bool destroy = true);

    /**
     * \brief Set node's data to null.
     */
    void resetData();

    /**
     * \brief Gets the node's data.
     * \return Pointer to the node's data.
     */
    uint8_t* getData();

    /**
     * \brief Gets the type of the node.
     * \return Type of the node.
     */
    [[nodiscard]] uint16_t bType() const;

    /**
     * \brief Gets the number of keys in the node.
     * \return Number of keys in the node.
     */
    [[nodiscard]] uint16_t nKeys() const;

    /**
     * \brief Sets the header of the node.
     * \param type Type of the node.
     * \param nkeys Number of keys in the node.
     */
    void _setHeader(uint16_t type, uint16_t nkeys);

    /**
     * \brief Shrinks the node's data.
     * \param ns New size of the data in page size.
     */
    void shrink(uint16_t ns);

    /**
     * \brief Gets the number of bytes node's data consist of.
     * \return Number of bytes in the node.
     */
    uint16_t nBytes();

    /**
     * \brief Gets the offset at a given idx.
     * \param idx Index to get the offset from.
     * \return Offset of the data at the given Index.
     */
    uint16_t getOffset(uint16_t idx);

    /**
     * \brief Sets the offset of data at a given index.
     * \param idx Index of the data to set the offset at.
     * \param offset Offset to set.
     */
    void setOffset(uint16_t idx, uint16_t offset);

    /**
     * \brief Gets the pointer at a given position.
     * \param idx Position to get the pointer from.
     * \return Pointer at the given position.
     */
    uint64_t getPtr(uint16_t idx);

    /**
     * \brief Sets the pointer at a given index.
     * \param idx Index to set the pointer at.
     * \param ptr Pointer to set.
     */
    void setPtr(uint16_t pos, uint64_t ptr);

    /**
     * \brief Gets the key-value position at a given index.
     * \param idx Index for which key value position is needed.
     * \return Key-value position for given index.
     */
    uint16_t kvPos(uint16_t idx);

    /**
     * \brief Gets the key at a given index.
     * \param idx Index to get the key from.
     * \return Key at the given index.
     */
    std::vector<uint8_t> getKey(uint16_t idx);

    /**
     * \brief Gets the value at a given index.
     * \param idx Index to get the value from.
     * \return Value at the given index.
     */
    std::vector<uint8_t> getVal(uint16_t idx);

    /**
     * \brief Looks up the largest element less than or equal to the given key.
     * \param key Key to search for.
     * \return Position of the largest element less than or equal to the given key.
     */
    uint16_t nodeLookUpLE(std::vector<uint8_t> &key);

    /**
     * \brief Modifies the node's data.
     * \param srcPos Source position to modify.
     * \param data Data to modify with.
     * \param size Size of the data.
     */
    void modifyData(uint16_t srcPos, void *data, uint16_t size);

    /**
     * \brief Appends a range of key-value pairs to the node.
     * \param srcNode Source node to append from.
     * \param destIdx Destination index to append to.
     * \param srcIdx Source idex to append from.
     * \param count Number of key-value pairs to append.
     */
    void nodeAppendRange(BNode* srcNode, uint16_t destIdx, uint16_t srcIdx, uint16_t count);

    /**
     * \brief Appends a key-value pair to the node.
     * \param idx Index to append the key-value pair at.
     * \param key Key to append.
     * \param val Value to append.
     * \param ptr Pointer in case of InteriorNode.
     */
    void nodeAppendKV(uint16_t idx, std::vector<uint8_t> &key, std::vector<uint8_t> &val, uint64_t ptr = 0);

    /**
     * \brief Inserts or Update a key-value pair into leaf node and copies the data in curnode.
     * \param srcNode Source node to insert from.
     * \param idx Index to insert the key-value pair at.
     * \param key Key to insert.
     * \param val Value to insert.
     * \param result Result of the update operation.
     */
    void leafInsert(BNode* srcNode, uint16_t idx, std::vector<uint8_t> &key, std::vector<uint8_t> &val, UpdateResult& result);

    /**
     * \brief Deletes a key-value pair from the leaf node and insert the remaining data in current node.
     * \param srcNode Original node to delete from.
     * \param idx Index of original node to delete the key-value pair from.
     */
    void leafDelete(BNode* srcNode, uint16_t idx);

    /**
     * \brief Updates or inserts a key-value pair into the node.
     * \param oldNode Original node to update or insert into.
     * \param idx Index to update or insert at.
     * \param key Key to update or insert.
     * \param val Value to update or insert.
     * \param update Flag to indicate if the operation is an update.
     */
    void updateOrInsert(BNode* oldNode, uint16_t idx, std::vector<uint8_t>&key,
                               std::vector<uint8_t>&val, bool update);

    /**
     * \brief Destructor for BNode.
     */
    ~BNode();
};

#endif //SQLITEV2_BNODE_H