//------------------------------------------------------------------------------------------------------------
//
// scriptparser.cpp
//
// 2009-05-20 16:43:57
//
//
//
//------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "basetypes.h"

#include "error.h"


#include "../if/scriptparser_if.h"

#include "../if/config_if.h"

//#define PLUGINAPI extern "C" __declspec(dllexport)
#include "scriptparser.h"
#include "scriptparser_inc.h"


typedef void *HSCRIPT;

HSCRIPT OpenScript(char *script_path, void *config_if);

u32     ParseScript(HSCRIPT hScript);

u32     GetSysInfo(HSCRIPT hScript, void *sysinfo, u32 size);

u32     GetResCount(HSCRIPT hScript, u32 *resCount);

u32     GetResInfo(HSCRIPT hScript, void *resinfo, u32 size, u32 index);

u32     CloseScript(HSCRIPT hScript);

u32     SetSysInfo(HSCRIPT hScript, void *sysinfo, u32 size);

u32     SetResInfo(HSCRIPT hScript, void *resinfo, u32 size, u32 index);

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
#include "../if/plugin_common.c"

/*
u32   QueryInterface    (const char * plug_guid)
{
    if (0 == strcmp(plug_guid, (const char *)SCRIPTPARSER_PLUGIN_TYPE))
    {
        return OK;
    }
    else
    {
        return __LINE__;
    }

}
*/

void *GetScriptParserInterface()
{
    static SCRIPTPARSER_IF __if;

//  if (0 == strcmp(plug_guid, (const char *)SCRIPTPARSER_PLUGIN_TYPE))
//  {
    __if.OpenScript   = OpenScript;     //
    __if.ParseScript  = ParseScript;    //
    __if.GetSysInfo   = GetSysInfo;     //
    __if.GetResCount  = GetResCount;    //
    __if.GetResInfo   = GetResInfo;     //
    __if.CloseScript  = CloseScript;    //
    __if.SetSysInfo   = SetSysInfo;     //
    __if.SetResInfo   = SetResInfo;     //

    return &__if;
//  }
//  else
//  {
//      return NULL;
//  }
}

HSCRIPT OpenScript(char *script_path, void *config_if)
{
#define __FUNCTION__    "OpenScript"
    FACE_SCRIPT *pScript = NULL;
    HCONFIG hConfig = NULL;
    CONFIG_IF *pConfig = NULL;

    if (NULL == script_path || NULL == config_if)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "Invalid arguments");
        return NULL;
    }

    pScript = (FACE_SCRIPT *)malloc(sizeof(FACE_SCRIPT));
    if (NULL == pScript)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc faild");
        return NULL;
    }
    memset(pScript , 0, sizeof(FACE_SCRIPT));

    /* initi handle first */
    strncpy(pScript->ScriptFileName, script_path, sizeof(pScript->ScriptFileName));
    pScript->SysInfo = NULL;
    pScript->ResInfoTable = NULL;
    pScript->ResCount = 0;

    //----------------------------------------------------------
    // set config info
    //----------------------------------------------------------
    pScript->config_if = config_if;
    pConfig = (CONFIG_IF *)config_if;

    hConfig = pConfig->OpenConfig(script_path, NULL);
    if (NULL == hConfig)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "OpenConfig failed");
        free(pScript);
        return NULL;
    }
    pScript->hConfig = hConfig;

    return (HSCRIPT)pScript;

#undef __FUNCTION__
}

