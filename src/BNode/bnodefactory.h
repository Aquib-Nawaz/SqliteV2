//
// Created by Aquib Nawaz on 01/11/24.
//

#ifndef SQLITEV2_BNODEFACTORY_H
#define SQLITEV2_BNODEFACTORY_H

#include "bnode.h"

class BNodeFactory {
public:
    BNode* createNode(uint8_t * data, bool destroyData);
};


#endif //SQLITEV2_BNODEFACTORY_H
