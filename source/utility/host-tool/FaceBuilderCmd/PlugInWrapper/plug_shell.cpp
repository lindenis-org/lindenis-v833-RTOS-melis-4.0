//------------------------------------------------------------------------------------------------------------
//
//2008-10-16 8:48:43
//
//scott
//
//plug_shell.cpp
//
//------------------------------------------------------------------------------------------------------------

#ifndef ___PLUG_IN__SHELL_____CPP_______
#define ___PLUG_IN__SHELL_____CPP_______

#include "plug_shell.h"
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>

typedef struct tag_PLUG_IN
{
    HINSTANCE   hModule;
    char        Plug_GUID[36];  //GUID_LEN
    u32         res0;           //must be 0
    Common_t    Common;         //
    u32         res1;           //
} __attribute__((packed))PLUG_IN_t;

void *LoadLibrary(const char *lpFileName);
int FreeLibrary(void *hModule);

#define GetProcAddress dlsym;

HPLUG _LoadPlugIn(const char *plug_path)
{
    PLUG_IN_t *plug_in = NULL;
    Common_t   *common  = NULL;

    plug_in = (PLUG_IN_t *)malloc(sizeof(PLUG_IN_t));
    memset(plug_in, 0, sizeof(PLUG_IN_t));

    plug_in->hModule = LoadLibrary(plug_path);

    if (NULL == plug_in->hModule)
    {
        char str[256];
        sprintf(str, "Err:%s %d %s", __FILE__, __LINE__, plug_path);
        printf("%s\n", str);

        free(plug_in);
        return NULL;
    }

    common = _GetCommonInterface(plug_in);
    if (NULL == common)
    {
        char str[256];
        sprintf(str, "Err:%s %d", __FILE__, __LINE__);

        printf("%s\n", str);
        _UnLoadPlugIn(plug_in);
        free(plug_in);
        return NULL;
    }

    return plug_in;
}

u32  _UnLoadPlugIn(HPLUG hPlug)
{
    PLUG_IN_t *plug_in = NULL;

    if (NULL == hPlug)
        return __LINE__;

    plug_in = (PLUG_IN_t *)hPlug;

    FreeLibrary(plug_in->hModule);
    plug_in->hModule = NULL;

    free(plug_in);
    plug_in = NULL;

    return OK;
}

Common_t   *_GetCommonInterface(HPLUG hPlug)
{
    PLUG_IN_t *plug_in = NULL;
    Common_t   *common  = NULL;

    if (NULL == hPlug)
        return NULL;

    plug_in = (PLUG_IN_t *)hPlug;
    common = &plug_in->Common;

    common->GetPlugInVersion = (pGetPlugInVersion)GetProcAddress(plug_in->hModule, "GetPlugInVersion");
    common->GetPlugInType    = (pGetPlugInType)GetProcAddress(plug_in->hModule, "GetPlugInType");
    common->GetPlugInName    = (pGetPlugInName)GetProcAddress(plug_in->hModule, "GetPlugInName");
    common->GetAuthor        = (pGetAuthor)GetProcAddress(plug_in->hModule, "GetAuthor");
    common->GetCopyRight     = (pGetCopyRight)GetProcAddress(plug_in->hModule, "GetCopyRight");
    common->QueryInterface   = (pQueryInterface)GetProcAddress(plug_in->hModule, "QueryInterface");
    common->GetInterface     = (pGetInterface)GetProcAddress(plug_in->hModule, "GetInterface");

    /*
        if(
        (common->GetPlugInVersion ==  NULL ) ||
        (common->GetPlugInType    ==  NULL ) ||
        (common->GetPlugInName    ==  NULL ) ||
        (common->GetAuthor        ==  NULL ) ||
        (common->GetCopyRight     ==  NULL ) ||
        (common->QueryInterface   ==  NULL ) ||
        (common->GetInterface     ==  NULL ) )
        {
            return NULL;
        }
    */
    return common;
}

u32  _QueryInterface(HPLUG hPlug, const char *plug_guid)
{
    u32 ret = OK;
    PLUG_IN_t *plug_in = NULL;
    Common_t   *common  = NULL;

    if (NULL == hPlug)
    {
        return __LINE__;
    }

    plug_in = (PLUG_IN_t *)hPlug;
    common  = &plug_in->Common;

    if (NULL == common->QueryInterface)
    {

        return __LINE__;
    }

    ret = common->QueryInterface(plug_guid);

    return  ret;
}

void *_GetInterface(HPLUG hPlug, const char *plug_guid)
{
    PLUG_IN_t *plug_in = NULL;
    Common_t   *common  = NULL;

    if (NULL == hPlug)
        return NULL;

    plug_in = (PLUG_IN_t *)hPlug;
    common = &plug_in->Common;

    if (NULL == common->GetInterface)
        return NULL;

    return common->GetInterface(plug_guid);
}

#endif //___PLUG_IN__SHELL_____CPP_______
