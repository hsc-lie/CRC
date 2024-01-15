#ifndef __CMDL_H_
#define __CMDL_H_

#include <stdio.h>
#include <stdint.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!(FALSE))
#endif

typedef uint8_t bool;

typedef enum
{
    CMDL_OK,                          //成功
    CMDL_ERROR_NO_INPUT_PARAM,        //没有输入参数
    CMDL_ERROR_INVALID_PARAM,         //参数不存在或不正确
    CMDL_ERROR_FUNC_NULL,             //回调函数为NULL
    CMDL_ERROR_PARAM_CONFLICT,        //参数冲突
}CMDL_ERROR_t;

typedef enum
{
    CMDL_PARAM_TYPE_SINGLE,           //单个参数
    CMDL_PARAM_TYPE_MULTIPLE,         //后接有参数
}CMDL_PARAM_TYPE_t;

typedef CMDL_ERROR_t (*CMDLParamHandleFunc_t)(char *params);

typedef struct
{
    CMDL_PARAM_TYPE_t Type;           //参数类型
    char LetterParam;                 //字母参数
    char *WordParam;                  //单词参数
    CMDLParamHandleFunc_t Func;       //参数对应的回调函数
}CMDLParam_t;


typedef struct
{
    CMDLParamHandleFunc_t NotFlagParamFunc;      //无标志参数的回调函数
    CMDLParam_t *ParamTable;                     //参数列表
    int ParamTableLen;                           //参数列表长度
}CMDL_t;

extern CMDL_ERROR_t CMDLAnalysis(CMDL_t *cmdl, int argc, char **argv);

#endif