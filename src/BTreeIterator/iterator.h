/**
 * @file iterator.h
 * @brief Header file for the BTreeIterator class.
 *
 * This file contains the definition of the BTreeIterator class, which provides
 * an iterator for traversing the B-tree structure.
 *
 * @author Aquib Nawaz
 * @date 08/10/24
 */

#ifndef SQLITEV2_ITERATOR_H
#define SQLITEV2_ITERATOR_H

#include <stack>
#include <utility>
#include <vector>
#include "btree.h"

/**
 * @brief Class for iterating over the B-tree structure.
 *
 * This class provides an iterator for traversing the B-tree structure. It
 * maintains a stack of nodes and their indices to keep track of the current
 * position in the tree.
 */
class BTreeIterator {
    std::stack<std::pair<BNode*, uint16_t>> _stack; ///< Stack to keep track of nodes and their indices.
    BTree* _btree; ///< Pointer to the BTree being iterated over.
    void iterNext(bool direction); ///< Helper function to advance the iterator.

public:
    /**
     * @brief Constructor for BTreeIterator.
     * @param btree Pointer to the BTree to iterate over.
     */
    BTreeIterator(BTree* btree);

    /**
     * @brief Checks if the iterator is in a valid state.
     * @return True if the iterator is valid, false otherwise.
     */
    bool valid();

    /**
     * @brief Advances the iterator to the next element.
     */
    void next();

    /**
     * @brief Moves the iterator to the previous element.
     */
    void prev();

    /**
     * @brief Retrieves the key at the current iterator position.
     * @return A vector of bytes representing the key.
     */
    std::vector<uint8_t> key();

    /**
     * @brief Retrieves the value at the current iterator position.
     * @return A vector of bytes representing the value.
     */
    std::vector<uint8_t> value();

    /**
     * @brief Positions the iterator to the largest element less than or equal to the given key.
     * @param key The key to search for.
     */
    void seekLE(std::vector<uint8_t>& key);

    /**
     * @brief Destructor for BTreeIterator. Cleans up allocated nodes.
     */
    ~BTreeIterator();
};

#endif // SQLITEV2_ITERATOR_Hs