static u32 LoadSystemInfo(CONFIG_IF *config_if, HCONFIG hConfig, FACE_SYS_INFO *sys_info)
{
#define __FUNCTION__    "LoadSystemInfo"
    u32 ret = OK;
    char szValue[MAX_VALUE];

    if (NULL == config_if || NULL == hConfig || NULL == sys_info)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    //  scriptver
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_SCRIPTVER, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_SYS_SCRIPTVER, ret);
        return ret;
    }
    sys_info->Scriptver = atoi(szValue);

    //----------------------------------------------------------
    //  uistyle
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_UISTYLE, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_SYS_UISTYLE, ret);
        return ret;
    }
    strncpy(sys_info->UIstyle, szValue, sizeof(sys_info->UIstyle));

    //----------------------------------------------------------
    //  name
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_NAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_SYS_NAME, ret);
        return ret;
    }
    strncpy(sys_info->Name, szValue, sizeof(sys_info->Name));

    //----------------------------------------------------------
    //  author
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_AUTHOR, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_SYS_AUTHOR, ret);
        return ret;
    }
    strncpy(sys_info->Author, szValue, sizeof(sys_info->Author));

    //----------------------------------------------------------
    //  solution
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_SOLUTION, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "GetKeyString(%s) faild %d", KEYNAME_SYS_SOLUTION, ret);
        return ret;
    }
    strncpy(sys_info->Solution, szValue, sizeof(sys_info->Solution));

    //----------------------------------------------------------
    //  stylename
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_STYLENAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_SYS_STYLENAME, ret);
        return ret;
    }
    strncpy(sys_info->Stylename, szValue, sizeof(sys_info->Stylename));

    //----------------------------------------------------------
    //  StyleID : 目前未使用，使用默认值
    //----------------------------------------------------------
    sys_info->StyleID = DEF_STYLE_ID;

    return ret;

#undef __FUNCTION__
}

static u32 LoadResCount(CONFIG_IF *config_if, HCONFIG hConfig, u32 *res_count)
{
#define __FUNCTION__    "LoadResCount"

    u32 ret = OK;
    char szValue[MAX_VALUE];
    static u32 ResCount = 0;

    if (NULL == config_if || NULL == hConfig || NULL == res_count)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    //  res_count
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetSectionCount(hConfig, &ResCount);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "GetSectionCount faild %d", ret);
        return ret;
    }

    //----------------------------------------------------------
    //  section count :
    //  system section(only one now) + res section
    //----------------------------------------------------------
    *res_count = ResCount - 1;

    return ret;
#undef __FUNCTION__
}

static u32 LoadResInfoByName(CONFIG_IF *config_if, HCONFIG hConfig,  FACE_RES_ITEM *res_info, s8 *sec_name)
{
#define __FUNCTION__ "LoadResInfoByName"

    u32  ret = OK;
    char szValue[MAX_VALUE];

    if (NULL == config_if || NULL == hConfig || NULL == res_info || NULL == sec_name)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    //  name
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_NAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "Section : %s, GetKeyString(%s) faild %d", sec_name, KEYNAME_RES_NAME, ret);
        return ret;
    }
    if (RES_NAME_LEN < strlen(szValue))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "res name too long (%s)", sec_name);
        return ret;
    }
    strncpy(res_info->Name, szValue, sizeof(res_info->Name));

    //----------------------------------------------------------
    //  file
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_FILE, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "Section : %s, GetKeyString(%s) faild %d",  sec_name, KEYNAME_RES_FILE, ret);
        return ret;
    }
    if (MAX_PATH < strlen(szValue))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "file path too long (%s)", sec_name);
        return __LINE__;
    }
    strncpy(res_info->File, szValue, sizeof(res_info->File));

    //----------------------------------------------------------
    //  commentCN
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_COMMENTCN, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "Section : %s, GetKeyString(%s) faild %d",  sec_name, KEYNAME_RES_COMMENTCN, ret);
        return ret;
    }
    if (COMMENT_LEN < strlen(szValue))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CommentCN too long (%s)", sec_name);
        return __LINE__;
    }
    strncpy(res_info->CommentCN, szValue, sizeof(res_info->CommentCN));

    //----------------------------------------------------------
    //  commentEN
    //----------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    ret = config_if->GetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_COMMENTEN, (s8 *)szValue);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "Section : %s, GetKeyString(%s) faild %d",  sec_name, KEYNAME_RES_COMMENTEN, ret);
        return ret;
    }
    if (COMMENT_LEN < strlen(szValue))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CommentEN too long (%s)", sec_name);
        return __LINE__;
    }
    strncpy(res_info->CommentEN, szValue, sizeof(res_info->CommentEN));

    //----------------------------------------------------------
    //  ID : 目前未使用，此处暂时只能填0
    //----------------------------------------------------------
    res_info->ID = 0;

    //----------------------------------------------------------
    //  Size
    //----------------------------------------------------------
    res_info->Size = FACE_RES_ITEM_SIZE;

    //----------------------------------------------------------
    //  Ver
    //----------------------------------------------------------
    res_info->Ver = FACE_RES_ITEM_VER;

    //----------------------------------------------------------
    //  store res section name
    //----------------------------------------------------------
    if (SECTION_NAME_LEN < strlen((char *)sec_name))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "section name too long (%s)", sec_name);
        return __LINE__;
    }
    strncpy(res_info->Section, (char *)sec_name, sizeof(res_info->Section));

    return ret;

