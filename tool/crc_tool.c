#include "crc_tool.h"

#include <stdio.h>
#include <string.h>

#include "cmdl.h"

static CMDL_ERROR_t CRCToolPrintVersion(char *param);
static CMDL_ERROR_t CRCToolPrintHelp(char *param);
static CMDL_ERROR_t CRCToolSetLSB(char *param);
static CMDL_ERROR_t CRCToolSetMSB(char *param);
static CMDL_ERROR_t CRCToolSetWide(char *param);
static CMDL_ERROR_t CRCToolSetPoly(char *param);
static CMDL_ERROR_t CRCToolSetInitValue(char *param);
static CMDL_ERROR_t CRCToolSetXorValue(char *param);
static CMDL_ERROR_t CRCToolSetInHex(char *param);
static CMDL_ERROR_t CRCToolSetInString(char *param);
static CMDL_ERROR_t CRCToolSetInFile(char *param);
static CMDL_ERROR_t CRCToolSetOutBinFileLittleEndian(char *param);
static CMDL_ERROR_t CRCToolSetOutBinFileBigEndian(char *param);
static CMDL_ERROR_t CRCToolSetOutTxtFile(char *param);
static CMDL_ERROR_t CRCToolSetGenerateTable(char *param);


static CMDLParam_t CMDLParamTable[] = 
{
    //CRC参数设置
    {CMDL_PARAM_TYPE_SINGLE, 'l', "lsb", CRCToolSetLSB},
    {CMDL_PARAM_TYPE_SINGLE, 'm', "msb", CRCToolSetMSB},
    {CMDL_PARAM_TYPE_MULTIPLE, 'w', "wide", CRCToolSetWide},
    {CMDL_PARAM_TYPE_MULTIPLE, 'p', "poly", CRCToolSetPoly},
    {CMDL_PARAM_TYPE_MULTIPLE, '\0', "init", CRCToolSetInitValue},
    {CMDL_PARAM_TYPE_MULTIPLE, '\0', "xor", CRCToolSetXorValue},

    //输入参数设置
    {CMDL_PARAM_TYPE_MULTIPLE, 'h', "hex", CRCToolSetInHex},
    {CMDL_PARAM_TYPE_MULTIPLE, 's', "string", CRCToolSetInString},
    {CMDL_PARAM_TYPE_MULTIPLE, 'f', "file", CRCToolSetInFile},

    //输出参数设置
    {CMDL_PARAM_TYPE_MULTIPLE, '\0', "out-bin-little", CRCToolSetOutBinFileLittleEndian},
    {CMDL_PARAM_TYPE_MULTIPLE, '\0', "out-bin-big", CRCToolSetOutBinFileBigEndian},
    {CMDL_PARAM_TYPE_MULTIPLE, '\0', "out-txt", CRCToolSetOutTxtFile},

    //生成crc的表
    {CMDL_PARAM_TYPE_MULTIPLE, 'g', "generate-table", CRCToolSetGenerateTable},

    //版本及帮助
    {CMDL_PARAM_TYPE_SINGLE, 'v', "version", CRCToolPrintVersion},
    {CMDL_PARAM_TYPE_SINGLE, '\0', "help", CRCToolPrintHelp},

    //{CMDL_PARAM_TYPE_MULTIPLE, '\0', "", NULL},
};

static CMDL_t CMDL = 
{
    .NotFlagParamFunc = CRCToolSetInFile,
    .ParamTable = CMDLParamTable,
    .ParamTableLen = sizeof(CMDLParamTable)/sizeof(CMDLParam_t),
};

static CRCTool_t CRCTool;
static uint8_t CRC8Table[256] = {0};
static uint16_t CRC16Table[256] = {0};
static uint32_t CRC32Table[256] = {0};


