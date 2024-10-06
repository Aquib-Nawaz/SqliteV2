//
// Created by Aquib Nawaz on 03/10/24.
//

#include <cassert>
#include "table.h"
#include "convertor.h"

StringRecord::StringRecord(const char* _value, uint32_t _len, bool _copy):copy(_copy),
    len(_len) {
    if(copy){
        value = new char[len];
        memcpy((void*)value, _value, len);
    }
    else{
        value = _value;
    }
}

uint32_t StringRecord::lengthInBytes() {
    return len+2;
}

StringRecord::StringRecord(uint8_t * bytes, bool _copy):copy(_copy) {
    len = littleEndianByteToInt16(bytes);
    if(copy){
        value = new char[len];
        memcpy((void*)value, bytes+2, len);
    }
    else
        value = (const char*)bytes+2;
}

uint32_t StringRecord::convertToBytes(uint8_t * bytes) {
    littleEndianInt16ToBytes(len, bytes);
    memcpy(bytes+2, value, len);
    return lengthInBytes();
}

RecordType StringRecord::getType() {
    return RECORD_STRING;
}

std::string StringRecord::toString() {
    return {value, len};
}

StringRecord::~StringRecord() {
    if(copy){
        delete[] value;
    }
}

IntRecord::IntRecord(int _val):value(_val){
}

IntRecord::IntRecord(uint8_t *byte) {
    value= littleEndianByteToInt32(byte);
}

uint32_t IntRecord::convertToBytes(uint8_t * bytes) {
    littleEndianInt32ToBytes(value, bytes);
    return lengthInBytes();
}

uint32_t IntRecord::lengthInBytes() {
    return 4;
}

RecordType IntRecord::getType() {
    return RECORD_INT;
}

int IntRecord::toInt() const {
    return value;
}

std::string IntRecord::toString() {
    return std::to_string(value);
}

TableDef::TableDef(uint8_t * key , uint8_t * val) {
    prefix = IntRecord(val).toInt();
    uint32_t offset=4;
    pKey = littleEndianByteToInt16(val + offset);
    offset+=2;
    uint16_t numRecords = littleEndianByteToInt16(val + offset);
    offset+=2;
    for(int i=0;i<(int)numRecords;i++){
        types.push_back((RecordType)val[offset++]);
        StringRecord temp(val+offset);
        offset += temp.lengthInBytes();
        columnNames.push_back(temp.toString());
    }
    //IgnorePrefix
    name = StringRecord(key+4).toString();
    assert(checkSanity());
}


uint32_t TableDef::keyLength() const {
    uint32_t ret = 4+StringRecord(name.c_str(), name.length()).lengthInBytes();
    return ret;
}

uint32_t TableDef::valueLength() {
    uint32_t ret =  sizeof (prefix ) + sizeof (pKey) + 2 + types.size();
    for(int i=0;i<(int)types.size();i++){
        ret+=StringRecord(columnNames[i].c_str(), columnNames[i].length()).lengthInBytes();
    }
    return ret;
}


std::vector<uint8_t> TableDef::getValue() {
    uint32_t offset = 0;
    std::vector<uint8_t> bytes(valueLength());
    littleEndianInt32ToBytes(prefix, bytes.data() + offset);
    offset += 4;
    littleEndianInt16ToBytes(pKey, bytes.data() + offset);
    offset += 2;
    littleEndianInt16ToBytes(types.size(), bytes.data() + offset);
    offset += 2;
    int sz = (int)types.size();
    for(int i=0;i<sz;i++){
        bytes[offset++] = (uint8_t )types[i];
        offset += StringRecord(columnNames[i].c_str(), columnNames[i].length())
                .convertToBytes(bytes.data()+offset);
    }
    return bytes;
}

std::vector<uint8_t> TableDef::getKey() const {
    std::vector<uint8_t> bytes(keyLength());
    littleEndianInt32ToBytes(1, bytes.data());
    StringRecord(name.c_str(), name.length()).convertToBytes(bytes.data()+4);
    return bytes;
}

void TableDef::pushColumn(std::string col, RecordType type) {
    columnNames.push_back(std::move(col));
    types.push_back(type);
}

