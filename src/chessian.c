#include"chessian.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include"util.h"

//help double convert
union DoubleConverter{
    double d;
    char c[8];
};

//expand buffer when size is not enough
//do nothing when size is enough
static void expandCHBuffer(struct CHBuffer* buffer,size_t need_size){
    size_t avail = buffer->buffer_len - buffer->offset;
    if(avail < need_size){ //expand char[]
        unsigned char *old = buffer->buffer;
        size_t new_size = need_size-avail+buffer->buffer_len;
        D_LOG("buffer expand from:%d -> to:%d",buffer->buffer_len,new_size);
        unsigned char *new = realloc(old,sizeof(unsigned char) * new_size);
        buffer->buffer = new;
        buffer->buffer_len = new_size;
    }
}

struct CHBuffer* chessian_createCHBuffer(size_t buffer_size){
    struct CHBuffer *buffer = malloc(sizeof(struct CHBuffer));
    buffer->buffer_len = buffer_size;
    buffer->offset = 0;
    buffer->buffer = malloc(sizeof(unsigned char) * buffer_size);
    return buffer;
}

void chessian_destoryCHBuffer(struct CHBuffer *buffer){
    if(buffer != NULL){
        free(buffer->buffer);
        free(buffer);
        buffer = NULL;
    }
}

void chessian_resetCHBuffer(struct CHBuffer *buffer){
    if(buffer == NULL){
        return;
    }
    buffer->offset = 0;
}

//bytes_len must be less then 0xffff
//if length over 0xffff, high level should part them
size_t chessian_writeBytes(struct CHBuffer *buffer,unsigned char *bytes,size_t bytes_len,int final){
    if(buffer == NULL || bytes == NULL || bytes_len == 0){
        return -1;
    }
    if(bytes_len > 0xffff){
        return -2;
    }

    if(bytes_len == 1 && bytes[0] <= 15){
        expandCHBuffer(buffer,1);
        buffer->buffer[buffer->offset++] = bytes[0] + 0x20;
        return 1;
    }

    unsigned char leading_c = final ? 'B' : 'b';
    expandCHBuffer(buffer,bytes_len+3);
    buffer->buffer[buffer->offset++] = leading_c;
    buffer->buffer[buffer->offset++] = bytes_len >> 8;
    buffer->buffer[buffer->offset++] = bytes_len;
    unsigned char* buffer_ref = &buffer->buffer[buffer->offset];
    memcpy(buffer_ref,bytes,bytes_len);
    buffer->offset += bytes_len;
    return bytes_len+3;
}


size_t chessian_writeBoolean(struct CHBuffer *buffer,int b_v){
    if(buffer == NULL){
        return -1;
    }
    expandCHBuffer(buffer,1);
    if(b_v){
        buffer->buffer[buffer->offset++] = 'T';
    } else {
        buffer->buffer[buffer->offset++] = 'F';
    }
    return 1;
}

size_t chessian_writeDate(struct CHBuffer *buffer,long timestamp_v,int b_millis){
    if(buffer == NULL){
        return -1;
    }
    if(b_millis){
        expandCHBuffer(buffer,9);
        buffer->buffer[buffer->offset++] = 0x4a;
        buffer->buffer[buffer->offset++] = timestamp_v >> 56;
        buffer->buffer[buffer->offset++] = timestamp_v >> 48;
        buffer->buffer[buffer->offset++] = timestamp_v >> 40;
        buffer->buffer[buffer->offset++] = timestamp_v >> 32;
        buffer->buffer[buffer->offset++] = timestamp_v >> 24;
        buffer->buffer[buffer->offset++] = timestamp_v >> 16;
        buffer->buffer[buffer->offset++] = timestamp_v >> 8;
        buffer->buffer[buffer->offset++] = timestamp_v;
        return 9;
    } else {
        expandCHBuffer(buffer,5);
        buffer->buffer[buffer->offset++] = 0x4b;
        buffer->buffer[buffer->offset++] = timestamp_v >> 24;
        buffer->buffer[buffer->offset++] = timestamp_v >> 16;
        buffer->buffer[buffer->offset++] = timestamp_v >> 8;
        buffer->buffer[buffer->offset++] = timestamp_v;
        return 5;
    }
}

size_t chessian_writeDouble(struct CHBuffer *buffer,double d_v){
    if(buffer == NULL){
        return -1;
    }

    if(d_v == 0.0d){
        expandCHBuffer(buffer,1);
        buffer->buffer[buffer->offset++] = 0x5b;
        return 1;
    }
    if(d_v == 1.0d){
        expandCHBuffer(buffer,1);
        buffer->buffer[buffer->offset++] = 0x5c;
        return 1;
    }
    
    int i_v = d_v;
    if(i_v == d_v){ //means no fractional
        if(i_v >= -128 && i_v <= 127){
            expandCHBuffer(buffer,2);
            buffer->buffer[buffer->offset++] = 0x5d;
            buffer->buffer[buffer->offset++] = i_v;
            return 2;
        }
        if(i_v >= -32768 && i_v <= 32767){
            expandCHBuffer(buffer,3);
            buffer->buffer[buffer->offset++] = 0x5e;
            buffer->buffer[buffer->offset++] = i_v >> 8;
            buffer->buffer[buffer->offset++] = i_v;
            return 3;
        }
    }

    union DoubleConverter converter;
    converter.d = d_v;
    expandCHBuffer(buffer,9);
    buffer->buffer[buffer->offset++] = 'D';
    buffer->buffer[buffer->offset++] = converter.c[7];
    buffer->buffer[buffer->offset++] = converter.c[6];
    buffer->buffer[buffer->offset++] = converter.c[5];
    buffer->buffer[buffer->offset++] = converter.c[4];
    buffer->buffer[buffer->offset++] = converter.c[3];
    buffer->buffer[buffer->offset++] = converter.c[2];
    buffer->buffer[buffer->offset++] = converter.c[1];
    buffer->buffer[buffer->offset++] = converter.c[0];
    return 9;
}