/*
 * @函数名  PolyPrintf
 * @用  途  在终端打印多项式
 * @参  数  poly:多项式的值
 * @返回值  
*/
static void PolyPrintf(uint32_t poly)
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
 * @函数名  CRCPrintInfo
 * @用  途  打印CRC工具相关信息
 * @参  数  crc:CRC结构体指针
 * @返回值  
*/
static void CRCPrintInfo(CRC_t *crc)
{
    uint32_t poly = crc->Poly;
    uint8_t bitWide = crc->BitWide;
    CRC_FIRST_BIT_t firstBit = crc->FirstBit;

    printf("CRC-%d", bitWide);
    if(CRC_FIRST_BIT_LSB == firstBit)
    {
        printf("-LSB\n");
    }
    else
    {
        printf("-MSB\n");
        
    }
    PolyPrintf(poly);
}

/*
 * @函数名  CRCToolPrintHelp
 * @用  途  CRC工具打印帮助文档(命令行参数回调)
 * @参  数  param:未使用
 * @返回值
*/
static CMDL_ERROR_t CRCToolPrintHelp(char *param)
{
    printf("Usage: crc [OPTION]... [FILE]\n");
    printf("\t-l, --lsb               set lsb\n");
    printf("\t-m, --msb               set msb\n");
    printf("\t-w, --wide              set wide(example: -w 32)\n");
    printf("\t-p, --poly              set poly(example: -p 4C11DB7)\n");
    printf("\t--init                  set crc init value(example: --init FFFFFFFF)\n");
    printf("\t--xor                   set crc out xor value(example:--xor FFFFFFFF)\n");
    printf("\n");
    printf("\t-h, --hex               set hex input(example: -h 12345678)\n");
    printf("\t-s, --string            set string input(example: -s 123456)\n");
    printf("\t-f, --file              set file input(example: -f filename\n");
    printf("\n");
    printf("\t--out-bin-little        set output crc checksum to little-endian bin file\n");
    printf("\t                        example:--out-bin-little out.bin\n");
    printf("\t--out-bin-big           set output crc checksum to big-endian bin file\n");
    printf("\t                        example:--out-bin-big out.bin\n");
    printf("\t--out-txt               set output crc checksum to text file\n");
    printf("\t                        example:--out-txt out.txt\n");
    printf("\t-g, --generate-table    generate crc table(example:-g table.c)\n");
    printf("\n");
    printf("\t-v, --version           print version\n");
    printf("\t--help                  print help\n");

    return CMDL_OK;
}

/*
 * @函数名  CRCToolPrintVersion
 * @用  途  CRC工具打印版本号(命令行参数回调)
 * @参  数  param:未使用
 * @返回值
*/
static CMDL_ERROR_t CRCToolPrintVersion(char *param)
{
    printf("crc version ");
    printf(CRC_TOOL_VERSION);
    printf("\n");
    return CMDL_OK; 
}

/*
 * @函数名  CRCToolSetLSB
 * @用  途  CRC工具设置为小端(命令行参数回调)
 * @参  数  param:未使用
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetLSB(char *param)
{
    CRCTool.CRC.FirstBit = CRC_FIRST_BIT_LSB;
    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetMSB
 * @用  途  CRC工具设置为大端(命令行参数回调)
 * @参  数  param:未使用
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetMSB(char *param)
{
    CRCTool.CRC.FirstBit = CRC_FIRST_BIT_MSB;
    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetWide
 * @用  途  CRC工具设置位宽(命令行参数回调)
 * @参  数  str:多项式的字符串
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetWide(char *str)
{
    CMDL_ERROR_t ret = CMDL_ERROR_INVALID_PARAM;
    size_t len;

    len = strlen(str);

    if(1 == len)
    {
        if(*str == '8')
        {
            CRCTool.CRC.BitWide = 8;
            ret = CMDL_OK;
        }
    }
    else if(2 == len)
    {
        if((*str == '1') && (*(str+1) == '6'))
        {
            CRCTool.CRC.BitWide = 16;
            ret = CMDL_OK;
        }
        else if((*str == '3') && (*(str+1) == '2'))
        {
            CRCTool.CRC.BitWide = 32;
            ret = CMDL_OK;
        }
    }
    
    return ret;
}

/*
 * @函数名  CRCToolSetWide
 * @用  途  CRC工具通用16进制字符串转uint32_t
 * @参  数  str:字符串
 *          value:转换后的值
 * @返回值  FALSE:字符串长度或格式不正确
 *          TRUE:转换成功
*/
static bool CRCToolStringConvertToUInt32(char *str, uint32_t *value)
{
    uint8_t *p;
    uint8_t *pEnd;
    uint8_t polyArr[4];

    char strLen = strlen(str);

    if((strLen > 8) || (0 != (strLen & 1)))
    {
        return FALSE;
    }

    if(FALSE == ConvertStringToHex(str, strLen, polyArr))
    {
        return FALSE;
    }

    *value = 0;
    pEnd = polyArr + (strLen >> 1);
    for(p = polyArr;p < pEnd;++p)
    {
        *value <<= 8;
        *value |= *p;
    }

    return TRUE;
}

