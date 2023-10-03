#include "crc.h"

/*
static uint8_t ReversalBit8(uint8_t value)
{
    value = ((value << 4) & 0xf0) | ((value >> 4) & 0x0f);
    value = ((value << 2) & 0xcc) | ((value >> 2) & 0x33);
    value = ((value << 1) & 0xaa) | ((value >> 1) & 0x55);

    return value;
}
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


static uint32_t CRCCalculateTableValue(CRC_t * crc, uint8_t index, uint32_t andValue)
{
    uint32_t i;
    bool isLSB;
    uint8_t bitWide;
    uint32_t crcTableValue;
    uint32_t poly;

    isLSB = crc->IsLSB;
    bitWide = crc->BitWide;
    poly = crc->Poly;
    crcTableValue = index;
    
    if(FALSE != isLSB)//isLSB==TRUE
    {
        andValue = 0x1;
    }
    else
    {
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

static uint32_t CRC8UpdateByte(uint32_t crcValue, uint8_t data, void * table)
{
    crcValue ^= data;
    crcValue = ((uint8_t *)(table))[crcValue & 0xff];
    return crcValue;
}

static uint32_t CRC16MSBUpdateByte(uint32_t crcValue, uint8_t data, void * table)
{
    crcValue ^= (data << 8) & 0xff00;
    crcValue = ((uint16_t *)(table))[(crcValue >> 8) & 0xff] ^ ((crcValue << 8) & 0xff00);
    return crcValue;
}


static uint32_t CRC16LSBUpdateByte(uint32_t crcValue, uint8_t data, void * table)
{
    crcValue ^= data;
    crcValue = ((uint16_t *)(table))[crcValue & 0xff] ^ ((crcValue >> 8) & 0x00ff);
    return crcValue;
}

static uint32_t CRC32MSBUpdateByte(uint32_t crcValue, uint8_t data, void * table)
{
    crcValue ^= (data << 24) & 0xff000000;
    crcValue = ((uint32_t *)(table))[(crcValue >> 24)& 0xff] ^ ((crcValue << 8) & 0xffffff00);
    return crcValue;
}

static uint32_t CRC32LSBUpdateByte(uint32_t crcValue, uint8_t data, void * table)
{
    crcValue ^= data;
    crcValue = ((uint32_t *)(table))[crcValue & 0xff] ^ ((crcValue >> 8) & 0x00ffffff);
    return crcValue;
}



void CRCInit(CRC_t * crc, uint8_t bitWide, uint32_t poly, bool isLSB)
{
    if(crc == NULL)
    {
        return;
    }

    crc->BitWide = bitWide;
    crc->CRCTable = NULL;
    crc->IsLSB = isLSB;

    if(FALSE != isLSB)//isLSB==TRUE
    {
        crc->Poly = ReversalBit(poly, bitWide);
    }
    else
    {
        crc->Poly = poly;
    }
}

void CRCGenerateTable(CRC_t * crc, void * table)
{
    uint32_t i;
    uint8_t bitWide;
    uint32_t andValue; 

    if((NULL == crc)
       && (NULL == table)
    )
    {
        return;
    }

    bitWide = crc->BitWide;
    andValue = 1 << (bitWide - 1);

    for(i = 0;i < 256;++i)
    {
        if(bitWide <= 8)
        {
            ((uint8_t *)table)[i] = (uint8_t)CRCCalculateTableValue(crc, i, andValue);
        }
        else if(bitWide <= 16)
        {
            ((uint16_t *)table)[i] = (uint16_t)CRCCalculateTableValue(crc, i, andValue);
        }
        else
        {    
            ((uint32_t *)table)[i] = (uint32_t)CRCCalculateTableValue(crc, i, andValue);
        }
    }

    crc->CRCTable = table;
}

void CRCSetTable(CRC_t * crc, uint8_t * table)
{
    if((NULL == crc)
       && (NULL == table)
    )
    {
        return;
    }
    crc->CRCTable = table;
}

void CRCStart(CRC_t * crc, uint32_t initValue)
{
    crc->CRCValue = initValue;
}

void CRCUpdate(CRC_t * crc, uint8_t * data, uint32_t len)
{
    uint32_t i;
    bool isLSB;
    uint32_t crcValue;
    void * table = NULL;
    uint8_t bitWide;

    uint32_t (*crcUpdateByteFunc)(uint32_t crcValue, uint8_t data, void * table);

    if((NULL == crc)
      || (NULL == data)
    )
    {
        return;   
    }

    isLSB = crc->IsLSB;
    crcValue = crc->CRCValue;
    table = (void *)crc->CRCTable;
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

    for(i = 0;i < len;++i)
    {
        crcValue = crcUpdateByteFunc(crcValue, data[i], table);
    }

    crc->CRCValue = crcValue;
}

uint32_t CRCGetCheckValue(CRC_t * crc, uint32_t xorOutValue)
{
    uint32_t checkValue;

    checkValue = crc->CRCValue ^ xorOutValue;

    return checkValue;
}
