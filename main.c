#include <stdio.h>
#include <string.h>

#include "crc/crc.h"

uint8_t CRC8Table[256] = {0};
unsigned short CRC16Table[256] = {0};
uint32_t CRC32Table[256] = {0};

uint32_t ReversalBit(uint32_t value, uint8_t bitWide)
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

void PolyPrintf(uint32_t poly)
{
    uint32_t i;

    printf("poly:");

    if(poly <= 0xff)
    {
        printf("x^8");
    }
    else if(poly <= 0xffff)
    {
        printf("x^16");
    }
    else
    {
        printf("x^32");
    }

    for(i = 0;i < 31;++i)
    {
        if(poly & 0x80000000)
        {
            printf("+x^%d",31-i);
        }
        poly <<= 1;
    }

    if(poly & 0x80000000)
    {
        printf("+1");
    }

    printf("\n");
    
}

void CRCInfoPrintf(CRC_t * crc)
{
    uint32_t poly;
    uint8_t bitWide;
    bool isLSB;

    poly = crc->Poly;
    bitWide = crc->BitWide;
    isLSB = crc->IsLSB;


    printf("CRC-%d\n", bitWide);
    if(FALSE != isLSB)
    {
        printf("LSB\n");
        poly = ReversalBit(poly, bitWide);
        PolyPrintf(poly);
    }
    else
    {
        printf("MSB\n");
        PolyPrintf(poly);
    }
}

void Test()
{
    CRC_t crc;
    uint32_t poly;
    uint8_t bitWide;
    bool isLSB;
    uint32_t initValue;
    uint32_t xorOutValue;
    const char * str = "12345";

    uint32_t crcValue;

    printf("/*****Test*****/\n\n");

    poly = 0x07;
    bitWide = 8;
    isLSB = FALSE;
    initValue = 0x00;
    xorOutValue = 0x00;
    CRCInit(&crc, bitWide, poly, isLSB);
    CRCGenerateTable(&crc, CRC8Table);
    CRCStart(&crc, initValue);
    CRCUpdate(&crc, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&crc, xorOutValue);

    CRCInfoPrintf(&crc);
    printf("init:0x%x\n", initValue);
    printf("xor out:0x%x\n", xorOutValue);
    printf("in data:%s\n", str);
    printf("check value:0x%x\n", crcValue);
    printf("\n");

    poly = 0x8005;
    bitWide = 16;
    isLSB = TRUE;
    initValue = 0x00;
    xorOutValue = 0x00;
    CRCInit(&crc, bitWide, poly, isLSB);
    CRCGenerateTable(&crc, CRC16Table);
    CRCStart(&crc, initValue);
    CRCUpdate(&crc, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&crc, xorOutValue);

    
    CRCInfoPrintf(&crc);
    printf("init:0x%x\n", initValue);
    printf("xor out:0x%x\n", xorOutValue);
    printf("in data:%s\n", str);
    printf("check value:0x%x\n", crcValue);
    printf("\n");

    poly = 0x04C11DB7;
    bitWide = 32;
    isLSB = TRUE;
    initValue = 0xffffffff;
    xorOutValue = 0xffffffff;
    CRCInit(&crc, bitWide, poly, isLSB);
    CRCGenerateTable(&crc, CRC32Table);
    CRCStart(&crc, initValue);
    CRCUpdate(&crc, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&crc, xorOutValue);    
    
    
    CRCInfoPrintf(&crc);
    printf("init:0x%x\n", initValue);
    printf("xor out:0x%x\n", xorOutValue);
    printf("in data:%s\n", str);
    printf("check value:0x%x\n", crcValue);
    printf("\n");

    poly = 0x04C11DB7;
    bitWide = 32;
    isLSB = FALSE;
    initValue = 0xffffffff;
    xorOutValue = 0x00000000;
    CRCInit(&crc, bitWide, poly, isLSB);
    CRCGenerateTable(&crc, CRC32Table);
    CRCStart(&crc, initValue);
    CRCUpdate(&crc, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&crc, xorOutValue);    
    
    
    CRCInfoPrintf(&crc);
    printf("init:0x%x\n", initValue);
    printf("xor out:0x%x\n", xorOutValue);
    printf("in data:%s\n", str);
    printf("check value:0x%x\n", crcValue);
    printf("\n");


}


int main(void)
{
    Test();
	return 0;
}
