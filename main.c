#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "crc/crc.h"


#define GENERATE_CRC_TABLE_DIR "generate_table/"
#define GENERATE_CRC_TABLE_C_FILE_NAME "crc_table.c"
#define GENERATE_CRC_TABLE_H_FILE_NAME "crc_table.h"


CRC_t CRC8;
CRC_t CRC16;
CRC_t CRC32;

uint8_t CRC8Table[256] = {0};
uint16_t CRC16Table[256] = {0};
uint32_t CRC32Table[256] = {0};

/*
 * @函数名  ReversalBit
 * @用  途  反转数据位
 * @参  数  value:需要反转的原始值
 *          bitWide:原始值的位宽
 * @返回值  反转后的值
*/
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

/*
 * @函数名  PolyPrintf
 * @用  途  在终端打印多项式
 * @参  数  poly:多项式的值
 * @返回值  
*/
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

/*
 * @函数名  CRCInfoPrintf
 * @用  途  在终端打印CRC相关信息
 * @参  数  crc:CRC结构体指针
 * @返回值  
*/
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

/*
 * @函数名  Test
 * @用  途  测试用
 * @参  数  
 * @返回值  
*/
void Test()
{
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
    CRCInit(&CRC8, bitWide, poly, isLSB);
    CRCGenerateTable(&CRC8, CRC8Table);
    CRCStart(&CRC8, initValue);
    CRCUpdate(&CRC8, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&CRC8, xorOutValue);//0xCB

    CRCInfoPrintf(&CRC8);
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
    CRCInit(&CRC16, bitWide, poly, isLSB);
    CRCGenerateTable(&CRC16, CRC16Table);
    CRCStart(&CRC16, initValue);
    CRCUpdate(&CRC16, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&CRC16, xorOutValue);//0xA455

    
    CRCInfoPrintf(&CRC16);
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
    CRCInit(&CRC32, bitWide, poly, isLSB);
    CRCGenerateTable(&CRC32, CRC32Table);
    CRCStart(&CRC32, initValue);
    CRCUpdate(&CRC32, (uint8_t *)str, strlen(str));
    crcValue = CRCGetCheckValue(&CRC32, xorOutValue);//0xCBF53A1C
    
    
    CRCInfoPrintf(&CRC32);
    printf("init:0x%x\n", initValue);
    printf("xor out:0x%x\n", xorOutValue);
    printf("in data:%s\n", str);
    printf("check value:0x%x\n", crcValue);
    printf("\n");

}

/*
 * @函数名  GenerateCRCTableArrName
 * @用  途  生成CRC表的数组名
 * @参  数  crc:CRC结构体指针
 *          tableArrName:生成好的数组名
 * @返回值  
*/
void GenerateCRCTableArrName(CRC_t * crc, char * tableArrName)
{
    bool isLSB;
    uint8_t bitWide;
    uint8_t arrBitWide;
    uint32_t poly;

    isLSB = crc->IsLSB;
    bitWide = crc->BitWide;
    poly = crc->Poly;

    if(bitWide <= 8)
    {
        arrBitWide = 8;
    }
    else if(bitWide <= 16)
    {
        arrBitWide = 16;
    }
    else
    {
        arrBitWide = 32;
    }

    if(FALSE != isLSB)//isLSB==TRUE
    {
        poly = ReversalBit(poly,bitWide);
        sprintf(tableArrName, "const uint%d_t CRCTableLSB0x%x[256]", arrBitWide, poly);
    }
    else
    {
        sprintf(tableArrName, "const uint%d_t CRCTableMSB0x%x[256]", arrBitWide, poly);
    }
}

