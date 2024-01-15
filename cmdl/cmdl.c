#include "cmdl.h"

#include "string.h"

/*
 * @函数名  CMDLFindLetterParam
 * @用  途  查找匹配的字母参数
 * @参  数  table:命令行参数表
 *          tableLen:命令行参数表的长度
 *          param:参数
 * @返回值  NULL:没有找到匹配项
 *          其他:匹配到的参数项
*/
static CMDLParam_t *CMDLFindLetterParam(CMDLParam_t *table, int tableLen, char param)
{
    int i;

    for(i = 0;i < tableLen;++i)
    {
        if(param == table[i].LetterParam)
        {
            return &table[i];
            break;
        }
    }

    return NULL;
}

/*
 * @函数名  CMDLLetterParamAnalysis
 * @用  途  字母参数解析
 * @参  数  table:命令行参数表
 *          tableLen:命令行参数表的长度
 *          params:参数
 *          func:返回参数参数项的回调函数(用于接收后一个参数调用)
 * @返回值  错误的状态
*/
static CMDL_ERROR_t CMDLLetterParamAnalysis(CMDLParam_t *table, int tableLen, char *params, CMDLParamHandleFunc_t *func)
{
    CMDL_ERROR_t ret = CMDL_ERROR_NO_INPUT_PARAM;
    CMDLParam_t *cmdlParam;
    
    *func = NULL;

    while('\0' != *params)
    {
        cmdlParam = CMDLFindLetterParam(table, tableLen, *params);

        if(NULL == cmdlParam)
        {
            printf("invalid parameter -%c", *params);
            return CMDL_ERROR_INVALID_PARAM;
        }

        if(NULL == cmdlParam->Func)
        {
            printf("function is null\n");
            return CMDL_ERROR_FUNC_NULL;
        }

        if(CMDL_PARAM_TYPE_SINGLE == cmdlParam->Type)
        {
            ret = cmdlParam->Func(NULL);
        }
        else if(CMDL_PARAM_TYPE_MULTIPLE == cmdlParam->Type)
        {
            if(NULL == *func)
            {
                *func = cmdlParam->Func;
                ret = CMDL_OK;
            }
            else
            {
                return CMDL_ERROR_PARAM_CONFLICT;
            }
        }

        ++params;
    }

    return ret;
}

/*
 * @函数名  CMDLFindWordParam
 * @用  途  查找匹配的单词参数
 * @参  数  table:命令行参数表
 *          tableLen:命令行参数表的长度
 *          params:参数
 * @返回值  NULL:没有找到匹配项
 *          其他:匹配到的参数项
*/
static CMDLParam_t *CMDLFindWordParam(CMDLParam_t *table, int tableLen, char *param)
{
    int i;

    for(i = 0;i < tableLen;++i)
    {
        if(0 == strcmp(param, table[i].WordParam))
        {
            return &table[i];
            break;
        }
    }

    return NULL;
}

/*
 * @函数名  CMDLWordParamAnalysis
 * @用  途  单词参数解析
 * @参  数  table:命令行参数表
 *          tableLen:命令行参数表的长度
 *          params:参数
 *          func:返回参数参数项的回调函数(用于接收后一个参数调用)
 * @返回值  错误的状态
*/
static CMDL_ERROR_t CMDLWordParamAnalysis(CMDLParam_t *table, int tableLen, char *params, CMDLParamHandleFunc_t *func)
{
    CMDL_ERROR_t ret = CMDL_ERROR_NO_INPUT_PARAM;
    CMDLParam_t *cmdlParam;
    
    *func = NULL;

    cmdlParam = CMDLFindWordParam(table, tableLen, params);

    if(NULL == cmdlParam)
    {
        printf("invalid parameter --%s\n", params);
        return CMDL_ERROR_INVALID_PARAM;
    }

    if(NULL == cmdlParam->Func)
    {
        printf("function is null\n");
        return CMDL_ERROR_FUNC_NULL;
    }

    if(CMDL_PARAM_TYPE_SINGLE == cmdlParam->Type)
    {
        ret = cmdlParam->Func(NULL);
    }
    else if(CMDL_PARAM_TYPE_MULTIPLE == cmdlParam->Type)
    {
        *func = cmdlParam->Func;
        ret = CMDL_OK;
    }

    return ret;
}

/*
 * @函数名  CMDLAnalysis
 * @用  途  命令行解析
 * @参  数  cmdl:命令行结构体
 *          argc:字符串个数(main函数传入)
 *          argv:字符串数组(main函数传入)
 * @返回值  错误的状态
*/
CMDL_ERROR_t CMDLAnalysis(CMDL_t *cmdl, int argc, char **argv)
{
    int i;
    CMDL_ERROR_t ret = CMDL_ERROR_NO_INPUT_PARAM;
    CMDLParamHandleFunc_t multipleParamsHandleFunc;

    for(i = 1;i < argc;++i)
    {
        if(argv[i][0] != '-')//无flag参数
        {
            if(NULL == cmdl->NotFlagParamFunc)
            {
                printf("invalid parameter\n");
                return CMDL_ERROR_INVALID_PARAM;
            }

            ret = cmdl->NotFlagParamFunc(argv[i]); 
        }
        else if(argv[i][1] == '-')//双横杆flag参数
        {
            ret = CMDLWordParamAnalysis(cmdl->ParamTable, cmdl->ParamTableLen, &argv[i][2], &multipleParamsHandleFunc);

            if(CMDL_OK != ret)
            {
                return ret;
            }

            if(NULL == multipleParamsHandleFunc)
            {
                continue;
            }

            ++i;
            if(i < argc)
            {
                ret = multipleParamsHandleFunc(argv[i]);
            }
            else
            {
                printf("%s no input parameters\n", argv[i-1]);
                return CMDL_ERROR_NO_INPUT_PARAM;
            }

        }
        else//单横杆flag参数
        {
            ret = CMDLLetterParamAnalysis(cmdl->ParamTable, cmdl->ParamTableLen, &argv[i][1], &multipleParamsHandleFunc);

            if(CMDL_OK != ret)
            {
                return ret;
            }

            if(NULL == multipleParamsHandleFunc)
            {
                continue;
            }

            ++i;
            if(i < argc)
            {
                ret = multipleParamsHandleFunc(argv[i]);
            }
            else
            {
                printf("%s no input parameters\n", argv[i-1]);
                return CMDL_ERROR_NO_INPUT_PARAM;
            } 
        }

        if(CMDL_OK != ret)
        {
            return ret;
        }
    }

    return ret;
}

