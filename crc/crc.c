#include "crc.h"

typedef uint32_t (*CRCUpdateByteFunc_t)(void *table, uint32_t checksum, uint8_t *data, uint32_t len);

/*
static uint8_t ReversalBit8(uint8_t value)
{
    value = ((value << 4) & 0xf0) | ((value >> 4) & 0x0f);
    value = ((value << 2) & 0xcc) | ((value >> 2) & 0x33);
    value = ((value << 1) & 0xaa) | ((value >> 1) & 0x55);

    return value;
}
*/

/*
 * @函数名  ReversalBit
 * @用  途  反转数据位(内部调用)
 * @参  数  value:需要反转的原始值
 *          bitWide:原始值的位宽
 * @返回值  反转后的值
*/
static uint32_t ReversalBit(uint32_t value, uint8_t bitWide)
{
    uint32_t i;
    uint32_t temp = 0;

    for(i = 0;i < bitWide;++i)
    {
        temp <<= 1;
        temp |= value & 0x1;
        value >>= 1;
    }

    return temp;
}

/*
 * @函数名  CRCCalculateTableValue
 * @用  途  计算CRC表中的一个元素的值(内部调用)
 * @参  数  crc:crc结构体指针
 *          index:数组下标
 * @返回值  CRC表中元素的值
*/
static uint32_t CRCCalculateTableValue(CRC_t *crc, uint8_t index)
{
    uint32_t i;
    bool isLSB;
    uint8_t bitWide;
    uint32_t andValue;
    uint32_t crcTableValue;
    uint32_t poly;

    isLSB = crc->IsLSB;
    bitWide = crc->BitWide;
    poly = crc->Poly;
    crcTableValue = index;

    if(FALSE != isLSB)//isLSB==TRUE
    {
        poly = ReversalBit(poly, bitWide);
        andValue = 0x1;
    }
    else
    {
        andValue = 1 << (bitWide - 1);
        if(bitWide > 8)
        {
            crcTableValue <<= bitWide - 8;
        }
    }

    for(i = 0;i < 8;++i)
    {
        if(TRUE == crc->IsLSB)
        {
            crcTableValue = (crcTableValue & andValue) ? ((crcTableValue >> 1) ^ poly) : (crcTableValue >> 1);
        }
        else
        {
            crcTableValue = (crcTableValue & andValue) ? ((crcTableValue << 1) ^ poly) : (crcTableValue << 1);
        }
    }

    return crcTableValue;
}

/*
 * @函数名  CRC8UpdateByte
 * @用  途  CRC8计算更新一个字节(内部调用)
 * @参  数  checksum:更新前CRC校验值
 *          data:需要校验的原始数据
 *          table:CRC表的指针
 * @返回值  更新后的CRC校验值
*/
static uint32_t CRC8UpdateByte(void *table, uint32_t checksum, uint8_t *data, uint32_t len)
{
    uint32_t i;

    for(i = 0;i < len;++i)
    {
        checksum ^= data[i];
        checksum = ((uint8_t *)(table))[checksum & 0xff];
    }
    
    return checksum;
}

/*
 * @函数名  CRC16MSBUpdateByte
 * @用  途  CRC16MSB计算更新一个字节(内部调用)
 * @参  数  checksum:更新前CRC校验值
 *          data:需要校验的原始数据
 *          table:CRC表的指针
 * @返回值  更新后的CRC校验值
*/
static uint32_t CRC16MSBUpdateByte(void *table, uint32_t checksum, uint8_t *data, uint32_t len)
{
    uint32_t i;

    for(i = 0;i < len;++i)
    {
        checksum ^= (data[i] << 8) & 0xff00;
        checksum = ((uint16_t *)(table))[(checksum >> 8) & 0xff] ^ ((checksum << 8) & 0xff00);
    }

    return checksum;
}

/*
 * @函数名  CRC16LSBUpdateByte
 * @用  途  CRC16LSB计算更新一个字节(内部调用)
 * @参  数  checksum:更新前CRC校验值
 *          data:需要校验的原始数据
 *          table:CRC表的指针
 * @返回值  更新后的CRC校验值
*/
static uint32_t CRC16LSBUpdateByte(void *table, uint32_t checksum, uint8_t *data, uint32_t len)
{
    uint32_t i;

    for(i = 0;i < len;++i)
    {
        checksum ^= data[i];
        checksum = ((uint16_t *)(table))[checksum & 0xff] ^ ((checksum >> 8) & 0x00ff);
    }

    return checksum;
}

/*
 * @函数名  CRC32MSBUpdateByte
 * @用  途  CRC32MSB计算更新一个字节(内部调用)
 * @参  数  checksum:更新前CRC校验值
 *          data:需要校验的原始数据
 *          table:CRC表的指针
 * @返回值  更新后的CRC校验值
*/
static uint32_t CRC32MSBUpdateByte(void *table, uint32_t checksum, uint8_t *data, uint32_t len)
{
    uint32_t i;

    for(i = 0;i < len;++i)
    {
        checksum ^= (data[i] << 24) & 0xff000000;
        checksum = ((uint32_t *)(table))[(checksum >> 24)& 0xff] ^ ((checksum << 8) & 0xffffff00);
    }

    return checksum;
}

