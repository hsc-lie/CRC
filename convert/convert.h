#ifndef __CONVERT_H_
#define __CONVERT_H_

#include <stdint.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

#ifndef NULL
#define NULL ((void *)(0))
#endif

typedef unsigned char bool;

typedef enum
{
    CONVERT_LITTLE_END,         //大端
    CONVERT_BIG_END,            //小端
}CONVERT_END_TYPE_t;

extern uint16_t ConvertUInt8ArrToUInt16(uint8_t *arr, CONVERT_END_TYPE_t type);
extern uint32_t ConvertUInt8ArrToUInt32(uint8_t *arr, CONVERT_END_TYPE_t type);
extern bool ConvertStringToHex(char *str, int strLen, uint8_t *hex);
extern void ConvertHexToString(uint8_t *hex, int hexLen, char *str);

#endif /*__CONVERT_H_*/