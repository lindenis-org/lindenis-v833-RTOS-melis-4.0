//------------------------------------------------------------------------------------------------------------
//
// FaceBuilderCore.cpp
//
// 2009-11-4 14:16:32
//
// sunny
//
//------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "FaceBuilderCore.h"

//#include "..\share\file.cpp"
//#include "..\share\error.cpp"
#include "PlugInWrapper/PlugInWrapper.h"
#include "error.h"
#include "if/config_if.h"
#include "if/scriptparser_if.h"
#include "if/facemaker_if.h"

#include "res.h"
#include "script_type.h"

extern BOOL IsRelatePath(LPCTSTR lpDir);
extern void GetParentDir(LPCTSTR lpFileName, LPTSTR lpDirRet);
extern void GetFullPath(LPCTSTR lpStandar, LPCTSTR lpRelatePath, LPTSTR lpRet);

extern int FaceBuilderIsFullName(const char *FilePath);
extern void FaceBuilderGetFullPath(char *dName, const char *sName);

extern void *GetConfigInterface();
extern void *GetScriptParserInterface();
extern void *GetFaceMakerInterface();

int FaceBuilderCore::Buildbin()
{
#define __FUNCTION__    "Buildbin"
    u32 i;
    u32 ret = OK;
    u32 resCount = 0;
    FACE_SYS_INFO SysInfo;
    FACE_RES_ITEM ResItem;

    CPlugInWrapper scriptParserWrapper;
    CPlugInWrapper FaceMakerWrapper;

    char szModuleDir[MAX_PATH];
    char szPlugPath[MAX_PATH];
    SCRIPTPARSER_IF *scriptparser_if = NULL;
    FACE_IF *face_if = NULL;

    HFACE hFace = NULL;
    HSCRIPT hScript = NULL;

    char szFullItemFile[MAX_PATH];
    char szTmpItem[MAX_PATH];

    if (m_ConfigIf == NULL)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "m_ConfigIf == NULL");
        return __LINE__;
    }

    //GetModuleDirectory(szModuleDir);
    getcwd(szModuleDir, MAX_PATH);

    //---------------------------------------------------------------
    //  Load scriptparser plug
    //---------------------------------------------------------------
    sprintf(szPlugPath, "%s/%s", szModuleDir, SCRIPTPARSER_PLUG);

//  ret = scriptParserWrapper.SetPlugInPath(szPlugPath);
//
//  ret = scriptParserWrapper.LoadPlugIn();

//  ret = scriptParserWrapper.QueryInterface(SCRIPTPARSER_PLUGIN_TYPE);

    //scriptparser_if = (SCRIPTPARSER_IF *)scriptParserWrapper.GetInterface(SCRIPTPARSER_PLUGIN_TYPE);
    scriptparser_if = (SCRIPTPARSER_IF *)GetScriptParserInterface();
    if (NULL == scriptparser_if)
    {
        goto Error_End ;
    }

    //---------------------------------------------------------------
    //  Load facemaker plug
    //---------------------------------------------------------------
    sprintf(szPlugPath, "%s/%s", szModuleDir, FACEMAKER_PLUG);

    //ret = FaceMakerWrapper.SetPlugInPath(szPlugPath);

    //ret = FaceMakerWrapper.LoadPlugIn();

    //ret = FaceMakerWrapper.QueryInterface(FACEMAKER_PLUGIN_TYPE);

    //face_if = (FACE_IF *)FaceMakerWrapper.GetInterface(FACEMAKER_PLUGIN_TYPE);
    face_if = (FACE_IF *)GetFaceMakerInterface();
    if (NULL == face_if)
    {
        goto Error_End ;
    }

    //---------------------------------------------------------------
    //  Open script
    //---------------------------------------------------------------
    hScript = scriptparser_if->OpenScript((char *)(LPSTR)(LPCTSTR)m_FileScript, m_ConfigIf);
    if (NULL == hScript)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "OpenScript failed");
        goto Error_End ;
    }

    ret = scriptparser_if->ParseScript(hScript);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "ParseScript %d", ret);
        goto Error_End;
    }

    //---------------------------------------------------------------
    //  Create face
    //---------------------------------------------------------------
    hFace = face_if->CreateFace((char *)(LPSTR)(LPCTSTR)m_FileBin,
                                (char *)(LPSTR)(LPCTSTR)m_FileHead);

    if (NULL == hFace)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CreateFace failed");
        goto Error_End ;
    }

    ret = scriptparser_if->GetSysInfo(hScript, (void *)(&SysInfo), FACE_SYS_INFO_SIZE);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "GetSysInfo %d", ret);
    }

    ret = scriptparser_if->SetSysInfo(hScript, (void *)(&SysInfo), FACE_SYS_INFO_SIZE);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetSysInfo %d", ret);
        goto Error_End ;
    }

    ret = face_if->SetSysInfo(hFace, (void *)(&SysInfo), FACE_SYS_INFO_SIZE);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetSysInfo %d", ret);
        goto Error_End ;
    }

    ret = scriptparser_if->GetResCount(hScript, &resCount);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "GetResCount %d", ret);
        goto Error_End ;
    }
    ret = face_if->SetResCount(hFace, resCount);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "SetResCount %d", ret);
        goto Error_End ;
    }

    for (i = 0; i < resCount; i++)
    {
        ret = scriptparser_if->GetResInfo(hScript, (void *)(&ResItem), FACE_RES_ITEM_SIZE, i);
        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "GetResInfo %d", ret);
            goto Error_End ;
        }

        ret = scriptparser_if->SetResInfo(hScript, (void *)(&ResItem), FACE_RES_ITEM_SIZE, i);
        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "SetResInfo %d", ret);
            goto Error_End ;
        }

        if (!FaceBuilderIsFullName(ResItem.File))
        {
            FaceBuilderGetFullPath(szFullItemFile, ResItem.File);
            strcpy(ResItem.File, szFullItemFile);
        }

        ret = face_if->SetResInfo(hFace, (void *)(&ResItem), FACE_RES_ITEM_SIZE, i);
        if (OK != ret)
        {
            Err(__FUNCTION__, __FILE__, __LINE__, "SetResInfo %d", ret);
            goto Error_End ;
        }
    }
    //---------------------------------------------------------------
    // Build face
    //---------------------------------------------------------------
    ret = face_if->BuildFace(hFace, NULL, NULL, 0);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "BuildFace %d", ret);
        goto Error_End ;
    }

    ret = scriptparser_if->CloseScript(hScript);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CloseScript %d", ret);
        goto Error_End ;
    }
    ret = face_if->CloseFace(hFace);
    if (OK != ret)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "CloseFace %d", ret);
        goto Error_End ;
    }

    return OK;