#undef __FUNCTION__
}

static u32 InvalidResName(char *res_name)
{
    u32 ret = OK;

    if (NULL == res_name)
        return __LINE__;
    //----------------------------------------------------------
    //  invalid res name : charecter, digit, '_'
    //----------------------------------------------------------
    while (*res_name != '\0')
    {
        if (isalpha(*res_name) || ((*res_name) == '_') || isdigit(*res_name))
        {
            res_name++;
        }
        else
        {
            return __LINE__;
        }
    }
    return ret;
}

static u32 IsNullFileName(char *file_name)
{
    u32 ret = OK;

    if (strcasecmp(file_name, NULL_FILE_NAME))
    {
        return __LINE__;
    }

    return ret;
}

static u32 ResItemsCheck(FACE_RES_ITEM res_table[], u32 res_count)
{
#define __FUNCTION__    "ResItemsCheck"

    u32 i;
    u32 j;
    u32 ret = OK;

    //----------------------------------------------------------
    // res items name must different
    //----------------------------------------------------------
    for (i = 0; i < res_count; i++)
    {

        //----------------------------------------------------------
        // check res name invalid or not
        //----------------------------------------------------------
        ret = InvalidResName(res_table[i].Name);
        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "Find invalid res name (%s)", res_table[i].Name);
            return  __LINE__;
        }

        //----------------------------------------------------------
        // 目前暂时不检测
        //----------------------------------------------------------
        //ret = IsNullFileName(res_table[i].File);
        //if (OK != ret)
        //{
        //    ret = IsFileExist(res_table[i].File);
        //    if (OK != ret)
        //    {
        //        // 文件不存在，文件名填为NULL
        //        strcpy(res_table[i].File, NULL_FILE_NAME);
        //
        //        //Err(__FUNCTION__, __FILE__, __LINE__,
        //        //    "res file %s not exist", res_table[i].File);
        //        //return  __LINE__;
        //    }
        //}

        //Msg("%d name=%s", i, res_table[i].Name);
        //----------------------------------------------------------
        // check res name repeat or not
        //----------------------------------------------------------
        for (j = (i + 1); j < res_count; j++)
        {
            if (!strcasecmp(res_table[i].Name, res_table[j].Name))
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "Find repeat res name %d(%s) %d(%s)", i, res_table[i].Name, j, res_table[j].Name);
                return  __LINE__;
            }
        }
    }

    //----------------------------------------------------------
    // maybe have other check here
    //----------------------------------------------------------
    return ret;

#undef __FUNCTION__
}

