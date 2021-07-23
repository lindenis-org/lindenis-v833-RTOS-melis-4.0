//------------------------------------------------------------------------------------------------------------
//
//2008-10-16 8:48:43
//
//scott
//
//plug_common_if.h
//
//------------------------------------------------------------------------------------------------------------

#ifndef ___PLUG_COMMON__IF___H_______
#define ___PLUG_COMMON__IF___H_______

#include "include\basetypes.h"

//------------------------------------------------------------------------------------------------------------
///插件的通用的接口类型定义
//------------------------------------------------------------------------------------------------------------
typedef s8 *(*pGetPlugInType)();
typedef s8 *(*pGetPlugInName)();
typedef u32(*pGetPlugInVersion)();
typedef s8 *(*pGetAuthor)();
typedef s8 *(*pGetCopyRight)();

typedef u32(*pQueryInterface)(const char *plug_guid);
typedef void *(*pGetInterface)(const char *plug_guid);

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
typedef struct tag_Common
{
    pGetPlugInVersion   GetPlugInVersion;
    pGetPlugInType      GetPlugInType;
    pGetPlugInName      GetPlugInName;
    pGetAuthor          GetAuthor;
    pGetCopyRight       GetCopyRight;

    pQueryInterface     QueryInterface;
    pGetInterface       GetInterface;
} Common_t;
#pragma pack(pop)

//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------

#endif //___PLUG_COMMON__IF___H_______
