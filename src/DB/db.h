//
// Created by Aquib Nawaz on 05/10/24.
//

#ifndef SQLITEV2_DB_H
#define SQLITEV2_DB_H

#include <string>
#include "table.h"
#include "btree.h"
#include "mmap.h"

class DB {
    BTree *btree;
    static TableDef Meta_Def;
    bool getTable(std::string const&, TableDef&);
public:
    DB(MMapChunk*);
    bool CreateTable(TableDef&);
    bool Insert(std::string &, Row&);
    bool Get(std::string&, Row&);
    bool Delete(std::string&, Row&);
    ~DB();
};


#endif //SQLITEV2_DB_H