/*
u32     ParseScript(HSCRIPT hScript)
{
    u32 i = 0;
    u32 ret = OK;
    u32 resCount = 0;
    HCONFIG hConfig = NULL;
    CONFIG_IF *pConfig = NULL;
    FACE_SCRIPT *pScript = NULL;
    s8 resSectionName[SECTION_NAME_LEN];

    if (NULL == hScript)
    {
        return __LINE__;
    }
    pScript = (FACE_SCRIPT *)hScript;
    pConfig = (CONFIG_IF *)(pScript->config_if);
    hConfig = (HCONFIG)(pScript->hConfig);

    //----------------------------------------------------------
    // load system info
    //----------------------------------------------------------
    pScript->SysInfo = (FACE_SYS_INFO *)malloc(FACE_SYS_INFO_SIZE);
    if (NULL == pScript->SysInfo)
    {
        Err("ParseScript", __FILE__, __LINE__, "malloc");
        pConfig->CloseConfig(hConfig);
        return __LINE__;
    }

    memset(pScript->SysInfo, 0, FACE_SYS_INFO_SIZE);

    ret = LoadSystemInfo(pConfig, hConfig, pScript->SysInfo);
    if (OK != ret)
    {
        Err("ParseScript", __FILE__, __LINE__, "LoadSystemInfo %d", ret);
        pConfig->CloseConfig(hConfig);
        return ret;
    }

    //----------------------------------------------------------
    // load section count
    //----------------------------------------------------------
    ret = LoadResCount(pConfig, hConfig, &resCount);
    if (OK != ret)
    {
        Err("ParseScript", __FILE__, __LINE__, "LoadResCount %d", ret);
        pConfig->CloseConfig(hConfig);
        return ret;
    }
    pScript->ResCount = resCount;

    //----------------------------------------------------------
    // load res info
    //----------------------------------------------------------
    pScript->ResInfoTable = (FACE_RES_ITEM *)malloc(FACE_RES_ITEM_SIZE * resCount);
    if (NULL == pScript->ResInfoTable)
    {
        Err("ParseScript", __FILE__, __LINE__, "malloc");
        pConfig->CloseConfig(hConfig);
        return __LINE__;
    }

    memset(pScript->ResInfoTable, 0, FACE_RES_ITEM_SIZE * resCount);

    for (i = 0; i < resCount; i++)
    {
        memset(resSectionName, 0, sizeof(resSectionName));
        sprintf((char *)resSectionName, "%s%d", SECNAME_RES_PREFIX, (i + 1));

        ret = LoadResInfoByName(pConfig, hConfig, &(pScript->ResInfoTable[i]), resSectionName);
        if (OK != ret)
        {
            Err("ParseScript", __FILE__, __LINE__, "LoadResInfoByName %d", ret);
            pConfig->CloseConfig(hConfig);
            return ret;
        }

        //----------------------------------------------------------
        // res ID : 目前在脚本中无该项内容，暂时顺利排列ID号
        // ID从1开始编号, 未来可能会采用其他方式获得ID号
        //----------------------------------------------------------
        pScript->ResInfoTable[i].ID = i + RES_ID_BASE;
    }

    //----------------------------------------------------------
    // res items check
    //----------------------------------------------------------
    ret = ResItemsCheck(pScript->ResInfoTable, pScript->ResCount);
    if (OK != ret)
    {
        Err("ParseScript", __FILE__, __LINE__, "res check");
        pConfig->CloseConfig(hConfig);
        return ret;
    }

    return ret;
}
*/

