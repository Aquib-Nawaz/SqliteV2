//
// Created by Aquib Nawaz on 21/09/24.
//

#include "convertor.h"
#include <cstring>


uint16_t littleEndianByteToInt16(const uint8_t bytes[]) {
    uint16_t ret;
    memcpy(&ret, bytes, 2);
    return ret;
}

void littleEndianInt16ToBytes(uint16_t val, uint8_t bytes[]) {
    memcpy(bytes, &val, 2);
}

uint64_t littleEndianByteToInt64(const uint8_t bytes[]){
    uint64_t ret;
    memcpy(&ret, bytes, 8);
    return ret;
}

void littleEndianInt64ToBytes(uint64_t val, uint8_t bytes[]) {
    memcpy(bytes, &val, 8);
}

uint32_t littleEndianByteToInt32(const uint8_t bytes[]){
    uint32_t ret;
    memcpy(&ret, bytes, 4);
    return ret;
}

void littleEndianInt32ToBytes(uint32_t val, uint8_t bytes[]) {
    memcpy(bytes, &val, 4);
}