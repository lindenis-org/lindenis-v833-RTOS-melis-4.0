//------------------------------------------------------------------------------------------------------------
//
// resaccess.cpp
//
// 2009-05-20 16:43:57
//
//
//
//------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "basetypes.h"
#include "error.h"

#include "../if/resaccess_if.h"

//#define PLUGINAPI extern "C" __declspec(dllexport)
#include "resaccess.h"

#include "resaccess_private.h"

#include "resaccess_private.cpp"

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//     插件入口
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
/*
bool APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    return TRUE;
}
*/
//------------------------------------------------------------------------------------------------------------
//插件的通用接口
//------------------------------------------------------------------------------------------------------------
//#include "../if/plugin_common.c"


PLUGINAPI u32   QueryRecAccessInterface(const char *plug_guid)
{
    if (0 == strcmp(plug_guid, (const char *)RESACCESS_PLUGIN_TYPE))
    {
        return OK;
    }
    else
    {
        return __LINE__;
    }

}

PLUGINAPI void *GetResAccessInterface()
{
    static RESACCESS_IF __if;

    //if (0 == strcmp(plug_guid, (const char *)RESACCESS_PLUGIN_TYPE))
    //{
    __if.OpenRes        = OpenRes;

    __if.CloseRes       = CloseRes;

    __if.ReadRes        = ReadRes;

    __if.GetResSize     = GetResSize;

    __if.GetResAddr     = GetResAddr;

    __if.GetResType     = GetResType;

    __if.GetRes         = GetRes;

    return &__if;
    //}
    //else
//  {
//      return NULL;
//  }
}

