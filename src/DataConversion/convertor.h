//
// Created by Aquib Nawaz on 21/09/24.
//

#ifndef SQLITEV2_CONVERTOR_H
#define SQLITEV2_CONVERTOR_H

#include <cstdint>

/**
 * \brief Converts a little-endian byte array to a 16-bit integer.
 * \param bytes Array of bytes in little-endian order.
 * \return 16-bit integer representation of the byte array.
 */
uint16_t littleEndianByteToInt16(const uint8_t bytes[]);

/**
 * \brief Converts a little-endian byte array to a 32-bit integer.
 * \param bytes Array of bytes in little-endian order.
 * \return 32-bit integer representation of the byte array.
 */
uint32_t littleEndianByteToInt32(const uint8_t bytes[]);

/**
 * \brief Converts a little-endian byte array to a 64-bit integer.
 * \param bytes Array of bytes in little-endian order.
 * \return 64-bit integer representation of the byte array.
 */
uint64_t littleEndianByteToInt64(const uint8_t bytes[]);

/**
 * \brief Converts a big-endian byte array to a 32-bit integer.
 * \param bytes Array of bytes in big-endian order.
 * \return 32-bit integer representation of the byte array.
 */
uint32_t bigEndianByteToInt32(const uint8_t bytes[]);

/**
 * \brief Converts a 16-bit integer to a little-endian byte array.
 * \param val 16-bit integer to convert.
 * \param bytes Array to store the resulting bytes.
 */
void littleEndianInt16ToBytes(uint16_t val, uint8_t bytes[]);

/**
 * \brief Converts a 32-bit integer to a little-endian byte array.
 * \param val 32-bit integer to convert.
 * \param bytes Array to store the resulting bytes.
 */
void littleEndianInt32ToBytes(uint32_t val, uint8_t bytes[]);

/**
 * \brief Converts a 64-bit integer to a little-endian byte array.
 * \param val 64-bit integer to convert.
 * \param bytes Array to store the resulting bytes.
 */
void littleEndianInt64ToBytes(uint64_t val, uint8_t bytes[]);

/**
 * \brief Converts a 32-bit integer to a big-endian byte array.
 * \param val 32-bit integer to convert.
 * \param bytes Array to store the resulting bytes.
 */
void bigEndianInt32ToBytes(uint32_t val, uint8_t bytes[]);

#endif //SQLITEV2_CONVERTOR_H