// res_n 可以有空格的处理方法
u32     ParseScript(HSCRIPT hScript)
{
#define __FUNCTION__ "ParseScript"
    u32 i = 0;
    u32 ret = OK;
    u32 resCount = 0;
    HCONFIG hConfig = NULL;
    CONFIG_IF *pConfig = NULL;
    FACE_SCRIPT *pScript = NULL;
    s8 resSectionName[SECTION_NAME_LEN];

    if (NULL == hScript)
    {
        return __LINE__;
    }
    pScript = (FACE_SCRIPT *)hScript;
    pConfig = (CONFIG_IF *)(pScript->config_if);
    hConfig = (HCONFIG)(pScript->hConfig);

    //----------------------------------------------------------
    // load system info
    //----------------------------------------------------------
    pScript->SysInfo = (FACE_SYS_INFO *)malloc(FACE_SYS_INFO_SIZE);
    if (NULL == pScript->SysInfo)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc");
        pConfig->CloseConfig(hConfig);
        return __LINE__;
    }

    memset(pScript->SysInfo, 0, FACE_SYS_INFO_SIZE);

    ret = LoadSystemInfo(pConfig, hConfig, pScript->SysInfo);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "LoadSystemInfo %d", ret);
        pConfig->CloseConfig(hConfig);
        return ret;
    }

    //----------------------------------------------------------
    // load section count
    //----------------------------------------------------------
    ret = LoadResCount(pConfig, hConfig, &resCount);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "LoadResCount %d", ret);
        pConfig->CloseConfig(hConfig);
        return ret;
    }
    pScript->ResCount = resCount;

    //----------------------------------------------------------
    // load res info
    //----------------------------------------------------------
    pScript->ResInfoTable = (FACE_RES_ITEM *)malloc(FACE_RES_ITEM_SIZE * resCount);
    if (NULL == pScript->ResInfoTable)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "malloc");
        pConfig->CloseConfig(hConfig);
        return __LINE__;
    }

    memset(pScript->ResInfoTable, 0, FACE_RES_ITEM_SIZE * resCount);

    //----------------------------------------------------------
    // res_n 可以有空格的处理方法
    //----------------------------------------------------------

    //扫描res section的名称
    POS pos = pConfig->GetFirstSection(hConfig);
    i = 0;
    while (pos)
    {
        memset(resSectionName, 0, SECTION_NAME_LEN);
        pos = pConfig->GetNextSection(hConfig, pos, resSectionName);

        char *pch = strstr((char *)resSectionName, SECNAME_RES_PREFIX);
        if (NULL == pch)
        {
            continue;
        }

        ret = LoadResInfoByName(pConfig, hConfig, &(pScript->ResInfoTable[i]), resSectionName);
        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "LoadResInfoByName failed %d", ret);
            pConfig->CloseConfig(hConfig);
            return ret;
        }

        //----------------------------------------------------------
        // res ID : 目前在脚本中无该项内容，暂时顺利排列ID号
        // ID从1开始编号, 未来可能会采用其他方式获得ID号
        //----------------------------------------------------------
        pScript->ResInfoTable[i].ID = i + RES_ID_BASE;

        //Msg("%d %d %s %s %s\n", i, pScript->ResInfoTable[i].ID, pScript->ResInfoTable[i].Section, pScript->ResInfoTable[i].Name, pScript->ResInfoTable[i].File);
        i++;
    }

    /*
        for (i = 0; i < resCount; i++)
        {
            memset(resSectionName, 0, sizeof(resSectionName));
            sprintf((char *)resSectionName, "%s%d", SECNAME_RES_PREFIX, (i + 1));

            ret = LoadResInfoByName(pConfig, hConfig, &(pScript->ResInfoTable[i]), resSectionName);
            if (OK != ret)
            {
                Err(__FUNCTION__, __FILE__, __LINE__, "LoadResInfoByName %d", ret);
                pConfig->CloseConfig(hConfig);
                return ret;
            }

            //----------------------------------------------------------
            // res ID : 目前在脚本中无该项内容，暂时顺利排列ID号
            // ID从1开始编号, 未来可能会采用其他方式获得ID号
            //----------------------------------------------------------
            pScript->ResInfoTable[i].ID = i + RES_ID_BASE;
        }
    */
    //----------------------------------------------------------
    // res items check
    //----------------------------------------------------------
    ret = ResItemsCheck(pScript->ResInfoTable, pScript->ResCount);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "res check failed %d", ret);
        pConfig->CloseConfig(hConfig);
        return ret;
    }

    return ret;

#undef __FUNCTION__
}

u32 GetSysInfo(HSCRIPT hScript, void *sysinfo, u32 size)
{
    u32 ret = OK;
    FACE_SCRIPT *pScript = NULL;

    if (NULL == hScript || NULL == sysinfo)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    // size大小不能小于SYSTEM_INFO结构体的大小
    //----------------------------------------------------------
    if (size < FACE_SYS_INFO_SIZE)
    {
        return __LINE__;
    }

    pScript = (FACE_SCRIPT *)hScript;

    memcpy(sysinfo, pScript->SysInfo, FACE_SYS_INFO_SIZE);
    return ret;
}

u32 GetResCount(HSCRIPT hScript, u32 *resCount)
{
    u32 ret = OK;
    FACE_SCRIPT *pScript = NULL;

    if (NULL == hScript || NULL == resCount)
    {
        return __LINE__;
    }

    pScript = (FACE_SCRIPT *)hScript;

    *resCount = pScript->ResCount;

    return ret;
}

u32 GetResInfo(HSCRIPT hScript, void *resinfo, u32 size, u32 index)
{
    u32 ret = OK;
    FACE_SCRIPT *pScript = NULL;

    if (NULL == hScript || NULL == resinfo)
    {
        return __LINE__;
    }
    pScript = (FACE_SCRIPT *)hScript;

    //----------------------------------------------------------
    // size不能小于RES_ITEM结构体的大小
    //----------------------------------------------------------
    if (size < FACE_RES_ITEM_SIZE)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    // index不能大于Res item个数
    //----------------------------------------------------------
    if (index > pScript->ResCount)
    {
        return __LINE__;
    }

    memcpy(resinfo, &(pScript->ResInfoTable[index]), FACE_RES_ITEM_SIZE);

    return ret;
}

