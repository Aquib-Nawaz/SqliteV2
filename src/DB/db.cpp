//
// Created by Aquib Nawaz on 05/10/24.
//

#include "db.h"
#include "pagedb.h"

static const char* meta = "@meta";
static const char* metaPrefix = "prefix";
static const char* metaKey = "key";
static const char* metaVal = "val";

DB::DB(MMapChunk * mmap) {
    btree = new DiskPageDBMemory(mmap);
}

bool DB::getTable(std::string const &tableName, TableDef& def) {
    TableDef temp;
    temp.name = tableName;
    auto key = temp.getKey();
    auto val = btree->Get(key);
    if(val.empty()) {
        return false;
    }
    def = TableDef(key.data(), val.data());
    return true;
}

bool DB::CreateTable(TableDef &tableDef) {
    if(!tableDef.checkSanity()) {
       return false;
    }
    auto key = tableDef.getKey();
    if(!btree->Get(key).empty()){
        return false;
    }
    //Code to get the prefix
    Row row;
    row.pushRow(metaKey, new StringRecord(metaPrefix, strlen(metaPrefix)));
    auto prefixKey = row.getKey(Meta_Def);
    auto prefix = btree->Get(prefixKey);
    tableDef.prefix=3;
    if(!prefix.empty()) {
        tableDef.prefix = stoi(StringRecord(prefix.data()).toString(), nullptr, 10);
    }
    //Insert definition
    auto val = tableDef.getValue();
    UpdateResult res;
    res.type = UPDATE_INSERT;
    btree->Insert(key, val, res);

    //Update prefix in meta table
    std::string updatedPrefix = std::to_string(tableDef.prefix+1+tableDef.getIndex().size());
    row.pushRow(metaVal, new StringRecord(updatedPrefix.c_str(), updatedPrefix.size()));
    auto updatedPrefixVal = row.getValue(Meta_Def);
    res.type = UPDATE_UPSERT;
    btree->Insert(prefixKey, updatedPrefixVal, res);
    return true;
}

bool DB::Insert(std::string &tableName, Row &row, UpdateResult& res) {
    TableDef def;
    if(!getTable(tableName, def) || !row.checkAndReorder(def, false)) {
        return false;
    }
    auto key = row.getKey(def),
        val = row.getValue(def);
    btree->Insert(key, val, res);
    DeleteResult temp;
    if(res.updated){
        Row oldRow(key, res.oldValue, def);
        for(auto &indexKey: oldRow.getIndexTableKeys(def)){
            btree->Delete(indexKey, temp);
        }
    }
    if(res.updated || res.added){
        for(auto &indexKey: row.getIndexTableKeys(def)){
            btree->Insert(indexKey, key, res);
        }
    }
    return true;
}

bool DB::Get(std::string &tableName, Row &row) {
    TableDef def;
    if(!getTable(tableName, def) || !row.checkAndReorder(def, true)) {
        return false;
    }
    auto key = row.getKey(def);
    auto val = btree->Get(key);
    if(val.empty()) {
        return false;
    }
    row.populateValue(def, val);
    return true;
}

bool DB::Delete(std::string & tableName, Row & row) {
    TableDef def;
    if(!getTable(tableName, def) || !row.checkAndReorder(def, true)) {
        return false;
    }
    auto key = row.getKey(def);
    DeleteResult res;
    bool ret = btree->Delete(key, res);
    if(ret){
        row.populateValue(def, res.oldVal);
        for(auto &indexKey: row.getIndexTableKeys(def)){
            btree->Delete(indexKey, res);
        }
    }
    return ret;
}

DB::~DB() {
    delete btree;
}

TableDef DB::Meta_Def = []() {
    TableDef def;
    def.name = meta;
    def.prefix = 2;
    def.pKey = 1;
    def.pushColumn(metaKey, RECORD_STRING);
    def.pushColumn(metaVal, RECORD_STRING);
    return def;
}();