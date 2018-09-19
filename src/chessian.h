#ifndef C_HESSIAN_H
#define C_HESSIAN_H
#include<stddef.h>

// auto expand buffer
struct CHBuffer{
    unsigned char* buffer;
    size_t buffer_len;
    int offset;
};

struct CHBuffer* chessian_createCHBuffer(size_t buffer_size);

void chessian_destoryCHBuffer(struct CHBuffer *buffer);

void chessian_resetCHBuffer(struct CHBuffer *buffer);

size_t chessian_writeBytes(struct CHBuffer *buffer,unsigned char *bytes,size_t bytes_len,int final);

size_t chessian_writeBoolean(struct CHBuffer *buffer,int b_v);

size_t chessian_writeDate(struct CHBuffer *buffer,long timestamp_v,int b_millis);

size_t chessian_writeDouble(struct CHBuffer *buffer,double d_v);

size_t chessian_writeInt(struct CHBuffer *buffer,int int_v);

size_t chessian_writeLong(struct CHBuffer *buffer,long long_v);

size_t chessian_writeNull(struct CHBuffer *buffer);

size_t chessian_writeString(struct CHBuffer *buffer,char* str_v);

#endif
