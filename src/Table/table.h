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
    virtual std::string toString()=0;
    virtual ~Record()=default;
};

class StringRecord: public Record{
    const char* value;
    bool copy;
    uint32_t len;
    uint32_t byteLen;
    uint32_t _lengthInBytes();
public:
    StringRecord(const char* _value, uint32_t len, bool copy=false);
    StringRecord(const uint8_t *);
    uint32_t lengthInBytes() override;
    uint32_t convertToBytes(uint8_t *) override;
    RecordType getType() override;
    std::string toString() override;
    ~StringRecord() override;
};

class IntRecord: public Record{
    uint32_t value;
public:
    explicit IntRecord(int value);
    explicit IntRecord(uint8_t *);
    uint32_t lengthInBytes() override;
    uint32_t convertToBytes(uint8_t *) override;
    RecordType getType() override;
    std::string toString() override;
    [[nodiscard]] int toInt() const;
};
class Row;
// | 4B prefix | 2B pKey| 2B numRecords | recordType | columnName
class TableDef {
    std::vector<RecordType> types;
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> indexes;
    public:
    std::string  name;
    int prefix;
    uint16_t pKey;
    TableDef()=default;
    TableDef(uint8_t *, uint8_t *);
    bool operator==(const TableDef &rhs) const;
    uint32_t keyLength() const;
    uint32_t valueLength();
    void pushColumn(std::string col, RecordType type);
    void addIndex(std::vector<std::string> &index);
    std::vector<uint8_t> getKey() const;
    std::vector<uint8_t> getValue();
    bool checkSanity();
    int getIndexSize()const;
    friend class Row;
};

class Row{

    uint32_t keyLength(TableDef &);
    uint32_t valueLength(TableDef &);
public:
    std::vector<std::string> cols;
    std::vector<Record*> value;
    Row()=default;
    Row (std::vector<uint8_t> &key, std::vector<uint8_t> &val, TableDef &tableDef);
    Row (std::vector<std::string>, std::vector<Record*>);
    void pushRow(std::string col, Record* val);
    bool checkAndReorder(TableDef &tableDef, bool isKey);
    void populateValue(TableDef &tableDef, std::vector<uint8_t> &val);
    std::vector<uint8_t> getKey(TableDef &tableDef);
    std::vector<uint8_t> getValue(TableDef &tableDef);
    ~Row();
};


#endif //SQLITEV2_TABLE_H
