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

void test_ch_writeBinary(){
    struct CHBuffer* b = chessian_createCHBuffer(10);

    unsigned char bin_data[10];
    memset(bin_data,0,10);

    bin_data[0] = 'a';
    bin_data[1] = 0xcc;
    bin_data[2] = 0x11;
    bin_data[3] = 0xbb;
    bin_data[4] = 5;
    bin_data[5] = '\n';
    bin_data[6] = '\0';

    size_t bytes = chessian_writeBytes(b,bin_data,7,1);
    test_assert("Test writeBytes:\"single final chunck\"",bytes == 10 &&
                b->buffer[0] == 'B' && b->buffer[1] == 0x00 &&
                b->buffer[2] == 0x07 && b->buffer[3] == 'a' &&
                b->buffer[4] == 0xcc &&
                b->buffer[5] == 0x11 && b->buffer[6] == 0xbb &&
                b->buffer[7] == 5 && b->buffer[8] == '\n' &&
                b->buffer[9] == 0);

    chessian_resetCHBuffer(b);
    bin_data[0] = 'c';
    bin_data[1] = 0x10;
    bin_data[2] = 'p';
    bin_data[3] = 'a';
    bin_data[4] = 'b';
    bin_data[5] = 'c';
    size_t b1 = chessian_writeBytes(b,bin_data,3,0);
    size_t b2 = chessian_writeBytes(b,&bin_data[3],3,1);
    test_assert("Test writeBytes:\"2 chunks\"", (b1 + b2 == 12) &&
                b->buffer[0] == 'b' && b->buffer[1] == 0 &&
                b->buffer[2] == 3 && b->buffer[3] == 'c' &&
                b->buffer[4] == 0x10 && b->buffer[5] == 'p' &&
                b->buffer[6] == 'B' && b->buffer[7] == 0 &&
                b->buffer[8] == 3 && b->buffer[9] == 'a' &&
                b->buffer[10] == 'b' && b->buffer[11] == 'c');
    chessian_destoryCHBuffer(b);
}

void test_ch_writeBoolean(){
    struct CHBuffer* b = chessian_createCHBuffer(10);

    size_t b1 = chessian_writeBoolean(b,0);
    size_t b2 = chessian_writeBoolean(b,1);
    test_assert("Test writeBoolean:\"false and true\"",(b1+b2 == 2) &&
                b->buffer[0] == 'F' && b->buffer[1] == 'T');
    
    chessian_destoryCHBuffer(b);
}

void test_ch_writeDate(){
    struct CHBuffer *b = chessian_createCHBuffer(10);

    size_t b1 = chessian_writeDate(b,1537237335000L,1);
    size_t b2 = chessian_writeDate(b,1537200000L,0);

    test_assert("Test writeDate:\"2018/9/18 10:22:15 and 2018/9/18 00:00:00\"",
                (b1+b2 == 14) && b->buffer[0] == 0x4a && b->buffer[1] == 0
                && b->buffer[2] == 0 && b->buffer[3] == 0x01
                && b->buffer[4] == 0x65 && b->buffer[5] == 0xea
                && b->buffer[6] == 0x7c
                && b->buffer[7] == 0x3b && b->buffer[8] == 0xd8
                && b->buffer[9] == 0x4b && b->buffer[10] == 0x5b
                && b->buffer[11] == 0x9f && b->buffer[12] == 0xcf
                && b->buffer[13] == 0x80);

    chessian_destoryCHBuffer(b);
}

void test_ch_writeDouble(){
    struct CHBuffer *b = chessian_createCHBuffer(10);

    size_t b1 = chessian_writeDouble(b,0.0f);
    size_t b2 = chessian_writeDouble(b,1.0f);

    test_assert("Test writeDouble:\"0.0 1.0\"",(b1+b2==2) && b->buffer[0] == 0x5b
                && b->buffer[1] == 0x5c);

    size_t b3 = chessian_writeDouble(b,100.0d);
    size_t b4 = chessian_writeDouble(b,-111.0d);

    test_assert("Test writeDouble:\"100.0 -111.0\"",(b3+b4==4) && b->buffer[2] == 0x5d
                && b->buffer[3] == 0x64 && b->buffer[4] == 0x5d
                && b->buffer[5] == 0x91);

    size_t b5 = chessian_writeDouble(b,-32768.0);
    size_t b6 = chessian_writeDouble(b,32767.0);

    test_assert("Test writeDouble:\"-32768.0 32767.0\"",(b5+b6 == 6) && b->buffer[6] == 0x5e
                && b->buffer[7] == 0x80 && b->buffer[8] == 0
                && b->buffer[9] == 0x5e && b->buffer[10] == 0x7f && b->buffer[11] == 0xff);

    union u{
        double d;
        unsigned char c[8];
    }u_t;
    size_t b7 = chessian_writeDouble(b,13789.6789d);
    u_t.d = 13789.6789d;
    test_assert("Test writeDouble:\"13789.6789\"",b7 == 9 && b->buffer[12] == 'D'
                && b->buffer[13] == u_t.c[7] && b->buffer[14] == u_t.c[6]
                && b->buffer[15] == u_t.c[5] && b->buffer[16] == u_t.c[4]
                && b->buffer[17] == u_t.c[3] && b->buffer[18] == u_t.c[2]
                && b->buffer[19] == u_t.c[1] && b->buffer[20] == u_t.c[0]);

    chessian_destoryCHBuffer(b);
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
    chessian_destoryCHBuffer(b);
}

void test_ch_writeLong(){
    struct CHBuffer *b = chessian_createCHBuffer(10);

    size_t b1 = chessian_writeLong(b,9);
    test_assert("Test writeLong:\"9\"",b1 == 1 && b->buffer[0] == 0xe9);
    b1 = chessian_writeLong(b,2043);
    test_assert("Test writeLong:\"2043\"",b1 == 2 && b->buffer[1] == 0xff
                && b->buffer[2] == 0xfb);
    b1 = chessian_writeLong(b,-252134);
    test_assert("Test writeLong:\"-252134\"",b1 == 3 && b->buffer[3] == 0x38
                && b->buffer[4] == 0x27
                && b->buffer[5] == 0x1a);
    b1 = chessian_writeLong(b,214748364700011);
    test_assert("Test writeLong:\"214748364700011\"",b1 == 9
                && b->buffer[6] == 'L' && b->buffer[7] == 0
                && b->buffer[8] == 0 && b->buffer[9] == 0xc3
                && b->buffer[10] == 0x4f && b->buffer[11] == 0xff
                && b->buffer[12] == 0xfe && b->buffer[13] == 0x79
                && b->buffer[14] == 0x6b);
    
    chessian_destoryCHBuffer(b);
}

void test_ch_writeNull(){
    struct CHBuffer *b = chessian_createCHBuffer(10);

    size_t b1 = chessian_writeNull(b);
    test_assert("Test writeNull",b1 == 1 && b->buffer[0] == 'N');
    
    chessian_destoryCHBuffer(b);
}


int main(){
    util_logLevel = LOG_INFO;
    test_ch_writeBinary();
    test_ch_writeBoolean();
    test_ch_writeDate();
    test_ch_writeDouble();
    test_ch_writeInt();
    test_ch_writeLong();
    test_ch_writeNull();
    test_report();

    return 0;
}
