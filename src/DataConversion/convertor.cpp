//
// Created by Aquib Nawaz on 21/09/24.
//

#include "convertor.h"
#include <cstring>


uint16_t bigEndianByteToInt16(const uint8_t bytes[]) {
    return (bytes[0] << 8) | bytes[1];
}

void bigEndianInt16ToBytes(uint16_t val, uint8_t bytes[]) {
    bytes[0] = val >> 8;
    bytes[1] = val & 0xFF;
}

uint64_t bigEndianByteToInt64(const uint8_t bytes[] ){
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

void littleEndianInt32ToBytes(uint32_t val, uint8_t bytes[]) {
    for(int i=3;i>=0;i--){
        bytes[i] = val & 0xFF;
        val = val >> 8;
    }
}

uint32_t littleEndianByteToInt32(const uint8_t bytes[]){
    uint32_t ret=0;
    for(int i=0;i<4;i++){
        ret = (ret<<8) | bytes[i];
    }
    return ret;
}

