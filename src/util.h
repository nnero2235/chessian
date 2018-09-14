#ifndef C_HESSIAN_UTIL_H
#define C_HESSIAN_UTIL_H
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdarg.h>

#define LOG_DEBUG 1
#define LOG_INFO 2
#define LOG_ERROR 3

extern unsigned int util_logLevel;
extern unsigned int util_testNum;
extern unsigned int util_testFail;

static inline void putNowTime(char *timeStr) {
    if(timeStr == NULL){
        return;
    }
    time_t t;
    struct tm *localTime;
    t = time(NULL);
    localTime = localtime(&t);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", localTime);
}

static inline void putFormatLogStr(char *buffer,const char *format,...){
    if(buffer == NULL){
        return;
    }
    va_list ap;
    va_start(ap, format);
    vsprintf(buffer, format, ap);
    va_end(ap);
}


#define REAL_LOG(level, ...)                   \
    char *timeStr = malloc(sizeof(char) * 20);               \
    putNowTime(timeStr);                                     \
    char *msg = malloc(sizeof(char) * 1024);                 \
    putFormatLogStr(msg,__VA_ARGS__);                        \
    printf("[%s] - [%s] : %s\n", level, timeStr, msg); \
    free(timeStr);                                     \
    free(msg);

#define D_LOG(...)             \
    if (util_logLevel <= LOG_DEBUG)  \
    {                           \
        REAL_LOG("Debug", __VA_ARGS__); \
    }

#define I_LOG(...)              \
    if (util_logLevel <= LOG_INFO)  \
    {                           \
        REAL_LOG("Info", __VA_ARGS__); \
    }

#define E_LOG(...)              \
    if (util_logLevel <= LOG_ERROR)  \
    {                           \
        REAL_LOG("Error", __VA_ARGS__); \
    }

#define test_assert(msg, expr)             \
    do                                     \
    {                                      \
        util_testNum++;                         \
        printf("%d - %s", util_testNum, msg); \
        if (expr)                          \
            printf(" PASSED \n");          \
        else                               \
        {                                  \
            printf(" FAILED \n");          \
            util_testFail++;                        \
        }                                  \
    } while (0);

#define test_report()                                                           \
    do                                                                          \
    {                                                                           \
        if (util_testFail == 0)                                                          \
        {                                                                       \
            printf("Summary:%d tests -> All passed!\n", util_testNum);  \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            printf("Summary:%d tests -> passed\n", util_testNum - util_testFail);             \
            printf("======== WARN ========\nWe have %d tests failed!\n", util_testFail); \
        }                                                                       \
        util_testNum = 0; \
        util_testFail = 0; \
    } while (0);

void printByteArrayInHex(unsigned char arr[],size_t arr_size);


#endif