static u32 UpdateSysInfo(CONFIG_IF *config_if, HCONFIG hConfig, FACE_SYS_INFO *sys_info)
{
#define __FUNCTION__ "UpdateSysInfo"


    u32 ret = OK;

    if (NULL == config_if || NULL == hConfig || NULL == sys_info)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    //  scriptver
    //----------------------------------------------------------
    ret = config_if->SetKeyValue(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_SCRIPTVER,
                                 (u32)sys_info->Scriptver, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "SetKeyValue(%s) faild %d", KEYNAME_SYS_SCRIPTVER, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  uistyle
    //----------------------------------------------------------
    ret = config_if->SetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_UISTYLE,
                                  (s8 *)sys_info->UIstyle,  0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "SetKeyString(%s) faild %d", KEYNAME_SYS_UISTYLE, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  name
    //----------------------------------------------------------
    ret = config_if->SetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_NAME,
                                  (s8 *)sys_info->Name, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "SetKeyString(%s) faild %d", KEYNAME_SYS_NAME, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  author
    //----------------------------------------------------------
    ret = config_if->SetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_AUTHOR,
                                  (s8 *)sys_info->Author, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "SetKeyString(%s) faild %d", KEYNAME_SYS_AUTHOR, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  solution
    //----------------------------------------------------------
    ret = config_if->SetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_SOLUTION,
                                  (s8 *)sys_info->Solution, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "SetKeyString(%s) faild %d", KEYNAME_SYS_SOLUTION, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  stylename
    //----------------------------------------------------------
    ret = config_if->SetKeyString(hConfig, (s8 *)SECNAME_SYSTEM, (s8 *)KEYNAME_SYS_STYLENAME,
                                  (s8 *)sys_info->Stylename, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__,
            "SetKeyString(%s) faild %d", KEYNAME_SYS_STYLENAME, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  StyleID : 目前未使用
    //----------------------------------------------------------

    return ret;

#undef __FUNCTION__
}

u32 SetSysInfo(HSCRIPT hScript, void *sysinfo, u32 size)
{
#define __FUNCTION__  "SetSysInfo"

    u32 ret = OK;
    FACE_SCRIPT *pScript = NULL;

    if (NULL == hScript || NULL == sysinfo)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    // size大小不能大于SYSTEM_INFO结构体的大小
    //----------------------------------------------------------
    if (size > FACE_SYS_INFO_SIZE)
    {
        return __LINE__;
    }

    pScript = (FACE_SCRIPT *)hScript;

    memcpy(pScript->SysInfo, sysinfo, FACE_SYS_INFO_SIZE);

    //----------------------------------------------------------
    // 更新系统资源信息到脚本文件
    //----------------------------------------------------------
    ret = UpdateSysInfo((CONFIG_IF *)(pScript->config_if), (HCONFIG)(pScript->hConfig), (FACE_SYS_INFO *)sysinfo);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "UpdateSysInfo: %d", ret);
    }
    return ret;

#undef __FUNCTION__
}


