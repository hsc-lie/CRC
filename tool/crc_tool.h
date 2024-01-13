#ifndef __CRC_TOOL_H_
#define __CRC_TOOL_H_

#include "crc.h"

//版本号
#define CRC_TOOL_VERSION    "v0.0.01"

//CRC工具可接受的输入16进制计算的最大数量
#define CRC_TOOL_IN_HEX_MAX  32

typedef enum
{
    CRC_TOOL_NOT_IN,               //没有需要计算的输入
    CRC_TOOL_IN_HEX,               //需要计算输入16进制的数据
    CRC_TOOL_IN_STRING,            //需要计算输入的字符串
    CRC_TOOL_IN_FILE,              //需要计算输入的文件
}CRC_TOOL_IN_TYPE_t;

typedef enum
{
    CRC_TOOL_NOT_OUT,              //没有文件需要输出
    CRC_TOOL_OUT_BIN,              //需要将计算的校验和输出为bin格式(暂只支持大端)
    CRC_TOOL_OUT_TXT,              //需要将计算的校验和输出为文本格式
}CRC_TOOL_OUT_TYPE_t;

typedef struct
{
    CRC_TOOL_IN_TYPE_t InType;                     //输入类型
    CRC_TOOL_OUT_TYPE_t OutType;                   //输出类型
    bool GenerateFlag;                             //是否需要生成CRC表
    uint8_t InHex[CRC_TOOL_IN_HEX_MAX];            //输入的16进制缓存
    int InHexLen;                                  //输入16进制值的长度
    uint8_t *InString;                             //输入的字符串
    int InStringLen;                               //输入的字符串长度
    char *InFilePath;                              //输入的文件路径
    char *OutFilePath;                             //输出的文件路径
    char *GenerateTablePath;                       //生成CRC表的文件路径
    CRC_t CRC;                                     //CRC结构体
    uint32_t InitValue;                            //CRC校验初始值
    uint32_t XorValue;                             //CRC校验异或值
}CRCTool_t;

void CRCToolMain(int argc, char **argv);

#endif /*__CRC_TOOL_H_*/