Error_End:

    return ret;

#undef __FUNCTION__
}

int FaceBuilderCore::Buildface()
{
    u32 ret = OK;
    CString str;

    //----------------------------------------------------------
    //  Initial m_FaceDocument
    //----------------------------------------------------------
    m_FaceDocument.Initial();

    CreateNULLScriptFile();

    SetFaceScriptNULL();
    SetFaceDocment();

    ret = m_FaceDocument.StoreFaceDocumet((char *)(LPCTSTR)m_FileFace);

    //----------------------------------------------------------
    //  删除临时脚本文件
    //----------------------------------------------------------
    remove(m_FileNULLScript);

    return ret;
}

int FaceBuilderCore::SetFaceDocment()
{
    //----------------------------------------------------------
    //  face.script
    //----------------------------------------------------------
    strcpy(m_FaceDocument.m_FaceScriptFile, m_FileNULLScript);

    //----------------------------------------------------------
    //  face.bin
    //----------------------------------------------------------
    strcpy(m_FaceDocument.m_FaceTemplate, (char *)(LPCTSTR)m_FileBin);

    return OK;
}

#define    COPY_BUFFER_SIZE     (16 * 1024)

int FaceBuilderCore::CreateNULLScriptFile()
{
    u32 len;
    FILE *hSource = NULL;
    FILE *hDest = NULL;
    char Buffer[COPY_BUFFER_SIZE];

    sprintf(m_FileNULLScript, "%s.NULL", (char *)(LPCTSTR)m_FileScript);

    hSource = fopen((char *)(LPCTSTR)m_FileScript, "rb");
    if (NULL == hSource)
    {
        Err("CreateNULLScriptFile", __FILE__, __LINE__, "fopen");
        return __LINE__;
    }
    hDest = fopen(m_FileNULLScript, "wb+");
    if (NULL == hDest)
    {
        Err("CreateNULLScriptFile", __FILE__, __LINE__, "fopen");
        return __LINE__;
    }
    //----------------------------------------------------------
    //  copy file data
    //----------------------------------------------------------
    while ((len = fread(Buffer, 1, COPY_BUFFER_SIZE, hSource)) != 0)
    {
        if (fwrite(Buffer, 1, len, hDest) != len)
        {
            Err("CreateNULLScriptFile", __FILE__, __LINE__, "fwrite");
            fclose(hSource);
            fclose(hDest);
            return __LINE__;
        }
    }
    fclose(hSource);
    fclose(hDest);

    return OK;
}

