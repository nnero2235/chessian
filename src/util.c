#include"util.h"

unsigned int util_logLevel = LOG_DEBUG;
unsigned int util_testFail = 0;
unsigned int util_testNum = 0;

void printByteArrayInHex(unsigned char arr[],size_t arr_size){
    printf("buffer print:\n");
    for(int i=0;i<arr_size;i++){
        printf("0x%x ",arr[i]);
    }
    printf("\n");
}