static u32 UpdateResInfoByName(CONFIG_IF *config_if, HCONFIG hConfig, FACE_RES_ITEM *res_info, s8 *sec_name)
{
#define __FUNCTION__  "UpdateResInfoByName"

    u32  ret = OK;

    if (NULL == config_if || NULL == hConfig ||
            NULL == res_info  || NULL == sec_name)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    //  name
    //----------------------------------------------------------
    if (RES_NAME_LEN < strlen((const char *)(res_info->Name)))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "res name too long");
        return ret;
    }
    ret = config_if->SetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_NAME,
                                  (s8 *)res_info->Name,  0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetKeyString(%s) faild %d", KEYNAME_RES_NAME, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  file
    //----------------------------------------------------------
    //scott close
    /*
    if (RES_NAME_LEN < strlen((const char *)(res_info->File)))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "file path too long");
        return ret;
    }
    */
    ret = config_if->SetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_FILE,
                                  (s8 *)res_info->File,  0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetKeyString(%s) faild %d", KEYNAME_RES_FILE, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  commentCN
    //----------------------------------------------------------
    if (COMMENT_LEN < strlen((const char *)(res_info->CommentCN)))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CommentCN too long");
        return __LINE__;
    }
    ret = config_if->SetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_COMMENTCN,
                                  (s8 *)res_info->CommentCN, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetKeyString(%s) faild %d", KEYNAME_RES_COMMENTCN, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  commentEN
    //----------------------------------------------------------
    if (COMMENT_LEN < strlen((const char *)(res_info->CommentEN)))
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CommentCN too long");
        return __LINE__;
    }
    ret = config_if->SetKeyString(hConfig, sec_name, (s8 *)KEYNAME_RES_COMMENTEN,
                                  (s8 *)res_info->CommentEN,  0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetKeyString(%s) faild %d", KEYNAME_RES_COMMENTEN, ret);
        return ret;
    }

    //----------------------------------------------------------
    //  ID : 目前未使用
    //----------------------------------------------------------

    //----------------------------------------------------------
    //  Size ： 不能更新
    //----------------------------------------------------------

    //----------------------------------------------------------
    //  Ver ： 不能更新
    //----------------------------------------------------------

    //----------------------------------------------------------
    //  res section name : 不能更新
    //----------------------------------------------------------
    return ret;

#undef __FUNCTION__
}

u32 SetResInfo(HSCRIPT hScript, void *resinfo, u32 size, u32 index)
{
#define __FUNCTION__  "SetResInfo"
    u32 ret = OK;
    FACE_SCRIPT *pScript = NULL;

    if (NULL == hScript || NULL == resinfo)
    {
        return __LINE__;
    }
    pScript = (FACE_SCRIPT *)hScript;

    //----------------------------------------------------------
    // size不能大于RES_ITEM结构体的大小
    //----------------------------------------------------------
    if (size > FACE_RES_ITEM_SIZE)
    {
        return __LINE__;
    }

    //----------------------------------------------------------
    // index不能大于Res item个数
    //----------------------------------------------------------
    if (index > pScript->ResCount)
    {
        return __LINE__;
    }

    memcpy(&(pScript->ResInfoTable[index]), resinfo, FACE_RES_ITEM_SIZE);

    //----------------------------------------------------------
    // 更新资源信息到脚本文件
    //----------------------------------------------------------
    ret = UpdateResInfoByName((CONFIG_IF *)(pScript->config_if), (HCONFIG)(pScript->hConfig), (FACE_RES_ITEM *)resinfo,
                              (s8 *)(pScript->ResInfoTable[index].Section));
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "UpdateResInfo: %d", ret);
    }

    return ret;

#undef __FUNCTION__
}

u32  CloseScript(HSCRIPT hScript)
{
#define __FUNCTION__ "CloseScript"
    u32 ret = OK;
    HCONFIG hConfig = NULL;
    CONFIG_IF *pConfig = NULL;
    FACE_SCRIPT *pScript = NULL;

    if (NULL == hScript)
    {
        return __LINE__;
    }
    pScript = (FACE_SCRIPT *)hScript;
    pConfig = (CONFIG_IF *)(pScript->config_if);
    hConfig = (HCONFIG)(pScript->hConfig);

    //----------------------------------------------------------
    // first : free system info space
    //----------------------------------------------------------
    if (pScript->SysInfo)
    {
        free(pScript->SysInfo);
        pScript->SysInfo = NULL;
    }

    //----------------------------------------------------------
    // second : free res info table space
    //----------------------------------------------------------
    if (pScript->ResInfoTable)
    {
        free(pScript->ResInfoTable);
        pScript->ResInfoTable = NULL;
    }

    //----------------------------------------------------------
    // third : close script file
    //----------------------------------------------------------
    ret = pConfig->CloseConfig(hConfig);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CloseConfig %d", ret);

        // close config fail , I think should continue
    }

    //----------------------------------------------------------
    // last : free script handle space
    //----------------------------------------------------------
    free(pScript);
    pScript = NULL;

    return ret;

#undef __FUNCTION__
}