/*
 * @函数名  CRC32LSBUpdateByte
 * @用  途  CRC32LSB计算更新一个字节(内部调用)
 * @参  数  checksum:更新前CRC校验值
 *          data:需要校验的原始数据
 *          table:CRC表的指针
 * @返回值  更新后的CRC校验值
*/
static uint32_t CRC32LSBUpdateByte(void *table, uint32_t checksum, uint8_t *data, uint32_t len)
{
    uint32_t i;

    for(i = 0;i < len;++i)
    {
        checksum ^= data[i];
        checksum = ((uint32_t *)(table))[checksum & 0xff] ^ ((checksum >> 8) & 0x00ffffff);
    }

    return checksum;
}

/*
 * @函数名  CRCInit
 * @用  途  CRC结构体初始化
 * @参  数  crc:CRC结构体指针
 *          bitWide:CRC位宽(8,16,32)
 *          poly:多项式的值,这里不需要根据LSB和MSB进行手动反转
 *               例如x^8+x^2+x+1,这里poly都填0x07,CRCCalculateTableValue函数内部会根据LSB和MSB进行反转
 *          isLSB:TRUE为LSB,FALSE为MSB
 * @返回值  
*/
void CRCInit(CRC_t *crc, uint8_t bitWide, uint32_t poly, bool isLSB)
{
    if(crc == NULL)
    {
        return;
    }

    crc->BitWide = bitWide;
    crc->CRCTable = NULL;
    crc->IsLSB = isLSB;
}

/*
 * @函数名  CRCGenerateTable
 * @用  途  生成CRC的表
 *          如果不改变参数,该函数只需初始时候调用一次
 *          生成的表在RAM中,占较多的空间
 *          RAM较小的如单片机可以手动设置已经计算好的const类型的表,见CRCSetTable函数
 * @参  数  crc:CRC结构体指针
 *          void:CRC表的指针
 *               请在之前分配好内存
 *               8位需要256*1字节(例uint8_t Table[256];)
 *               16位需要256*2字节(例uint16_t Table[256];)
 *               32位需要256*4字节(例uint32_t Table[256];)
 * @返回值  
*/
void CRCGenerateTable(CRC_t *crc, void *table)
{
    uint32_t i;
    uint8_t bitWide;

    if((NULL == crc)
       && (NULL == table)
    )
    {
        return;
    }

    bitWide = crc->BitWide;

    for(i = 0;i < 256;++i)
    {
        if(bitWide <= 8)
        {
            ((uint8_t *)table)[i] = (uint8_t)CRCCalculateTableValue(crc, i);
        }
        else if(bitWide <= 16)
        {
            ((uint16_t *)table)[i] = (uint16_t)CRCCalculateTableValue(crc, i);
        }
        else
        {    
            ((uint32_t *)table)[i] = (uint32_t)CRCCalculateTableValue(crc, i);
        }
    }

    crc->CRCTable = table;
}

/*
 * @函数名  CRCSetTable
 * @用  途  设置CRC的表
 * @参  数  crc:CRC结构体指针
 *          void:已经计算好的CRC表的指针
 *               设置的表一定要与CRC的参数对应
 *               一般用于以及固定的const类型的表,见crc_table.c
 * @返回值  
*/
void CRCSetTable(CRC_t *crc, uint8_t *table)
{
    if((NULL == crc)
       && (NULL == table)
    )
    {
        return;
    }
    crc->CRCTable = table;
}

/*
 * @函数名  CRCStart
 * @用  途  CRC开始计算时,或重新计算时调用
 * @参  数  crc:CRC结构体指针
 *          initValue:CRC计算的初始值
 * @返回值  
*/
void CRCStart(CRC_t *crc, uint32_t initValue)
{
    crc->Checksum = initValue;
}

/*
 * @函数名  CRCUpdate
 * @用  途  CRC校验计算,不一定要一次算完,可以将数据分块,多次调用该函数进行计算
 * @参  数  crc:CRC结构体指针
 *          data:需要校验的原始数据
 *          len:原始数据的长度
 * @返回值  
*/
void CRCUpdate(CRC_t *crc, uint8_t *data, uint32_t len)
{
    bool isLSB;
    uint32_t checksum;
    void * table = NULL;
    uint8_t bitWide;
    CRCUpdateByteFunc_t crcUpdateByteFunc;

    if((NULL == crc)
      || (NULL == data)
    )
    {
        return;   
    }

    isLSB = crc->IsLSB;
    checksum = crc->Checksum;
    table = crc->CRCTable;
    bitWide = crc->BitWide;
    
    if(FALSE != isLSB)//isLSB==TRUE
    {
        if(bitWide <= 8)
        {
            crcUpdateByteFunc = &CRC8UpdateByte;
        }
        else if(bitWide <= 16)
        {
            crcUpdateByteFunc = &CRC16LSBUpdateByte;
        }
        else
        {
            crcUpdateByteFunc = &CRC32LSBUpdateByte;
        }
    }
    else
    {
        if(bitWide <= 8)
        {
            crcUpdateByteFunc = &CRC8UpdateByte;
        }
        else if(bitWide <= 16)
        {
            crcUpdateByteFunc = &CRC16MSBUpdateByte;
        }
        else
        {
            crcUpdateByteFunc = &CRC32MSBUpdateByte;
        }
    }

    checksum = crcUpdateByteFunc(table, checksum, data, len);

    crc->Checksum = checksum;
}

/*
 * @函数名  CRCGetChecksum
 * @用  途  获取当前CRC校验的值
 * @参  数  crc:CRC结构体指针
 *          xorOutValue:结果需要异或的值
 * @返回值  
*/
uint32_t CRCGetChecksum(CRC_t *crc, uint32_t xorOutValue)
{
    uint32_t checksum;

    checksum = crc->Checksum ^ xorOutValue;

    return checksum;
}