size_t chessian_writeInt(struct CHBuffer *buffer,int int_v){
    if(buffer == NULL){
        return -1;
    }
    //compact: -16~47 can encode in 0x80 ~ 0xbf
    // value = code - 0x90
    if(int_v >= -16 && int_v <= 47){
        expandCHBuffer(buffer,1);
        buffer->buffer[buffer->offset++] = int_v + 0x90;
        return 1;
    }
    //compact: -2048 ~ 2047 can encode in leading byte range 0xc0 to 0xcf
    //         and need 2 bytes to store
    // value = ((code - 0xc8) << 8) + b0;
    if(int_v >= -2048 && int_v <= 2047){
        expandCHBuffer(buffer,2);
        buffer->buffer[buffer->offset++] = (int_v >> 8) + 0xc8;
        buffer->buffer[buffer->offset++] = int_v;
        return 2;
    }
    //compact: -262144 and 262143 can be encoded in three bytes with the leading byte in the range xd0 to xd7
    //         need 3 bytes to store
    // value = ((code-0xd4)<<16) + (b1<<8) + b0
    if(int_v >= -262144 && int_v <= 262143){
        expandCHBuffer(buffer,3);
        buffer->buffer[buffer->offset++] = (int_v >> 16) + 0xd4;
        buffer->buffer[buffer->offset++] = int_v >> 8;
        buffer->buffer[buffer->offset++] = int_v;
        return 3;
    }
    //compact: normal 'I' b3 b2 b1 b0
    //        need 5 bytes to store
    // value =  (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
    expandCHBuffer(buffer,5);
    buffer->buffer[buffer->offset++] = 'I';
    buffer->buffer[buffer->offset++] = int_v >> 24;
    buffer->buffer[buffer->offset++] = int_v >> 16;
    buffer->buffer[buffer->offset++] = int_v >> 8;
    buffer->buffer[buffer->offset++] = int_v;
    return 5;
}

size_t chessian_writeLong(struct CHBuffer* buffer,long long_v){
    if(buffer == NULL){
        return -1;
    }

    if(long_v >= -8 && long_v <= 15){
        expandCHBuffer(buffer,1);
        buffer->buffer[buffer->offset++] = long_v + 0xe0;
        return 1;
    }

    if(long_v >= -2048 && long_v <= 2047){
        expandCHBuffer(buffer,2);
        buffer->buffer[buffer->offset++] = (long_v >> 8) + 0xf8;
        buffer->buffer[buffer->offset++] = long_v;
        return 2;
    }

    if(long_v >= -262144 && long_v <= 262143){
        expandCHBuffer(buffer,3);
        buffer->buffer[buffer->offset++] = (long_v >> 16) + 0x3c;
        buffer->buffer[buffer->offset++] = long_v >> 8;
        buffer->buffer[buffer->offset++] = long_v;
        return 3;
    }

    if(long_v >= -2147483647 && long_v <= 2147483647){
        expandCHBuffer(buffer,5);
        buffer->buffer[buffer->offset++] = 0x4c;
        buffer->buffer[buffer->offset++] = long_v >> 24;
        buffer->buffer[buffer->offset++] = long_v >> 16;
        buffer->buffer[buffer->offset++] = long_v >> 8;
        buffer->buffer[buffer->offset++] = long_v;
        return 5;
    }

    expandCHBuffer(buffer,9);
    buffer->buffer[buffer->offset++] = 'L';
    buffer->buffer[buffer->offset++] = long_v >> 56;
    buffer->buffer[buffer->offset++] = long_v >> 48;
    buffer->buffer[buffer->offset++] = long_v >> 40;
    buffer->buffer[buffer->offset++] = long_v >> 32;
    buffer->buffer[buffer->offset++] = long_v >> 24;
    buffer->buffer[buffer->offset++] = long_v >> 16;
    buffer->buffer[buffer->offset++] = long_v >> 8;
    buffer->buffer[buffer->offset++] = long_v;
    return 9;
}

size_t chessian_writeNull(struct CHBuffer *buffer){
    if(buffer == NULL){
        return -1;
    }
    expandCHBuffer(buffer,1);
    buffer->buffer[buffer->offset++] = 'N';
    return 1;
}

//TODO: not finished
size_t chessian_writeString(struct CHBuffer *buffer,char* str_v){
    if(buffer == NULL){
        return -1;
    }
    size_t len = strlen(str_v) + 3;
    expandCHBuffer(buffer,len);
    
    
    return len;
}

