//
// Created by Aquib Nawaz on 21/09/24.
//

#include "convertor.h"
#include <cstring>


uint16_t littleEndianByteToInt16(const uint8_t bytes[]) {
    return (bytes[0] << 8) | bytes[1];
}

void littleEndianInt16ToBytes(uint16_t val, uint8_t bytes[]) {
    bytes[0] = val >> 8;
    bytes[1] = val & 0xFF;
}

uint64_t littleEndianByteToInt64(const uint8_t bytes[]){
    uint64_t ret = 0;
    for(int i=0;i<8;i++){
        ret = (ret<<8) | bytes[i];
    }
    return ret;
}

void littleEndianInt64ToBytes(uint64_t val, uint8_t bytes[]){
    for(int i=7;i>=0;i--){
        bytes[i] = val & 0xFF;
        val = val >> 8;
    }
}

