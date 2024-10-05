//
// Created by Aquib Nawaz on 03/10/24.
//

#ifndef SQLITEV2_TABLE_H
#define SQLITEV2_TABLE_H
#include <cstdint>
#include <string>
#include <vector>

enum RecordType {
    RECORD_INT = 0,
    RECORD_STRING = 1
};

class Record{
protected:
public:
    virtual uint32_t lengthInBytes()=0;
    virtual uint32_t convertToBytes(uint8_t *)=0;
    virtual RecordType getType()=0;
    virtual ~Record()=default;
};

class StringRecord: public Record{
    const char* value;
    uint32_t len;
public:
    StringRecord(const char* _value, uint32_t len);
    explicit StringRecord(uint8_t *);
    uint32_t lengthInBytes() override;
    uint32_t convertToBytes(uint8_t *) override;
    RecordType getType() override;
    std::string toString();
};

class IntRecord: public Record{
    int value;
public:
    explicit IntRecord(int value);
    uint32_t lengthInBytes() override;
    uint32_t convertToBytes(uint8_t *) override;
    RecordType getType() override;
    IntRecord(uint8_t *);
    int toInt() const;
};
class Row;
// | 4B prefix | 2B pKey| 2B numRecords | recordType | columnName
class TableDef {
    std::vector<RecordType> types;
    std::vector<std::string>columnNames;
    public:
    std::string  name;
    uint32_t prefix;
    uint16_t pKey;
    TableDef()=default;
    TableDef(uint8_t *, uint8_t *);
    bool operator==(const TableDef &rhs) const;
    uint32_t keyLength();
    uint32_t valueLength();
    void pushColumn(std::string col, RecordType type);
    std::vector<uint8_t> getKey();
    std::vector<uint8_t> getValue();
    bool checkSanity();
    friend class Row;
};

class Row{
    std::vector<std::string> cols;
    std::vector<Record*> value;
    uint32_t keyLength(TableDef &);
    uint32_t valueLength(TableDef &);
public:
    Row()=default;
    Row (std::vector<uint8_t> &key, std::vector<uint8_t> &val, TableDef &tableDef);
    Row (std::vector<std::string>, std::vector<Record*>);
    void pushRow(std::string col, Record* val);
    bool checkAndReorder(TableDef &tableDef);
    std::vector<uint8_t> getKey(TableDef &tableDef);
    std::vector<uint8_t> getValue(TableDef &tableDef);
    ~Row();

};


#endif //SQLITEV2_TABLE_H
