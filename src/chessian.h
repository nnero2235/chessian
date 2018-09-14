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

size_t chessian_writeString(struct CHBuffer *buffer,char* str_v);

size_t chessian_writeInt(struct CHBuffer *buffer,int int_v);

size_t chessian_writeLong(struct CHBuffer *buffer,long long_v);

#endif
