// FaceScript.cpp: implementation of the FaceScript class.
//
//////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

#include "FaceScript.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int IsFileExist(char *pathname);

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
FaceScript::FaceScript()
{
    m_script_if = NULL;


}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
FaceScript::~FaceScript()
{
    m_script_if = NULL;

    Release();
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
u32 FaceScript::SetInterface(void *__if, const char *plug_guid)
{
    u32 ret = OK;

    if (NULL == __if)
    {
        return __LINE__;
    }

    if (GUID_LEN != strlen(plug_guid))
        return __LINE__;

    if (0 == strncmp(plug_guid, SCRIPTPARSER_PLUGIN_TYPE, GUID_LEN))
    {
        m_script_if = (SCRIPTPARSER_IF *)__if;
    }

    return ret;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
u32 FaceScript::ScriptParser(char *script_path, void *config_if)
{
    u32 ret = (u32)NULL;

    if (NULL == m_script_if)
        return __LINE__;

    ret = IsFileExist(script_path);
    if (OK != ret)
        return __LINE__;

    if (NULL == config_if)
        return __LINE__;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    Release();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = __ScriptParser(script_path, config_if);

    return ret;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
void FaceScript::Release()
{
    m_ScriptData.Release();
}

u32 FaceScript::__ScriptParser(char *script_path, void *config_if)
{
    u32 ret = OK;
    u32 i = 0;

    //--------------------------------------------------------------------------
    //script_if ...
    //--------------------------------------------------------------------------
    SCRIPTPARSER_IF *script_if = NULL;
    HSCRIPT hscript = NULL;

    FACE_SYS_INFO      sysinfo;
    FACE_RES_ITEM      resinfo;
    u32                resCount;

    if (NULL == m_script_if)
    {
        printf("Please set script if first !\n");
        return __LINE__;
    }

    script_if = m_script_if;
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    hscript = script_if->OpenScript(script_path,  config_if);
    if (NULL == hscript)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = script_if->ParseScript(hscript);
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = script_if->GetSysInfo(hscript, &sysinfo, sizeof(FACE_SYS_INFO));
    if (OK != ret)
    {
        goto Error_End;
    }

    m_ScriptData.SetSysInfo(&sysinfo);
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = script_if->GetResCount(hscript, &resCount);
    if (OK != ret)
    {
        goto Error_End;
    }

    ret = m_ScriptData.SetResCount(resCount);
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (i = 0; i < resCount; i++)
    {
        ret = script_if->GetResInfo(hscript, &resinfo, sizeof(FACE_RES_ITEM), i);
        if (OK != ret)
        {
            goto Error_End;
        }

        ret = m_ScriptData.SetResInfo(&resinfo, i);
        if (OK != ret)
        {
            goto Error_End;
        }

        /*
        Msg("[_ScriptParser_%d]", i);
        DumpResInfo(&resinfo);
        */
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    script_if->CloseScript(hscript);
    hscript = NULL;
    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:
    if (NULL != hscript)
    {
        script_if->CloseScript(hscript);
        hscript = NULL;
    }

    Release();

    Msg("__ScriptParser error %d", ret);

    return ret;
}




//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
FaceScriptData *FaceScript::GetScriptData()
{
    return &m_ScriptData;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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

u32 FaceScript::SaveDataToScript(char *script_path, void *config_if)
{
    return SaveDataToScript(script_path, &m_ScriptData, config_if);
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
u32 FaceScript::SaveDataToScript(char *script_path, FaceScriptData *pScriptData, void *config_if)
{
    u32 ret = OK;
    CString str;

    if (NULL == script_path)
    {
        return __LINE__;
    }

    if (NULL == pScriptData)
    {
        return __LINE__;
    }

    if (OK != IsFileExist(script_path))
    {

        str.Format("%s Not Exist ! Please check the file First!", script_path);
        printf("%s\n", str.GetBuffer(0));
        return __LINE__;
    }

    if (NULL == m_script_if)
    {
        printf("Please set script if first !\n");
        return __LINE__;
    }

    if (NULL == config_if)
    {
        printf("config if is NULL\n");
        return __LINE__;
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------

    return __SaveDataToScript(script_path, pScriptData, config_if);

}



//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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

u32 FaceScript::__SaveDataToScript(char *script_path, FaceScriptData *pScriptData, void *config_if)
{

    u32 ret = OK;
    u32 i = 0;

    //Msg("__SaveDataToScript: dump pScriptData %s", script_path);
    //pScriptData->Dump();

    //--------------------------------------------------------------------------
    //script_if ...
    //--------------------------------------------------------------------------
    SCRIPTPARSER_IF *script_if = NULL;
    HSCRIPT hscript = NULL;

    FACE_RES_ITEM      resinfo;

    script_if = m_script_if;
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    hscript = script_if->OpenScript(script_path,  config_if);
    if (NULL == hscript)
    {
        return __LINE__;
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = script_if->ParseScript(hscript);
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = script_if->SetSysInfo(hscript, m_ScriptData.GetSysInfo(), sizeof(FACE_SYS_INFO));
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (i = 0; i < m_ScriptData.GetResCount(); i++)
    {
        memset(&resinfo, 0, sizeof(FACE_RES_ITEM));

        ret = m_ScriptData.GetResInfo(&resinfo, i);
        if (OK != ret)
        {
            goto Error_End;
        }

        ret = script_if->SetResInfo(hscript, &resinfo, sizeof(FACE_RES_ITEM), i);
        if (OK != ret)
        {
            goto Error_End;
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    script_if->CloseScript(hscript);
    hscript = NULL;

    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:
    if (NULL != hscript)
    {
        script_if->CloseScript(hscript);
        hscript = NULL;
    }

    return ret;

}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
void FaceScript::DumpResInfo(FACE_RES_ITEM *pResInfo)
{
    if (NULL == pResInfo)
    {
        return ;
    }

    Msg("[DumpResInfo]");
    Msg("Ver	=%x",  pResInfo->Ver);
    Msg("Size	=%x", pResInfo->Size);
    Msg("ID		=%x",  pResInfo->ID);
    Msg("Section=%s",   pResInfo->Section);
    Msg("Name	=%s", pResInfo->Name);
    Msg("File	=%s", pResInfo->File);
    Msg("CommentCN=%s", pResInfo->CommentCN);
    Msg("CommentEN=%s", pResInfo->CommentEN);
    Msg("\n");

    return ;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
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