/*
 * @函数名  CRCToolSetPoly
 * @用  途  CRC工具设置多项式(命令行参数回调)
 * @参  数  str:多项式的字符串
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetPoly(char *str)
{
    if(FALSE == CRCToolStringConvertToUInt32(str, &CRCTool.CRC.Poly))
    {
        printf("poly error!:%s", str);
        return CMDL_ERROR_INVALID_PARAM;
    }

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetInitValue
 * @用  途  CRC工具设置计算初始值(命令行参数回调)
 * @参  数  str:异或值的字符串
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetInitValue(char *str)
{
    if(FALSE == CRCToolStringConvertToUInt32(str, &CRCTool.InitValue))
    {
        printf("init value error!:%s", str);
        return CMDL_ERROR_INVALID_PARAM;
    }

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetXorValue
 * @用  途  CRC工具设置输出异或值(命令行参数回调)
 * @参  数  str:异或值的字符串
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetXorValue(char *str)
{
    if(FALSE == CRCToolStringConvertToUInt32(str, &CRCTool.XorValue))
    {
        printf("xor value error!:%s", str);
        return CMDL_ERROR_INVALID_PARAM;
    }

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetInHex
 * @用  途  CRC工具设置为计算输入16进制形式字符串(命令行参数回调)
 * @参  数  str:输入的字符串
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetInHex(char *str)
{
    size_t strLen = strlen(str);

    if(CRC_TOOL_NOT_IN != CRCTool.InType)
    {
        printf("parameter conflict, with multiple inputs\n");
        return CMDL_ERROR_PARAM_CONFLICT;
    }

    if(0 != (strLen & 1))
    {
        printf("string error!:%s", str);
        return CMDL_ERROR_INVALID_PARAM;
    }

    if(strLen > (CRC_TOOL_IN_HEX_MAX << 1))
    {
        printf("error! param out of range, max of %d", CRC_TOOL_IN_HEX_MAX);
        return CMDL_ERROR_INVALID_PARAM;
    }


    if(FALSE == ConvertStringToHex(str, CRC_TOOL_IN_HEX_MAX << 1, CRCTool.InHex))
    {
        printf("input hex error!:%s", str);
        return CMDL_ERROR_INVALID_PARAM;
    }

    CRCTool.InType = CRC_TOOL_IN_HEX;
    CRCTool.InHexLen = (int)strLen >> 1;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetInString
 * @用  途  CRC工具设置为计算输入文件(命令行参数回调)
 * @参  数  str:输入的字符串
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetInString(char *str)
{
    size_t len = strlen(str);

    if(CRC_TOOL_NOT_IN != CRCTool.InType)
    {
        printf("parameter conflict, with multiple inputs\n");
        return CMDL_ERROR_PARAM_CONFLICT;
    }

    CRCTool.InType = CRC_TOOL_IN_STRING;
    CRCTool.InString = (uint8_t *)str;
    CRCTool.InStringLen = (int)len;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetInFile
 * @用  途  CRC工具设置为计算输入文件(命令行参数回调)
 * @参  数  path:输入文件路径
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetInFile(char *path)
{
    if(CRC_TOOL_NOT_IN != CRCTool.InType)
    {
        printf("parameter conflict, with multiple inputs\n");
        return CMDL_ERROR_PARAM_CONFLICT;
    }

    CRCTool.InType = CRC_TOOL_IN_FILE;
    CRCTool.InFilePath = path;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetOutBinFile
 * @用  途  CRC工具设置将校验和以2进制输出到文件(命令行参数回调)
 * @参  数  path:输出文件的路径
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetOutBinFileLittleEndian(char *path)
{
    if(CRC_TOOL_NOT_OUT != CRCTool.OutType)
    {
        printf("parameter conflict, with multiple outputs\n");
        return CMDL_ERROR_PARAM_CONFLICT;
    }

    CRCTool.OutType = CRC_TOOL_OUT_BIN_LITTLE_ENDIAN;
    CRCTool.OutFilePath = path;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetOutBinFile
 * @用  途  CRC工具设置将校验和以2进制输出到文件(命令行参数回调)
 * @参  数  path:输出文件的路径
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetOutBinFileBigEndian(char *path)
{
    if(CRC_TOOL_NOT_OUT != CRCTool.OutType)
    {
        printf("parameter conflict, with multiple outputs\n");
        return CMDL_ERROR_PARAM_CONFLICT;
    }

    CRCTool.OutType = CRC_TOOL_OUT_BIN_BIG_ENDIAN;
    CRCTool.OutFilePath = path;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetOutTxtFile
 * @用  途  CRC工具设置将校验和以文本形式输出到文件(命令行参数回调)
 * @参  数  path:输出文件的路径
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetOutTxtFile(char *path)
{
    if(CRC_TOOL_NOT_OUT != CRCTool.OutType)
    {
        printf("parameter conflict, with multiple outputs\n");
        return CMDL_ERROR_PARAM_CONFLICT;
    }

    CRCTool.OutType = CRC_TOOL_OUT_TXT;
    CRCTool.OutFilePath = path;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolSetGenerateTable
 * @用  途  CRC工具设置需要生成表(命令行参数回调)
 * @参  数  path:生成表的路径
 * @返回值
*/
static CMDL_ERROR_t CRCToolSetGenerateTable(char *path)
{
    CRCTool.GenerateFlag = TRUE;
    CRCTool.GenerateTablePath = path;

    return CMDL_OK;
}

