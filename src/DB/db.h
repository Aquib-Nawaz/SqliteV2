//
// Created by Aquib Nawaz on 05/10/24.
//

#ifndef SQLITEV2_DB_H
#define SQLITEV2_DB_H

#include <string>
#include "table.h"
#include "keyvalue.h"

class DB {
    KeyValue *kvStore;
    static TableDef Meta_Def;
    bool getTable(std::string const&, TableDef&);
public:
    explicit DB(KeyValue * kvStore);
    bool CreateTable(TableDef&);
    bool Insert(std::string &, Row&, UpdateResult&);
    bool Get(std::string&, Row&);
    bool Delete(std::string&, Row&);
    ~DB();
};


#endif //SQLITEV2_DB_H
