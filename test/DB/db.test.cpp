//
// Created by Aquib Nawaz on 22/09/24.
//

#include <iostream>
#include "db.h"
#include "mmap.h"
#include "../minunit.h"
#include <filesystem>
using namespace std;

int tests_run = 0;
static  const char* DBCreateTableTest(){
    const char* filePath = "test.db";
    std::remove(filePath);
    MMapChunk mmap(filePath);
    DB db(&mmap);
    TableDef def;
    def.name = "table";
    def.pKey=1;
    def.pushColumn("col1", RECORD_INT);
    def.pushColumn("col2", RECORD_STRING);
    def.pushColumn("col3", RECORD_INT);
    mu_assert("DB::CreateTable", db.CreateTable(def));
    mu_assert("DB:Table prefix 3", def.prefix == 3);
    Row row({"col1", "col2", "col3"},{new IntRecord(1),
                                      new StringRecord("Hello", 5),
                                      new IntRecord(2)});
    UpdateResult res;
    res.type = UPDATE_INSERT;
    mu_assert("DB::Insert", db.Insert(def.name, row, res));
    Row row2;
    row2.pushRow("col1", new IntRecord(1));
    mu_assert("DB::Get", db.Get(def.name, row2));
    mu_assert("DB::Cols Match", row.cols == row2.cols);
    for(int i=0; i<row.cols.size(); i++){
        mu_assert_iter(i,"DB::Cols Value Match", row.value[i]->toString() == row2.value[i]->toString());
    }
    std::remove(filePath);
    return nullptr;
}

static  const char* DBCreate2TableTest(){
    const char* filePath = "test.db";
    std::remove(filePath);
    MMapChunk mmap(filePath);
    DB db(&mmap);
    TableDef def;
    string table1 = "table";
    def.name = "table";
    def.pKey=1;
    def.pushColumn("col1", RECORD_INT);
    def.pushColumn("col2", RECORD_STRING);
    def.pushColumn("col3", RECORD_INT);
    std::vector<std::string> idx1 = {"col2", "col3"};
    def.addIndex(idx1);
    std::vector<std::string> idx2 = {"col3", "col2"};
    def.addIndex(idx1);
    mu_assert("DB::CreateTable", db.CreateTable(def));
    mu_assert("DB:Table prefix 3", def.prefix == 3);
    mu_assert("DB::CreateTable same name", !db.CreateTable(def));
    def.name = "table2";
    mu_assert("DB::CreateTable different name", db.CreateTable(def));
    mu_assert("DB:Table prefix 6", def.prefix == 6);
    Row row({"col1", "col2", "col3"},{new IntRecord(1),
                                      new StringRecord("Hello", 5),
                                      new IntRecord(2)});
    UpdateResult res;
    res.type = UPDATE_INSERT;
    mu_assert("DB::Insert", db.Insert(table1, row, res));
    Row row2;
    row2.pushRow("col1", new IntRecord(1));
    mu_assert("DB::Get no match for table2", !db.Get(def.name, row2));
    mu_assert("DB::Get no match for table2", db.Get(table1, row2));

    std::remove(filePath);
    return nullptr;
}



static const char* all_tests(){
    mu_run_test(DBCreateTableTest);
    mu_run_test(DBCreate2TableTest);
    return nullptr;
}


int main(){

    const char * result = all_tests();
    printf("Executing test file: %s\n", __FILE_NAME__);
    if(result != nullptr){
        printf("%s\n", result);
    }else{
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != nullptr;
}