/*
 * @函数名  CRCToolInit
 * @用  途  CRC工具初始化
 * @参  数  
 * @返回值
*/
void CRCToolInit()
{
    CRCInit(&CRCTool.CRC, CRC_FIRST_BIT_LSB, 32, 0x4C11DB7);
    CRCTool.InitValue = 0xFFFFFFFF;
    CRCTool.XorValue = 0xFFFFFFFF;
    CRCTool.InType = CRC_TOOL_NOT_IN;
    CRCTool.OutType = CRC_TOOL_NOT_OUT;
}

/*
 * @函数名  CRCToolGenerateTable
 * @用  途  CRC工具生成表
 * @参  数  
 * @返回值
*/
void CRCToolGenerateTable()
{
    uint8_t wide = CRCTool.CRC.BitWide;
    
    if(wide <= 8)
    {
        CRCGenerateTable(&CRCTool.CRC, CRC8Table);
    }
    else if(wide <= 16)
    {
        CRCGenerateTable(&CRCTool.CRC, CRC16Table);
    }
    else
    {
        CRCGenerateTable(&CRCTool.CRC, CRC32Table);
    }
    
}

/*
 * @函数名  CRCToolInStringCheck
 * @用  途  CRC工具对输入16进制进行校验和
 * @参  数  
 * @返回值  输出的校验和
*/
uint32_t CRCToolInHexCheck()
{
    CRCStart(&CRCTool.CRC, CRCTool.InitValue);
    CRCUpdate(&CRCTool.CRC, CRCTool.InHex, CRCTool.InHexLen);
    return CRCGetChecksum(&CRCTool.CRC, CRCTool.XorValue);
}

