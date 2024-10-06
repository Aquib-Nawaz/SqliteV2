//
// Created by Aquib Nawaz on 22/09/24.
//

#include <iostream>
#include "convertor.h"
#include "../minunit.h"
using namespace std;

int tests_run = 0;

static const char* littleEndianByteToInt16Test(){

    uint8_t bytes[2] = {0x01, 0x00};
    uint16_t result = bigEndianByteToInt16(bytes);

    mu_assert("error in bigEndianByteToInt16 cond 1" ,result == 0x0100);
    bytes[0] = 0xFF;
    bytes[1] = 0x20;
    result = bigEndianByteToInt16(bytes);
    mu_assert("error in bigEndianByteToInt16 cond 2", result == 0xFF20);
    return nullptr;
}

static const char * littleEndianInt16ToBytesTest(){
    uint16_t val = 0x0100;
    uint8_t byte[2] = {0x00, 0x00};
    bigEndianInt16ToBytes(val, byte);
    mu_assert("error in bigEndianInt16ToBytes cond 1", byte[0] == 0x01 && byte[1] == 0x00);
    return nullptr;
}


static const char* littleEndianByteToInt64Test(){
    uint8_t bytes[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint64_t result = bigEndianByteToInt64(bytes);
    mu_assert("error in bigEndianByteToInt64 cond 1", result == 0x0100000000000000);
    bytes[0] = 0xFF;
    bytes[1] = 0x20;
    bytes[2] = 0x00;
    bytes[3] = 0x00;
    bytes[4] = 0x00;
    bytes[5] = 0x00;
    bytes[6] = 0x00;
    bytes[7] = 0x00;
    result = bigEndianByteToInt64(bytes);
    mu_assert("error in bigEndianByteToInt64 cond 2", result == 0xFF20000000000000);
    return nullptr;
}

static const char * littleEndianInt64ToBytesTest(){
    uint64_t val = 0x0100000000000200;
    uint8_t byte[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    littleEndianInt64ToBytes(val, byte);
    mu_assert("error in littleEndianInt64ToBytes cond 1", byte[0] == 0x01 && byte[1] == 0x00 && byte[2] == 0x00 &&
    byte[3] == 0x00 && byte[4] == 0x00 && byte[5] == 0x00 && byte[6] == 0x02 && byte[7] == 0x00);
    return nullptr;
}

static const char* all_tests(){
    mu_run_test(littleEndianByteToInt16Test);
    mu_run_test(littleEndianInt16ToBytesTest);
    mu_run_test(littleEndianByteToInt64Test);
    mu_run_test(littleEndianInt64ToBytesTest);
    return nullptr;
}


int main(int argc , char* argv[]){

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