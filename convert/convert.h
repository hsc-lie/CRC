#ifndef __CONVERT_H_
#define __CONVERT_H_

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)(0))
#endif

#if (!defined(FALSE) || !defined(TRUE))
#define FALSE 0
#define TRUE (!(FALSE))
#endif

typedef unsigned char bool;

typedef enum
{
    CONVERT_LITTLE_ENDIAN,         //大端
    CONVERT_BIG_ENDIAN,            //小端
}CONVERT_ENDIAN_TYPE_t;

extern uint16_t ConvertUInt8ArrToUInt16(uint8_t *arr, CONVERT_ENDIAN_TYPE_t type);
extern uint32_t ConvertUInt8ArrToUInt32(uint8_t *arr, CONVERT_ENDIAN_TYPE_t type);
extern void ConvertUInt32ToUInt8Arr(uint32_t value, uint8_t *arr, CONVERT_ENDIAN_TYPE_t type);
extern bool ConvertStringToHex(char *str, int strLen, uint8_t *hex);
extern void ConvertHexToString(uint8_t *hex, int hexLen, char *str);

#endif /*__CONVERT_H_*/