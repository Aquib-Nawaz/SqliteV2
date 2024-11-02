//
// Created by Aquib Nawaz on 01/11/24.
//

#ifndef SQLITEV2_KV_H
#define SQLITEV2_KV_H

#include <cstdint>
#include <vector>
#include "dbapi.h"

class KeyValue {
public:
    virtual ~KeyValue()= default;
    virtual void Insert(std::vector<uint8_t> &, std::vector<uint8_t> &, UpdateResult&) = 0;
    virtual std::vector<uint8_t> Get(std::vector<uint8_t> &) = 0;
    virtual bool Delete(std::vector<uint8_t >&, DeleteResult&) = 0;
};

#endif //SQLITEV2_KV_H
