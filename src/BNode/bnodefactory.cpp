//
// Created by Aquib Nawaz on 01/11/24.
//

#include "bnodefactory.h"

BNode *BNodeFactory::createNode(uint8_t *data, bool destroyData) {
    return new BNode(data, destroyData);
}