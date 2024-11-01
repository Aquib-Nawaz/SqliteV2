//
// Created by Aquib Nawaz on 24/09/24.
//

#include "hashmapdb.h"
#include "btreememory/inmemorybtreeio.h"

HashMapDBMemory::HashMapDBMemory() {
    btree = new BTree(new InMemoryBTreeIO());
}
void HashMapDBMemory::Insert(std::vector<uint8_t> &key, std::vector<uint8_t> & value, UpdateResult& res) {
    btree->Insert(key, value, res);
}
std::vector<uint8_t> HashMapDBMemory::Get(std::vector<uint8_t> & key) {
    return btree->Get(key);
}
bool HashMapDBMemory::Delete(std::vector<uint8_t >& key, DeleteResult& res){
    return btree->Delete(key, res);
}

HashMapDBMemory::~HashMapDBMemory() {
    delete btree;
}


