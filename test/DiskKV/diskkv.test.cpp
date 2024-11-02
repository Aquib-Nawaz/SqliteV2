#include <stdio.h>
#include "../minunit.h"
#include "mmap.h"
#include <filesystem>
#include "pagedb.h"
#include <fstream>

int tests_run = 0;

static const char * EndToEndInsertTest(){
    const char * filePath = "test.db";
    std::remove(filePath);
    MMapChunk mmap = MMapChunk(filePath);
    std::vector<uint8_t>key,val;
    const char* csvFile = "../../../data/wine-quality.csv";
    std::filesystem::path path(csvFile);
    mu_assert("File Exist", std::filesystem::exists(path));
    std::ifstream csv(csvFile);
    std::string line;
    std::string keyPref = "WineQuality-";
    int idx=0;
    BNodeFactory factory;
    DiskKV *tree = new DiskKV(&mmap, &factory);

    UpdateResult res;
    res.type = UPDATE_INSERT;
    while(std::getline(csv,line)) {
        val = std::vector<uint8_t >(line.begin(), line.end());
        std::string temp = keyPref + std::to_string(idx++);
        key = std::vector<uint8_t>(temp.begin(), temp.end());
        tree->Insert(key, val, res);
        mu_assert_iter(idx, "Insert Error", res.added&&!res.updated);
        mu_assert_iter(idx, "Value Mismatch", tree->Get(key) == val);
    }
    delete tree;
    return nullptr;
}

static const char * EndToEndPersistenceTest(){
    const char * filePath = "test.db";
    MMapChunk mmap = MMapChunk(filePath);
    std::vector<uint8_t>key,val;
    const char* csvFile = "../../../data/wine-quality.csv";
    std::filesystem::path path(csvFile);
    mu_assert("File Exist", std::filesystem::exists(path));
    std::ifstream csv(csvFile);
    std::string line;
    std::string keyPref = "WineQuality-";
    int idx=0;
    BNodeFactory factory;
    DiskKV *tree = new DiskKV(&mmap, &factory);
    while(std::getline(csv,line)) {
        val = std::vector<uint8_t >(line.begin(), line.end());
        std::string temp = keyPref + std::to_string(idx++);
        key = std::vector<uint8_t>(temp.begin(), temp.end());
        mu_assert_iter(idx, "Value Mismatch", tree->Get(key) == val);
    }
    delete tree;
    std::remove(filePath);
    return nullptr;
}


static const char* all_tests(){

    mu_run_test(EndToEndInsertTest);
    mu_run_test(EndToEndPersistenceTest);
    return nullptr;
}

int main(){

    printf("Executing test file: %s\n", __FILE_NAME__);
    const char * result = all_tests();
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != nullptr;
}