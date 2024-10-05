//
// Created by Aquib Nawaz on 05/10/24.
//

#include <iostream>
#include <string>
#include "../minunit.h"
#include "table.h"
#include "convertor.h"
using namespace std;

int tests_run = 0;

static const char* StringRecordTest(){
    string str = "Hello World";
    StringRecord record(str.c_str(), str.size());
    mu_assert("StringRecord::lengthInBytes", record.lengthInBytes() == str.size()+2);
    mu_assert("StringRecord::toString", record.toString() == str);
    mu_assert("StringRecord::getType", record.getType() == RECORD_STRING);
    uint8_t bytes[record.lengthInBytes()];
    record.convertToBytes(bytes);
    mu_assert("StringRecord::convertToBytes length", littleEndianByteToInt16(bytes) == str.size());
    mu_assert("StringRecord::convertToBytes", StringRecord(bytes).toString()==str);
    return nullptr;
}

static const char* IntRecordTest(){
    int val = 100;
    IntRecord record(val);
    mu_assert("IntRecord::lengthInBytes", record.lengthInBytes() == 4);
    mu_assert("IntRecord::getType", record.getType() == RECORD_INT);
    uint8_t bytes[record.lengthInBytes()];

    record.convertToBytes(bytes);
    mu_assert("IntRecord::convertToBytes", IntRecord(bytes).toInt() == val);
    return nullptr;
}

static const char* TableDefTest(){
    TableDef def;
    def.name = "table";
    def.prefix = 2;
    def.pKey = 1;
    def.pushColumn("col1", RECORD_INT);
    def.pushColumn("col2", RECORD_STRING);
    def.pushColumn("col3", RECORD_INT);
    def.pushColumn("col4", RECORD_STRING);

    std::vector<uint8_t >key=def.getKey(), val=def.getValue();
    TableDef def2(key.data(), val.data());
    mu_assert("TableDef::first", littleEndianByteToInt32(key.data())==1);
    mu_assert("TableDef::equality", def2 == def);
    return nullptr;
}

static const char * CheckAndReorderTest(){
    TableDef def;
    def.name = "table";
    def.prefix = 2;
    def.pKey = 1;
    def.pushColumn("col1", RECORD_INT);
    def.pushColumn("col2", RECORD_STRING);
    def.pushColumn("col3", RECORD_INT);

    Row row;
    row.pushRow("col1", new IntRecord(1));
    mu_assert("Row::checkAndReorder", row.checkAndReorder(def));
    row.pushRow("col3", new IntRecord(2));
    mu_assert("Row::checkAndReorder", !row.checkAndReorder(def));
    row.pushRow("col2", new StringRecord("Hello", 5));
    mu_assert("Row::checkAndReorder", row.checkAndReorder(def));
    return nullptr;
}

static const char * RowToByteTest(){
    TableDef def;
    def.name = "table";
    def.prefix = 2;
    def.pKey = 1;
    def.pushColumn("col1", RECORD_INT);
    def.pushColumn("col2", RECORD_STRING);
    def.pushColumn("col3", RECORD_INT);
    Row row;
    row.pushRow("col1", new IntRecord(1));
    row.pushRow("col2", new StringRecord("Hello", 5));
    row.pushRow("col3", new IntRecord(2));

    std::vector<uint8_t> key = row.getKey(def);
    std::vector<uint8_t> val = row.getValue(def);
    Row row2(key, val, def);
    mu_assert("Row::equality key", row2.getKey(def) == key);
    mu_assert("Row::equality val", row2.getValue(def) == val);
    return nullptr;
}

static const char* all_tests(){
    mu_run_test(StringRecordTest);
    mu_run_test(IntRecordTest);
    mu_run_test(TableDefTest);
    mu_run_test(CheckAndReorderTest);
    mu_run_test(RowToByteTest);
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