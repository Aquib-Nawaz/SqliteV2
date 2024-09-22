//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_CONVERTOR_H
#define SQLITEV2_CONVERTOR_H

#include <cstdint>

uint16_t littleEndianByteToInt16(const uint8_t[]);
void littleEndianInt16ToBytes(uint16_t, uint8_t[]);
uint64_t littleEndianByteToInt64(const uint8_t[]);
void littleEndianInt64ToBytes(uint64_t ,uint8_t[]);


#endif //SQLITEV2_CONVERTOR_H
