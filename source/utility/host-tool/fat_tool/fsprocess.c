//------------------------------------------------------------------------------------------------------------
//
// fsprocess.cpp
//
// scott
//
// 2009-12-24 9:48:25
//
//------------------------------------------------------------------------------------------------------------

#include "include\config.h"

#include "src\share\error.cpp"

#include "src\share\file.cpp"

#include "if\fsprocess_if.h"

#include "if\Part_File_Plugin_if.h"
#include "if\part_if.h"

#define PLUGINAPI extern "C" __declspec(dllexport)

#include "fsprocess.h"
#include "fsprocess_inter.h"
#include "fsprocess_inter.cpp"

bool APIENTRY DllMain(HANDLE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    return TRUE;
}

//插件的通用接口
#include "if\plugin_common.c"

PLUGINAPI u32 QueryInterface(const char *plug_guid)
{
    if (0 == strcmp(plug_guid, (const char *)FSPROCESS_PLUGIN_TYPE))
    {
        return OK;
    }
    else
    {
        return __LINE__;
    }

}

PLUGINAPI void *GetInterface(const char *plug_guid)
{
    static FSPROCESS_IF __if;

    if (0 == strcmp(plug_guid, (const char *)FSPROCESS_PLUGIN_TYPE))
    {
        __if.BuildFileSystem    = BuildFileSystem;

        return &__if;
    }
    else
    {
        return NULL;
    }
}