bool TableDef::operator==(const TableDef &rhs) const {
    return (&rhs == this) || (prefix == rhs.prefix &&
           pKey == rhs.pKey &&
           types == rhs.types &&
           columnNames == rhs.columnNames &&
           name == rhs.name);
}

bool TableDef::checkSanity() {
    return columnNames.size() >= pKey;
}

Row:: Row(std::vector<uint8_t> &key, std::vector<uint8_t> &val, TableDef &tableDef){
    uint32_t offset = 4;
    uint16_t numColumns = tableDef.columnNames.size();
    uint8_t * data = key.data();
    for(uint16_t i=0;i<numColumns;i++){
        if(tableDef.types[i] == RECORD_INT){
            value.push_back(new IntRecord(data + offset));
            offset += 4;
        }else if(tableDef.types[i] == RECORD_STRING){
            value.push_back(new StringRecord(data+offset, true));
            offset += value.back()->lengthInBytes();
        }
        if(i==tableDef.pKey-1){
            offset=0;
            data = val.data();
        }
    }
    cols = tableDef.columnNames;
}

Row::Row(std::vector<std::string>key, std::vector<Record*>val) {
    cols = std::move(key);
    value = std::move(val);
}

void Row::pushRow(std::string col, Record* val){
    cols.push_back(std::move(col));
    value.push_back(val);
}

bool Row::checkAndReorder(TableDef &tableDef, bool isKey) {

    if(!(isKey ? cols.size() == tableDef.pKey : cols.size() == tableDef.columnNames.size())){
        return false;
    }
    std::vector<Record*> temp;
    for(int i=0;i<(int)cols.size();i++){
        int j;
        std::string &columnName = tableDef.columnNames[i];
        for(j=0;j<(int)cols.size();j++){
            if(cols[j] == columnName){
                temp.push_back(value[j]);
                break;
            }
        }
        //unknown column
        if(j == (int)cols.size() || tableDef.types[i] != value[j]->getType()){
            return false;
        }
    }
    value = std::move(temp);
    for(int i=0;i<(int)cols.size();i++){
        cols[i] = tableDef.columnNames[i];
    }
    return true;
}

void Row::populateValue(TableDef &tableDef, std::vector<uint8_t> &val) {
    assert(tableDef.pKey==cols.size());
    uint32_t offset = 0;
    uint8_t *data = val.data();
    for(int i=tableDef.pKey;i<(int)tableDef.columnNames.size();i++){
        if(tableDef.types[i] == RECORD_INT){
            value.push_back(new IntRecord(data+offset));
            offset += 4;
        }else if(tableDef.types[i] == RECORD_STRING){
            value.push_back(new StringRecord(data+offset, true));
            offset += value.back()->lengthInBytes();
        }
        cols.push_back(tableDef.columnNames[i]);
    }
}

uint32_t Row::keyLength(TableDef & def) {
    //prefix
    uint32_t ret = 4;
    for(int i=0; i<def.pKey; i++){
        ret += value[i]->lengthInBytes();
    }
    return ret;
}

uint32_t Row::valueLength(TableDef &def) {
    uint32_t ret = 0;
    for(int i=def.pKey; i<(int)def.columnNames.size(); i++){
        ret += value[i]->lengthInBytes();
    }
    return ret;
}

std::vector<uint8_t> Row::getKey(TableDef &tableDef) {
    std::vector<uint8_t> bytes(keyLength(tableDef));
    littleEndianInt32ToBytes(tableDef.prefix, bytes.data());
    uint32_t offset = 4;
    for(int i=0;i<tableDef.pKey;i++){
        offset += value[i]->convertToBytes(bytes.data()+offset);
    }
    return bytes;
}

std::vector<uint8_t> Row::getValue(TableDef &tableDef) {
    std::vector<uint8_t> bytes(valueLength(tableDef));
    uint32_t offset = 0;
    for(int i=tableDef.pKey;i<(int)tableDef.columnNames.size();i++){
        offset += value[i]->convertToBytes(bytes.data()+offset);
    }
    return bytes;
}

Row::~Row() {
    for(auto & val : value){
        delete val;
    }
}