int FaceBuilderCore::SetFaceScriptNULL()
{
    u32 i;
    u32 ret = OK;
    u32 resCount = 0;
    FACE_SYS_INFO SysInfo;
    FACE_RES_ITEM ResItem;

    CPlugInWrapper scriptParserWrapper;

    char szModuleDir[MAX_PATH];
    char szPlugPath[MAX_PATH];
    SCRIPTPARSER_IF *scriptparser_if = NULL;
    HSCRIPT hScript = NULL;

    //GetModuleDirectory(szModuleDir);
    getcwd(szModuleDir, MAX_PATH);

    //---------------------------------------------------------------
    //  Load scriptparser plug
    //---------------------------------------------------------------
    sprintf(szPlugPath, "%s/%s", szModuleDir, SCRIPTPARSER_PLUG);

//  ret = scriptParserWrapper.SetPlugInPath(szPlugPath);

//  ret = scriptParserWrapper.LoadPlugIn();

//  ret = scriptParserWrapper.QueryInterface(SCRIPTPARSER_PLUGIN_TYPE);

//  scriptparser_if = (SCRIPTPARSER_IF *)scriptParserWrapper.GetInterface(SCRIPTPARSER_PLUGIN_TYPE);
    scriptparser_if = (SCRIPTPARSER_IF *)GetScriptParserInterface();
    if (NULL == scriptparser_if)
    {
        goto Error_End ;
    }

    //---------------------------------------------------------------
    //  Open script
    //---------------------------------------------------------------
    hScript = scriptparser_if->OpenScript(m_FileNULLScript, m_ConfigIf);
    if (NULL == hScript)
    {
        Err("SetFaceScriptNULL", __FILE__, __LINE__, "OpenScript failed");
        goto Error_End ;
    }

    ret = scriptparser_if->ParseScript(hScript);
    if (OK != ret)
    {
        Err("SetFaceScriptNULL", __FILE__, __LINE__, "ParseScript %d", ret);
        goto Error_End;
    }

    ret = scriptparser_if->GetSysInfo(hScript, (void *)(&SysInfo), FACE_SYS_INFO_SIZE);
    if (OK != ret)
    {
        Err("SetFaceScriptNULL", __FILE__, __LINE__, "GetSysInfo %d", ret);
    }

    ret = scriptparser_if->SetSysInfo(hScript, (void *)(&SysInfo), FACE_SYS_INFO_SIZE);
    if (OK != ret)
    {
        Err("SetFaceScriptNULL", __FILE__, __LINE__, "SetSysInfo %d", ret);
        goto Error_End ;
    }

    ret = scriptparser_if->GetResCount(hScript, &resCount);
    if (OK != ret)
    {
        Err("SetFaceScriptNULL", __FILE__, __LINE__, "GetResCount %d", ret);
        goto Error_End ;
    }
    for (i = 0; i < resCount; i++)
    {
        ret = scriptparser_if->GetResInfo(hScript, (void *)(&ResItem), FACE_RES_ITEM_SIZE, i);
        if (OK != ret)
        {
            Err("OnFaceMaker_test", __FILE__, __LINE__, "GetResInfo %d", ret);
            goto Error_End ;
        }

        // set res item file NULL
        strcpy(ResItem.File, "NULL");

        ret = scriptparser_if->SetResInfo(hScript, (void *)(&ResItem), FACE_RES_ITEM_SIZE, i);
        if (OK != ret)
        {
            Err("SetFaceScriptNULL", __FILE__, __LINE__, "SetResInfo %d", ret);
            goto Error_End ;
        }
    }
    ret = scriptparser_if->CloseScript(hScript);
    if (OK != ret)
    {
        Err("SetFaceScriptNULL", __FILE__, __LINE__, "CloseScript %d", ret);
        goto Error_End ;
    }

    return OK;

Error_End:
    return ret;
}

//------------------------------------------------------------------------------
//  FaceBuilder配置信息
//------------------------------------------------------------------------------
#define SECNAME_INFO                "info"              //
#define KEYNAME_INFO_SCRIPTNAME     "script"            //
#define KEYNAME_INFO_BINNAME        "bin"               //
#define KEYNAME_INFO_HEADNAME       "head"              //
#define KEYNAME_INFO_FACENAME       "face"              //

