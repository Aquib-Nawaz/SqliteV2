//
// Created by Aquib Nawaz on 24/09/24.
//

#include "hashmapkv.h"
#include "btreememory/inmemorybtreeio.h"

HashMapKV::HashMapKV(BNodeFactory* factory) {
    btree = new BTree(new InMemoryBTreeIO(factory));
}
void HashMapKV::Insert(std::vector<uint8_t> &key, std::vector<uint8_t> & value, UpdateResult& res) {
    btree->Insert(key, value, res);
}
std::vector<uint8_t> HashMapKV::Get(std::vector<uint8_t> & key) {
    return btree->Get(key);
}
bool HashMapKV::Delete(std::vector<uint8_t >& key, DeleteResult& res){
    return btree->Delete(key, res);
}

HashMapKV::~HashMapKV() {
    delete btree;
}


