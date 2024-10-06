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
    uint16_t result = littleEndianByteToInt16(bytes);

    mu_assert("error in littleEndianByteToInt16 cond 1" ,result == 0x0001);
    bytes[0] = 0xFF;
    bytes[1] = 0x20;
    result = littleEndianByteToInt16(bytes);
    mu_assert("error in littleEndianByteToInt16 cond 2", result == 0x20FF);
    return nullptr;
}

static const char * littleEndianInt16ToBytesTest(){
    uint16_t val = 0x0100;
    uint8_t byte[2] = {0x00, 0x00};
    littleEndianInt16ToBytes(val, byte);
    mu_assert("error in littleEndianInt16ToBytes cond 1", byte[0] == 0x00 && byte[1] == 0x01);
    return nullptr;
}


static const char* littleEndianByteToInt64Test(){
    uint8_t bytes[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint64_t result = littleEndianByteToInt64(bytes);
    mu_assert("error in littleEndianByteToInt64 cond 1", result == 0x0000000000000001);
    bytes[0] = 0xFF;
    bytes[1] = 0x20;
    bytes[2] = 0x00;
    bytes[3] = 0x00;
    bytes[4] = 0x00;
    bytes[5] = 0x00;
    bytes[6] = 0x00;
    bytes[7] = 0x00;
    result = littleEndianByteToInt64(bytes);
    mu_assert("error in littleEndianByteToInt64 cond 2", result == 0x00000000000020FF);
    return nullptr;
}

static const char * littleEndianInt64ToBytesTest(){
    uint64_t val = 0x0100000000000200;
    uint8_t byte[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    littleEndianInt64ToBytes(val, byte);
    mu_assert("error in littleEndianInt64ToBytes cond 1", byte[0] == 0x00 && byte[1] == 0x02 && byte[2] == 0x00 &&
    byte[3] == 0x00 && byte[4] == 0x00 && byte[5] == 0x00 && byte[6] == 0x00 && byte[7] == 0x01);
    return nullptr;
}

static const char * bigEndianInt32ToBytesTest(){
    uint32_t val = 0x01020304;
    uint8_t byte[4] = {0x00, 0x00, 0x00, 0x00};
    bigEndianInt32ToBytes(val, byte);
    mu_assert("error in bigEndianInt32ToBytes cond 1", byte[0] == 0x01 && byte[1] == 0x02 && byte[2] == 0x03 && byte[3] == 0x04);
    mu_assert("bigEndianByteToInt32", bigEndianByteToInt32(byte) == val);
    return nullptr;
}

static const char* bigEndianOrderTest(){
    int act = -342;
    uint32_t a = act;
    a+=1<<31;
    mu_assert("error in bigEndianOrderTest less than zero", a<1<<31);
    int x = a;
    x+=1<<31;
    mu_assert("error in bigEndianOrderTest conversion", x==act);

    int act2 = 342;
    uint32_t a2 = act2;
    a2+=1<<31;
    mu_assert("error in bigEndianOrderTest greater than zero", a2>1<<31);
    int x2 = a2;
    x2+=1<<31;
    mu_assert("error in bigEndianOrderTest conversion", x2==act2);
    return nullptr;
}

static const char * uintToIntTest(){
    int a = -2323,c;
    uint32_t b = a;
    c=b;
    mu_assert("values should be equal", c==a);
    return nullptr;
}

static const char* all_tests(){
    mu_run_test(littleEndianByteToInt16Test);
    mu_run_test(littleEndianInt16ToBytesTest);
    mu_run_test(littleEndianByteToInt64Test);
    mu_run_test(littleEndianInt64ToBytesTest);
    mu_run_test(bigEndianInt32ToBytesTest);
    mu_run_test(bigEndianOrderTest);
    mu_run_test(uintToIntTest);
    return nullptr;
}


int main(){

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