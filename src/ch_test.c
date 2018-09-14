#include"chessian.h"
#include<string.h>
#include<stdio.h>
#include"util.h"

unsigned char BIG_BUFFER[1024];
int global_offset = 0;

void flushBuffer(struct CHBuffer* b){
    unsigned char* buffer_ref = &BIG_BUFFER[global_offset];
    memcpy(buffer_ref,b->buffer,b->offset);
    global_offset += b->offset;
    b->offset = 0;
}

void writeString(struct CHBuffer* b,char* str_v){
    flushBuffer(b);
    chessian_writeString(b,str_v);
}

void writeInt(struct CHBuffer* b,int int_v){
    flushBuffer(b);
    chessian_writeInt(b,int_v);
}

void test_ch_writeInt(){
    struct CHBuffer* b = chessian_createCHBuffer(10);
    size_t bytes = chessian_writeInt(b,0);
    test_assert("Test writeInt:\"0\"",bytes == 1 && b->buffer[0] == 0x90);
    bytes = chessian_writeInt(b,-16);
    test_assert("Test writeInt:\"-16\"",bytes == 1 && b->buffer[1] == 0x80);
    bytes = chessian_writeInt(b,-2048);
    test_assert("Test writeInt:\"-2048\"",bytes == 2 && b->buffer[2] == 0xc0 && b->buffer[3] == 0x00);
    bytes = chessian_writeInt(b,262143);
    test_assert("Test writeInt:\"262143\"",bytes == 3 && b->buffer[4] == 0xd7 && b->buffer[5] == 0xff && b->buffer[6] == 0xff);
    bytes = chessian_writeInt(b,167896554);
    test_assert("Test writeInt:\"167896554\"",bytes == 5 && b->buffer[7] == 0x49 &&
                b->buffer[8] == 0x0a && b->buffer[9] == 0x01 && b->buffer[10] == 0xe5 && b->buffer[11] == 0xea);
    test_report();
}

void test_ch_writeString(){
    struct CHBuffer* b = chessian_createCHBuffer(10);

    size_t bytes = chessian_writeString(b,"nnero");
    test_assert("Test writeString : \"nnero\"",bytes == 5);
}

int main(){

    test_ch_writeInt();
    return 0;
}