/*
 * @函数名  GenerateCRCTableArrDataText
 * @用  途  生成CRC表数组数据部分的文本
 * @参  数  crc:CRC结构体指针
 *          tableArrDataText:生成好的数组数据文本
 * @返回值  
*/
void GenerateCRCTableArrDataText(CRC_t * crc, char * tableArrDataText)
{
    uint32_t i;
    uint8_t bitWide;
    void * table = NULL;

    bitWide = crc->BitWide;
    table = (void *)crc->CRCTable;

    for(i = 0;i < 256;++i)
    {
        if((i & 0x7) == 0)
        {
            sprintf(tableArrDataText, "\r\n\t");
            tableArrDataText += 3;
        }
        if(bitWide <= 8)
        {
            sprintf(tableArrDataText, "0x%02x,", ((uint8_t *)(table))[i]);
            tableArrDataText += 5;
        }
        else if(bitWide <= 16)
        {
            sprintf(tableArrDataText, "0x%04x,", ((uint16_t *)(table))[i]);
            tableArrDataText += 7;
        }
        else
        {
            sprintf(tableArrDataText, "0x%08x,", ((uint32_t *)(table))[i]);
            tableArrDataText += 11;
        }
    }

    sprintf(tableArrDataText, "\r\n");


}

/*
 * @函数名  GenerateCRCTableText
 * @用  途  生成CRC表数组的文本
 * @参  数  crc:CRC结构体指针
 *          text:生成好的数组文本
 * @返回值  
*/
void GenerateCRCTableText(CRC_t * crc, char * text)
{
    char tableArrName[64];
    char tableArrDataText[3072];
    
    GenerateCRCTableArrName(crc, tableArrName);
    GenerateCRCTableArrDataText(crc, tableArrDataText);

    sprintf(text, "%s = \r\n{%s};\r\n\r\n", tableArrName, tableArrDataText);

}

/*
 * @函数名  GenerateCRCTableFile
 * @用  途  生成存放CRC表数组的文件
 * @参  数  
 * @返回值  
*/
void GenerateCRCTableFile()
{
    FILE * f;

    char * tableDIR = GENERATE_CRC_TABLE_DIR;
    char * tableCFilePath = GENERATE_CRC_TABLE_DIR
                            GENERATE_CRC_TABLE_C_FILE_NAME;
    char * tableHFilePath = GENERATE_CRC_TABLE_DIR
                            GENERATE_CRC_TABLE_H_FILE_NAME;
    char tableArrName[64];
    char tableArrText[4096];

#if(defined(Linux))
    mkdir(tableDIR, 0777);
#elif(defined(Windows))
    mkdir(tableDIR);
#endif

    f = fopen(tableCFilePath, "w");

    if(NULL == f)
    {
        printf("未能创建文件%s,请检查文件路径\n", tableCFilePath);
        return;
    }

    fprintf(f, "#include \"%s\"\r\n\r\n", GENERATE_CRC_TABLE_H_FILE_NAME);
    
    GenerateCRCTableText(&CRC8, tableArrText);
    fprintf(f, "%s", tableArrText);
    
    GenerateCRCTableText(&CRC16, tableArrText);
    fprintf(f, "%s", tableArrText);
    
    GenerateCRCTableText(&CRC32, tableArrText);
    fprintf(f, "%s", tableArrText);

    fclose(f);

    f = fopen(tableHFilePath, "w");
    
    if(NULL == f)
    {
        printf("未能创建文件%s,请检查文件路径", tableHFilePath);
    }

    fprintf(f, "#ifndef __CRC_TABLE_H_\r\n");
    fprintf(f, "#define __CRC_TABLE_H_\r\n");
    fprintf(f, "\r\n");

    fprintf(f, "#include \"crc.h\"\r\n\r\n");

    GenerateCRCTableArrName(&CRC8, tableArrName);
    fprintf(f, "extern %s;\r\n", tableArrName);

    GenerateCRCTableArrName(&CRC16, tableArrName);
    fprintf(f, "extern %s;\r\n", tableArrName);

    GenerateCRCTableArrName(&CRC32, tableArrName);
    fprintf(f, "extern %s;\r\n", tableArrName);

    fprintf(f, "\r\n");
    
    fprintf(f, "#endif /*__CRC_TABLE_H_*/"); 
    
    fprintf(f, "\r\n\r\n");
    
    fclose(f);
}


int main(void)
{
    Test();
    GenerateCRCTableFile();
    return 0;
}
