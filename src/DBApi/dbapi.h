//
// Created by Aquib Nawaz on 02/11/24.
//

#ifndef SQLITEV2_DBAPI_H
#define SQLITEV2_DBAPI_H

#include <vector>
#include <cstdint>

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

struct DeleteResult {
    std::vector<uint8_t >oldVal;
};

#endif //SQLITEV2_DBAPI_H
