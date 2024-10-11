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
    byteLen = _lengthInBytes();
}

StringRecord::StringRecord(const uint8_t * bytes):copy(true) {
    len=0;
    uint32_t i,j;
    for( i=0;i<=4000&&bytes[i];i++){
        if(bytes[i] == 1 ){
            i++;
        }
        len++;
    }
    byteLen = i+1;
    char* str = new char[len];
    for(i=0,j=0;i<byteLen;i++,j++){
        if(bytes[i] == 1){
            str[j] = bytes[++i]-1;
        }
        else
            str[j] = bytes[i];
    }
    value = str;
}


uint32_t StringRecord::_lengthInBytes() {
    uint32_t ret = len+1;
    for(uint32_t i=0;i<len;i++){
        if(value[i] == 0 || value[i] == 1){
            ret++;
        }
    }
    return ret;
}

uint32_t StringRecord::lengthInBytes() {
    return byteLen;
}

uint32_t StringRecord::convertToBytes(uint8_t * bytes) {
    uint32_t l = 0;
    for(uint32_t i=0;i<len;i++){
        if(value[i] == 0 || value[i] == 1){
            bytes[l++] = 1;
            bytes[l++] = value[i]+1;
        }
        else
            bytes[l++] = value[i];
    }
    bytes[l++] = 0;
    return l;
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
    value= bigEndianByteToInt32(byte)+(1<<31);
}

uint32_t IntRecord::convertToBytes(uint8_t * bytes) {
    bigEndianInt32ToBytes(value+(1<<31), bytes);
    return lengthInBytes();
}

uint32_t IntRecord::lengthInBytes() {
    return 4;
}

RecordType IntRecord::getType() {
    return RECORD_INT;
}

int IntRecord::toInt() const {
    return (int)value;
}

std::string IntRecord::toString() {
    return std::to_string(value);
}

TableDef::TableDef(uint8_t * key , uint8_t * val) {
    prefix = littleEndianByteToInt32(val);
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
    uint16_t numIndexes = littleEndianByteToInt16(val+offset);
    indexes.resize(numIndexes);
    offset+=2;
    for(int i=0;i<(int)numIndexes;i++){
        uint16_t numCols = littleEndianByteToInt16(val+offset);
        offset+=2;
        for(int j=0;j<(int)numCols;j++){
            StringRecord temp(val+offset);
            offset += temp.lengthInBytes();
            indexes[i].push_back(temp.toString());
        }
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
    ret+=2;
    for(auto & index : indexes){
        ret+=2;
        for(const auto & j : index){
            ret+=StringRecord(j.c_str(), j.length()).lengthInBytes();
        }
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
    littleEndianInt16ToBytes(indexes.size(), bytes.data() + offset);
    offset += 2;
    for(auto & index : indexes){
        littleEndianInt16ToBytes(index.size(), bytes.data() + offset);
        offset += 2;
        for(const auto & j : index){
            offset += StringRecord(j.c_str(), j.length()).convertToBytes(bytes.data()+offset);
        }
    }
    return bytes;
}

std::vector<uint8_t> TableDef::getKey() const {
    std::vector<uint8_t> bytes(keyLength());
    IntRecord(1).convertToBytes(bytes.data());
    StringRecord(name.c_str(), name.length()).convertToBytes(bytes.data()+4);
    return bytes;
}

void TableDef::pushColumn(std::string col, RecordType type) {
    columnNames.push_back(std::move(col));
    types.push_back(type);
}

void TableDef::addIndex(std::vector<std::string> &index) {
    indexes.push_back(index);
}

bool TableDef::operator==(const TableDef &rhs) const {
    return (&rhs == this) || (prefix == rhs.prefix &&
           pKey == rhs.pKey &&
           types == rhs.types &&
           columnNames == rhs.columnNames &&
           name == rhs.name);
}

bool TableDef::checkSanity() {
    for(auto &it1: indexes){
        for(auto &it2: it1){
            if(std::find(columnNames.begin(), columnNames.end(), it2) == columnNames.end()){
                return false;
            }
        }
    }
    return columnNames.size() >= pKey;
}

int TableDef::getIndexSize() const{
    return (int)indexes.size();
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
            value.push_back(new StringRecord(data+offset));
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
            value.push_back(new StringRecord(data+offset));
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
   IntRecord(tableDef.prefix).convertToBytes(bytes.data());
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