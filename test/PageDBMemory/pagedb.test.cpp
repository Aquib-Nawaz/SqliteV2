//
// Created by Aquib Nawaz on 22/09/24.
//

#include <iostream>
#include "../minunit.h"
#include "pagedb.h"
using namespace std;

int tests_run = 0;

static const char * PageDBInititalizationTest() {
    const char * fileName = "test.db";
    DiskPageDBMemory db(fileName);
    return nullptr;
}
static const char* all_tests(){
    mu_run_test(PageDBInititalizationTest);
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