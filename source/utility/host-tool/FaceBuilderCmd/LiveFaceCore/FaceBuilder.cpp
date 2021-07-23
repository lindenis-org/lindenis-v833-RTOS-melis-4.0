// FaceBuilder.cpp: implementation of the FaceBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"
//#include "LiveFace.h"
#include "FaceBuilder.h"

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
FaceBuilder::FaceBuilder()
{
    m_face_if = NULL;
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
FaceBuilder::~FaceBuilder()
{
    m_face_if = NULL;
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
u32 FaceBuilder::SetInterface(void *__if, const char *plug_guid)
{
    u32 ret = OK;

    if (NULL == __if)
        return __LINE__;

    if (GUID_LEN != strlen(plug_guid))
        return __LINE__;

    if (0 == strncmp(plug_guid, FACEMAKER_PLUGIN_TYPE, GUID_LEN))
    {
        m_face_if = (FACE_IF *)__if;
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

u32 FaceBuilder::Build(FaceScriptData *pScriptData,
                       char *resFile,
                       char *resHeadFile,
                       char *res_template,
                       void *res_if,
                       u32 isClone)
{
    u32     ret         = OK;
    HFACE   hface       = NULL;
    u32     resCount    = 0;
    u32     i           = 0;

    if (NULL == pScriptData || resFile == NULL || resHeadFile == NULL)
    {
        return __LINE__;
    }

    if (isClone)
    {
        if (NULL == res_template || NULL == res_if)
        {
            return __LINE__;
        }
    }

    if (NULL == m_face_if)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    resCount = pScriptData->GetResCount();
    if (0 == resCount)
    {
        printf("没有任何资源\n");
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (i = 0; i < resCount; i++)
    {
        FACE_RES_ITEM resInfo;
        ret = pScriptData->GetResInfo(&resInfo, i);
        if (OK != ret)
        {
            goto Error_End;
        }

        //--------------------------------------------------------------------------
        //资源图片不存在就提示用户
        //--------------------------------------------------------------------------
        if (0 != strcmp(resInfo.File, EMPTY_PATH))
        {
            if (OK != IsFileExist(resInfo.File))
            {
                CString str;
                str.Format("[%d] %s 不存在", i + 1, resInfo.File);
                printf("%s\n", str.GetBuffer(0));
                ret = __LINE__;
                goto Error_End;
            }
        }
        else
        {
            Msg("[%d] %s", i + 1, resInfo.File);
        }
    }


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    hface =  m_face_if->CreateFace(resFile,  resHeadFile);
    if (NULL == hface)
    {
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_face_if->SetSysInfo(hface, pScriptData->GetSysInfo(), sizeof(FACE_SYS_INFO));
    if (OK != ret)
    {
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_face_if->SetResCount(hface, resCount);
    if (OK != ret)
    {
        goto Error_End;
    }


    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (i = 0; i < resCount; i++)
    {
        FACE_RES_ITEM resInfo;
        ret = pScriptData->GetResInfo(&resInfo, i);
        if (OK != ret)
        {
            goto Error_End;
        }

        ret = m_face_if->SetResInfo(hface, &resInfo, sizeof(FACE_RES_ITEM), i);
        if (OK != ret)
        {
            goto Error_End;
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = m_face_if->BuildFace(hface, res_template, res_if, isClone);
    if (OK != ret)
    {
        Msg("Err:%s %d %d", __FILE__, __LINE__, ret);
        goto Error_End;
    }

    //--------------------------------------------------------------------------
    // 执行完成
    //--------------------------------------------------------------------------
    ret = m_face_if->CloseFace(hface);
    hface = NULL;

    return ret;

    //--------------------------------------------------------------------------
    // 出错处理
    //--------------------------------------------------------------------------
Error_End:

    if (NULL != hface)
    {
        m_face_if->CloseFace(hface);
        hface = NULL;
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