int FaceBuilderCore::LoadConfigFile(char *filename)
{
    int ret;
    char szValue[MAX_PATH];
    HCONFIG    hConfig = NULL;

    printf("filename %s\n", filename);

    if (m_ConfigIf == NULL)
    {
        Err("LoadConfigFile", __FILE__, __LINE__, "m_ConfigIf == NULL");
        return __LINE__;
    }
    hConfig = m_ConfigIf->OpenConfig(filename, NULL);
    if (NULL == hConfig)
    {
        Err("LoadConfigFile", __FILE__, __LINE__, "OpenConfig failed");
        printf("打开配置文件失败\n");
        return __LINE__;
    }

    //----------------------------------------------------------
    //  script filename
    //----------------------------------------------------------
    memset(szValue, 0, MAX_PATH);
    ret = m_ConfigIf->GetKeyString(hConfig, (s8 *)SECNAME_INFO,
                                   (s8 *)KEYNAME_INFO_SCRIPTNAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err("LoadConfigFile", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_INFO_SCRIPTNAME, ret);
        return ret;
    }
    if (!FaceBuilderIsFullName(szValue))
    {
        CString csTmp = szValue;
        //GetFullPath(filename, csTmp, szValue);
        FaceBuilderGetFullPath(szValue, csTmp);
    }
    //m_FileScript.Format("%s", szValue+'\0');
    m_FileScript = szValue;

    //  strcpy(m_FileScript, szValue);

    //----------------------------------------------------------
    //  Bin filename
    //----------------------------------------------------------
    memset(szValue, 0, MAX_PATH);
    ret = m_ConfigIf->GetKeyString(hConfig, (s8 *)SECNAME_INFO,
                                   (s8 *)KEYNAME_INFO_BINNAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err("LoadConfigFile", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_INFO_BINNAME, ret);
        return ret;
    }
    if (!FaceBuilderIsFullName(szValue))
    {
        CString csTmp = szValue;
        FaceBuilderGetFullPath(szValue, csTmp);
        //GetFullPath(filename, csTmp, szValue);
    }
    //m_FileBin.Format("%s", szValue);
    m_FileBin = szValue;
    //  strcpy(m_FileBin, szValue);

    //----------------------------------------------------------
    //  Head filename
    //----------------------------------------------------------
    memset(szValue, 0, MAX_PATH);
    ret = m_ConfigIf->GetKeyString(hConfig, (s8 *)SECNAME_INFO,
                                   (s8 *)KEYNAME_INFO_HEADNAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err("LoadConfigFile", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_INFO_HEADNAME, ret);
        return ret;
    }
    if (!FaceBuilderIsFullName(szValue))
    {
        CString csTmp = szValue;
        FaceBuilderGetFullPath(szValue, csTmp);
        //GetFullPath(filename, csTmp, szValue);
    }
    //m_FileHead.Format("%s", szValue);
    m_FileHead = szValue;
    printf("head full name:%s\n", szValue);
//  strcpy(m_FileHead, szValue);

    //----------------------------------------------------------
    //  Face filename
    //----------------------------------------------------------
    memset(szValue, 0, MAX_PATH);
    ret = m_ConfigIf->GetKeyString(hConfig, (s8 *)SECNAME_INFO,
                                   (s8 *)KEYNAME_INFO_FACENAME, (s8 *)szValue);
    if (OK != ret)
    {
        Err("LoadConfigFile", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", KEYNAME_INFO_FACENAME, ret);
        return ret;
    }
    if (!FaceBuilderIsFullName(szValue))
    {
        CString csTmp = szValue;
        FaceBuilderGetFullPath(szValue, csTmp);
        //GetFullPath(filename, csTmp, szValue);
    }
    //m_FileFace.Format("%s", szValue);
    m_FileFace = szValue;
    //  strcpy(m_FileFace, szValue);

    m_ConfigIf->CloseConfig(hConfig);
    return 0;
}

int FaceBuilderCore::LoadConfigPlug(void)
{
    int ret;

    char szModuleDir[MAX_PATH];
    char szPlugPath[MAX_PATH];

    //GetModuleDirectory(szModuleDir)
    getcwd(szModuleDir, MAX_PATH);

    //---------------------------------------------------------------
    //  Load config plug
    //---------------------------------------------------------------
    sprintf(szPlugPath, "%s/%s", szModuleDir, CONFIG_PLUG);

    //ret = m_ConfigWrapper.SetPlugInPath(szPlugPath);

    //ret = m_ConfigWrapper.LoadPlugIn();

    //ret = m_ConfigWrapper.QueryInterface(CONFIG_PLUGIN_TYPE);

    //m_ConfigIf = (CONFIG_IF *)m_ConfigWrapper.GetInterface(CONFIG_PLUGIN_TYPE);
    m_ConfigIf = (CONFIG_IF *)GetConfigInterface();
    if (NULL == m_ConfigIf)
    {
        return 1;
    }
    return 0;
}

FaceBuilderCore::FaceBuilderCore()
{
    m_ConfigIf = NULL;
}

FaceBuilderCore::~FaceBuilderCore()
{
    m_ConfigIf = NULL;
}