/*
 * @函数名  CRCToolInStringCheck
 * @用  途  CRC工具对输入的字符串进行校验和
 * @参  数  
 * @返回值  输出的校验和
*/
uint32_t CRCToolInStringCheck()
{
    CRCStart(&CRCTool.CRC, CRCTool.InitValue);
    CRCUpdate(&CRCTool.CRC, CRCTool.InString, CRCTool.InStringLen);
    return CRCGetChecksum(&CRCTool.CRC, CRCTool.XorValue);
}

/*
 * @函数名  CRCToolInFileCheck
 * @用  途  CRC工具对输入的文件进行校验和
 * @参  数  checksum:输出的校验和
 * @返回值  FALSE:文件读取有误
 *          TRUE:成功
*/
bool CRCToolInFileCheck(uint32_t *checksum)
{
    FILE *f;
    uint8_t buffer[1024];
    size_t readSize;

    f = fopen(CRCTool.InFilePath, "rb");

    if(NULL == f)
    {
        printf("no such in file:%s\n", CRCTool.InFilePath);
        return FALSE;
    }

    CRCStart(&CRCTool.CRC, CRCTool.InitValue);
    
    while(0 != (readSize = fread(buffer, 1, sizeof(buffer), f)))
    {
        CRCUpdate(&CRCTool.CRC, buffer, readSize);
    }

    fclose(f);

    *checksum = CRCGetChecksum(&CRCTool.CRC, CRCTool.XorValue);

    return TRUE;
}

/*
 * @函数名  CRCToolChecksumOutBin
 * @用  途  将校验和按2进制值写入到文件
 * @参  数  checksum:校验和
 * @返回值  
*/
void CRCToolChecksumOutBin(uint32_t checksum)
{
    FILE *f;

    uint8_t checksumArr[4];

    f = fopen(CRCTool.OutFilePath, "wb");

    if(NULL == f)
    {
        printf("no create out file:%s\n", CRCTool.OutFilePath);
        return;
    }

    if(CRC_TOOL_OUT_BIN_LITTLE_ENDIAN == CRCTool.OutType)
    {
        ConvertUInt32ToUInt8Arr(checksum, checksumArr, CONVERT_LITTLE_ENDIAN);
    }
    else if(CRC_TOOL_OUT_BIN_BIG_ENDIAN == CRCTool.OutType)
    {
        ConvertUInt32ToUInt8Arr(checksum, checksumArr, CONVERT_BIG_ENDIAN);
    }

    fwrite(checksumArr, 1, 4, f);

    fclose(f);
}

/*
 * @函数名  CRCToolChecksumOutTxt
 * @用  途  将校验和按16进制值写入到文件
 * @参  数  checksum:校验和
 * @返回值  
*/
void CRCToolChecksumOutTxt(uint32_t checksum)
{
    FILE *f;

    uint8_t checksumArr[4];
    char checksumTxt[8];

    f = fopen(CRCTool.OutFilePath, "wb");

    if(NULL == f)
    {
        printf("no create out file:%s\n", CRCTool.OutFilePath);
        return;
    }

    ConvertUInt32ToUInt8Arr(checksum, checksumArr, CONVERT_BIG_ENDIAN);

    ConvertHexToString(checksumArr, 4, checksumTxt);

    fwrite(checksumTxt, 1, 8, f);

    fclose(f);
}

