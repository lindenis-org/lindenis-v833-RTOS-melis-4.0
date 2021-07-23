// FaceDocumentBuilder.cpp: implementation of the FaceDocumentBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
//#include "LiveFace.h"

#include "FaceDocumentBuilder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "face_document_config.h"

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

long hexstr2dword(char *hexstr)
{
    long ul = 0;
    char *stopstring;
    ul = strtoul(hexstr, &stopstring, 16);
    return ul;
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
FaceDocumentBuilder::FaceDocumentBuilder()
{
    m_config_if = NULL;

    m_imgbuilder_if = NULL;

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
FaceDocumentBuilder::~FaceDocumentBuilder()
{


    m_config_if = NULL;

    m_imgbuilder_if = NULL;

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
u32 FaceDocumentBuilder::SetInterface(void *__if, const char *plug_guid)
{
    u32 ret = OK;

    if (NULL == __if)
    {
        Msg("NULL == __if %s", plug_guid);
        return __LINE__;
    }

    if (GUID_LEN != strlen(plug_guid))
    {
        return __LINE__;
    }

    if (0 == strncmp(plug_guid, CONFIG_PLUGIN_TYPE, GUID_LEN))
    {
        m_config_if = (CONFIG_IF *)__if;
    }
    else if (0 == strncmp(plug_guid, IMAGEBUILDER_PLUGIN_TYPE, GUID_LEN))
    {
        m_imgbuilder_if = (IMGBUILDER_IF *)__if;
    }
    else
    {
        Msg("invalid at %s %d", __FILE__, __LINE__);
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
u32 FaceDocumentBuilder::ParserConfig(char *face_doc_config, FaceDocumentData *pFaceDocumentData)
{
    u32 ret = OK;
    HCONFIG hConfig = NULL;
    int i = 0;

    if (NULL == m_config_if)
    {
        printf("please set config if\n");
        return __LINE__;
    }

    if (NULL == face_doc_config)
    {
        return __LINE__;
    }

    if (NULL == pFaceDocumentData)
    {
        return __LINE__;
    }

    if (OK != IsFileExist(face_doc_config))
    {
        Msg("%s not exist !", face_doc_config);
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    pFaceDocumentData->Release();

    //--------------------------------------------------------------------------
    // open script file
    //--------------------------------------------------------------------------
    hConfig = m_config_if->OpenConfig(face_doc_config, NULL);
    if (NULL == hConfig)
    {
        Err("ParserConfig", __FILE__, __LINE__, "OpenConfig %d", ret);
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = ParserSystem(hConfig, pFaceDocumentData);
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = pFaceDocumentData->AllocItemArray();
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (i = 0; i < (int)pFaceDocumentData->GetSystemData()->filecnt; i++)
    {
        ret = ParserItem(hConfig, i, pFaceDocumentData);
        if (OK != ret)
        {
            goto Error_End;
        }
    }

    //--------------------------------------------------------------------------
    // close script file
    //--------------------------------------------------------------------------
    ret = m_config_if->CloseConfig(hConfig);
    hConfig = NULL;

    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:

    if (NULL != hConfig)
    {
        m_config_if->CloseConfig(hConfig);
        hConfig = NULL;
    }

    Msg("ParserConfig_error");

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
u32 FaceDocumentBuilder::ParserSystem(HCONFIG hConfig, FaceDocumentData *pFaceDocumentData)
{
    u32 ret = OK;
    char szValue[MAX_VALUE];
    s8 *pSet = NULL;
    s8 *pKey = NULL;
    SYSTEM_HEAD *pSystemData = NULL;

    if (NULL == hConfig)
    {
        return __LINE__;
    }

    if (NULL == pFaceDocumentData)
    {
        return __LINE__;
    }

    pSystemData = pFaceDocumentData->GetSystemData();

    pSet = (s8 *)SET_SYSTEM;
    //--------------------------------------------------------------------------
    //  version
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_VERSION;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    pSystemData->version = hexstr2dword(szValue);

    //--------------------------------------------------------------------------
    //  pid
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_PID;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    pSystemData->pid = hexstr2dword(szValue);

    //--------------------------------------------------------------------------
    //  vid
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_VID;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    pSystemData->vid = hexstr2dword(szValue);


    //--------------------------------------------------------------------------
    //  hid
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_HID;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    pSystemData->hid = hexstr2dword(szValue);

    //--------------------------------------------------------------------------
    //  fid
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_FID;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    pSystemData->fid = hexstr2dword(szValue);

    //--------------------------------------------------------------------------
    //  image
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_FACEFILE;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    sprintf((char *)pSystemData->image, "%s", szValue);

    //--------------------------------------------------------------------------
    //  filecnt
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_COUNT;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserConfig", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    pSystemData->filecnt = atoi(szValue);

    //pFaceDocumentData->DumpSystemData();

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
u32 FaceDocumentBuilder::ParserItem(HCONFIG hConfig, int index, FaceDocumentData *pFaceDocumentData)
{
    u32 ret = OK;
    char szSet[MAX_VALUE];
    char szValue[MAX_VALUE];
    s8 *pSet = NULL;
    s8 *pKey = NULL;
    ITEM_INFO *pItemInfo = NULL;

    if (NULL == hConfig)
    {
        return __LINE__;
    }
    if (NULL == pFaceDocumentData)
    {
        return __LINE__;
    }

    pItemInfo = pFaceDocumentData->GetItemData(index);

    sprintf(szSet, "%s_%d", SET_ITEM, index);
    pSet = (s8 *)szSet;
    //--------------------------------------------------------------------------
    //  main
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_MAIN;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserItem", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    strncpy((char *)pItemInfo->main, szValue, 8);


    //--------------------------------------------------------------------------
    //  sub
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_SUB;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserItem", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    strncpy((char *)pItemInfo->sub, szValue, 16);


    //--------------------------------------------------------------------------
    //  file
    //--------------------------------------------------------------------------
    memset(szValue, 0, MAX_VALUE);
    pKey = (s8 *)KEY_FILE;
    ret = m_config_if->GetKeyString(hConfig, pSet, pKey, (s8 *)szValue);
    if (OK != ret)
    {
        Err("ParserItem", __FILE__, __LINE__,
            "GetKeyString(%s) faild %d", pKey, ret);
        return ret;
    }
    sprintf((char *)pItemInfo->filepath, "%s", szValue);


    //pFaceDocumentData->DumpItem(index);
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
u32 FaceDocumentBuilder::CheckItem(FaceDocumentData *pFaceDocumentData)
{
    u32 ret = OK;
    SYSTEM_HEAD *pSystemData = NULL;
    ITEM_INFO *pItemInfo = NULL;

    if (NULL == pFaceDocumentData)
    {
        return __LINE__;
    }

    pSystemData = pFaceDocumentData->GetSystemData();

    if (0 == strlen((const char *)pSystemData->image))
    {
        return __LINE__;
    }

    if (0 == pSystemData->filecnt)
    {
        return __LINE__;
    }


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (u32 i = 0 ; i < pSystemData->filecnt; i++)
    {
        pItemInfo = pFaceDocumentData->GetItemData(i);

        if (OK != IsFileExist((char *)pItemInfo->filepath))
        {
            Msg("%s not exist", pItemInfo->filepath);

            return __LINE__;
        }
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
u32 FaceDocumentBuilder::BuildFaceImg(FaceDocumentData *pFaceDocumentData)
{
    u32           ret         = OK;
    ITEM_INFO    *pItemInfo   = NULL;
    SYSTEM_HEAD *pSystemData = NULL;

    if (NULL == pFaceDocumentData)
    {
        return __LINE__;
    }

    pSystemData = pFaceDocumentData->GetSystemData();

    if (OK == IsFileExist((char *)pSystemData->image))
    {
        remove((const char *)pSystemData->image);
        Msg("BuildFaceImg:remove %s\n", (const char *)pSystemData->image);
    }

    if (NULL == m_imgbuilder_if)
    {
        return __LINE__;
    }

    Msg("FaceDocumentBuilder::BuildFaceImg dump info ......\n");
    pFaceDocumentData->Dump();
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_imgbuilder_if->SetHeadInfo(pSystemData->version,
                                       pSystemData->pid,
                                       pSystemData->vid,
                                       pSystemData->hid,
                                       pSystemData->fid,
                                       pSystemData->filecnt,
                                       pSystemData->image);
    if (OK != ret)
    {
        return ret;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (u32 i = 0 ; i < pSystemData->filecnt; i++)
    {
        pItemInfo = pFaceDocumentData->GetItemData(i);

        ret = m_imgbuilder_if->SetItemInfo(i,
                                           pItemInfo->filepath,
                                           pItemInfo->main,
                                           pItemInfo->sub);
        if (OK != ret)
        {
            return ret;
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_imgbuilder_if->BuildImage();

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
u32 FaceDocumentBuilder::Build(FaceDocumentData *pFaceDocumentData)
{
    u32 ret = OK;

    if (NULL == pFaceDocumentData)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    // check item file
    //--------------------------------------------------------------------------
    ret = CheckItem(pFaceDocumentData);
    if (OK != ret)
    {
        Msg("%s %d err", __FILE__, __LINE__);
        return ret;
    }

    //--------------------------------------------------------------------------
    // build face (img file)
    //--------------------------------------------------------------------------
    ret = BuildFaceImg(pFaceDocumentData);
    if (OK != ret)
    {
        Msg("%s %d err", __FILE__, __LINE__);
        return ret;
    }

    //Release();

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
