#include "convert.h"

#include "string.h"

/*
 * @函数名  ConvertUInt8ArrToUInt16
 * @用  途  uint8_t转uint16_t
 * @参  数  arr:uint8_t类型数组(2字节长度)
 *          type:大小端
 * @返回值  转换后的uint16_t
*/
uint16_t ConvertUInt8ArrToUInt16(uint8_t *arr, CONVERT_END_TYPE_t type)
{
    uint32_t ret = 0;

    if(CONVERT_LITTLE_END == type)
    {
        ret = (uint32_t)arr[0] | ((uint32_t)arr[1] << 8);
    }
    else if(CONVERT_BIG_END == type)
    {
        ret = ((uint32_t)arr[0] << 8) | (uint32_t)arr[1];
    }
    
    return ret;
}

/*
 * @函数名  ConvertUInt8ArrToUInt32
 * @用  途  uint8_t转uint32_t
 * @参  数  arr:uint8_t类型数组(4字节长度)
 *          type:大小端
 * @返回值  转换后的uint32_t
*/
uint32_t ConvertUInt8ArrToUInt32(uint8_t *arr, CONVERT_END_TYPE_t type)
{
    uint32_t ret = 0;

    if(CONVERT_LITTLE_END == type)
    {
        ret = ((uint32_t)arr[0] | ((uint32_t)arr[1] << 8) | ((uint32_t)arr[2] << 16) | ((uint32_t)arr[3] << 24));
    }
    else if(CONVERT_BIG_END == type)
    {
        ret = ((uint32_t)arr[0] << 24) | ((uint32_t)arr[1] << 16) | ((uint32_t)arr[2] << 8) | ((uint32_t)arr[3]);
    }
    
    return ret;
}

/*
 * @函数名  ConvertStringToHex
 * @用  途  字符串转16进制值数组
 * @参  数  str:字符串
 *          strLen:字符串长度
 *          hex:转换后的数组
 * @返回值  FALSE:字符串格式并非16进制
 *          TRUE:转换成功
*/
bool ConvertStringToHex(char *str, int strLen, uint8_t *hex)
{
    char c;
    char *p;
    char *pEnd = str+strLen;
    int hexIndex = 0;
    uint8_t temp = 0;

    for(p = str;(p < pEnd) && ('\0' != *p);++p)
    {
        c = *p;
        temp <<= 4;

        if((c >= '0') && (c <= '9'))
        {
            temp |= c - '0';
        }
        else if((c >= 'a') && (c <= 'f'))
        {
            temp |= c - 'a' + 10;
        }
        else if((c >= 'A') && (c <= 'F'))
        {
            temp |= c - 'A' + 10;
        }
        else
        {
            return FALSE;
        }

        ++hexIndex;
        if(0 == (hexIndex & 1))
        {
            *hex++ = temp;
        }
    }
    
    return TRUE;
}

/*
 * @函数名  ConvertStringToHex
 * @用  途  16进制值转字符串
 * @参  数  hex:输入数组
 *          hexLen:输入数组长度
 *          str:转换后的字符串
 * @返回值  
*/
void ConvertHexToString(uint8_t *hex, int hexLen, char *str)
{
    uint8_t *p;
    uint8_t *pEnd = hex + hexLen;

    const char *const convertTable = "0123456789ABCDEF";

    for(p = hex;p < pEnd;++p)
    {
        *str++ = convertTable[((*p) >> 4) & 0xF];
        *str++ = convertTable[(*p) & 0xF];
    }
}