/*
 * @函数名  CRCToolGenerateTableFile
 * @用  途  生成存放CRC表数组的文件
 * @参  数  
 * @返回值  
*/
void CRCToolGenerateTableFile()
{
    int i;
    FILE * f;
    char *tablePath = CRCTool.GenerateTablePath;
    CRC_FIRST_BIT_t firstBit = CRCTool.CRC.FirstBit;
    uint8_t bitWide = CRCTool.CRC.BitWide;
    uint8_t arrBitWide;
    uint32_t poly = CRCTool.CRC.Poly;
    void * table = (void *)CRCTool.CRC.CRCTable;

    f = fopen(tablePath, "w");

    if(NULL == f)
    {
        printf("no create crc table file: %s\n", tablePath);
        return;
    }

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

    if(CRC_FIRST_BIT_LSB == firstBit)
    {
        fprintf(f, "const uint%d_t CRCTableLSB0x%X[256] =\n", arrBitWide, poly);
    }
    else
    {
        fprintf(f, "const uint%d_t CRCTableMSB0x%X[256] =\n", arrBitWide, poly);
    }

    fprintf(f, "{");
    for(i = 0;i < 256;++i)
    {
        if((i & 0x7) == 0)
        {
            fprintf(f, "\n\t");
        }
        if(bitWide <= 8)
        {
            fprintf(f, "0x%02X,", ((uint8_t *)(table))[i]);
        }
        else if(bitWide <= 16)
        {
            fprintf(f, "0x%04X,", ((uint16_t *)(table))[i]);
        }
        else
        {
            fprintf(f, "0x%08X,", ((uint32_t *)(table))[i]);
        }
    }

    fprintf(f, "\n};");

    fclose(f);
}

/*
 * @函数名  CRCToolMain
 * @用  途  CRC工具主程序
 * @参  数  argc:字符串个数(main函数传入)
 *          argv:字符串数组(main函数传入)
 * @返回值  
*/
void CRCToolMain(int argc, char **argv)
{
    CMDL_ERROR_t ret;
    uint32_t crcChecksum = 0;
    bool crcPrintFlag = FALSE;

    if(1 == argc)
    {
        CRCToolPrintHelp(NULL);
        return;
    }

    CRCToolInit();

    ret = CMDLAnalysis(&CMDL, argc, argv);

    if(CMDL_OK != ret)
    {
        return;
    }

    CRCToolGenerateTable();

    if(CRC_TOOL_IN_HEX == CRCTool.InType)
    {
        crcPrintFlag = TRUE;
        crcChecksum = CRCToolInHexCheck();
    }
    else if(CRC_TOOL_IN_STRING == CRCTool.InType)
    {
        crcPrintFlag = TRUE;
        crcChecksum = CRCToolInStringCheck();
    }
    else if(CRC_TOOL_IN_FILE == CRCTool.InType)
    {
        if(FALSE == CRCToolInFileCheck(&crcChecksum))
        {
            return;  
        }

        crcPrintFlag = TRUE;
    }

    if((CRC_TOOL_OUT_BIN_LITTLE_ENDIAN == CRCTool.OutType)
       || (CRC_TOOL_OUT_BIN_BIG_ENDIAN == CRCTool.OutType)
    )
    {
        CRCToolChecksumOutBin(crcChecksum);
    }
    else if(CRC_TOOL_OUT_TXT == CRCTool.OutType)
    {
        CRCToolChecksumOutTxt(crcChecksum);
    }

    if(FALSE != CRCTool.GenerateFlag)
    {
        CRCToolGenerateTableFile();
        if(FALSE == crcPrintFlag)
        {
            CRCPrintInfo(&CRCTool.CRC);
        }
    }

    if(FALSE != crcPrintFlag)
    {
        CRCPrintInfo(&CRCTool.CRC);
        printf("init value:0x%X\n", CRCTool.InitValue);
        printf("init value:0x%X\n", CRCTool.XorValue);
        printf("checksum:0x%X\n", crcChecksum);
    }
    
}