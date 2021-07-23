// FaceScriptData.cpp: implementation of the FaceScriptData class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
//#include "LiveFace.h"

#include "FaceScriptData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
FaceScriptData::FaceScriptData()
{
    memset(&m_sysinfo, 0, sizeof(FACE_SYS_INFO));

    m_resinfo = NULL;

    m_resCount = 0;
}


//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
FaceScriptData::~FaceScriptData()
{
    Release();
}


//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
void FaceScriptData::Release()
{
    if (NULL != m_resinfo)
    {
        free(m_resinfo);
        m_resinfo = NULL;
        m_resCount = 0;

        memset(&m_sysinfo, 0, sizeof(m_sysinfo));
    }
}

//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
FACE_SYS_INFO *FaceScriptData::GetSysInfo()
{
    return &m_sysinfo;
}

//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
u32 FaceScriptData::GetResCount()
{
    return m_resCount;
}

//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
u32 FaceScriptData::GetResInfo(FACE_RES_ITEM *resInfo, u32 index)
{
    if (NULL == resInfo)
        return __LINE__;

    if (index >= m_resCount)
        return __LINE__;

    memcpy(resInfo, &m_resinfo[index], sizeof(FACE_RES_ITEM));

    return OK;

}
//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------

u32 FaceScriptData::SetSysInfo(FACE_SYS_INFO *sysinfo)
{
    if (NULL == sysinfo)
        return __LINE__;

    memcpy(&m_sysinfo, sysinfo, sizeof(FACE_SYS_INFO));

    return OK;
}
//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
u32 FaceScriptData::SetResCount(u32 resCount)
{
    if (0 == resCount)
        return __LINE__;

    if (NULL != m_resinfo)
    {
        free(m_resinfo);
        m_resinfo = NULL;
        m_resCount = 0;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    m_resCount = resCount;
    m_resinfo = (FACE_RES_ITEM *)malloc(m_resCount * sizeof(FACE_RES_ITEM));
    if (NULL == m_resinfo)
    {
        m_resCount = (u32)NULL;
        return __LINE__;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    memset(m_resinfo, 0, m_resCount * sizeof(FACE_RES_ITEM));

    return OK;
}
//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------

u32 FaceScriptData::SetResInfo(FACE_RES_ITEM *resInfo, u32 index)
{
    if (NULL == resInfo)
        return __LINE__;

    if (index >= m_resCount)
        return __LINE__;


    memcpy(&m_resinfo[index], resInfo, sizeof(FACE_RES_ITEM));

    return OK;

}
//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------
u32 FaceScriptData::Dump()
{
    u32 ret = OK;
    u32 i = 0;
    FACE_RES_ITEM *resInfo = NULL;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    Msg("Scriptver=%x", m_sysinfo.Scriptver);
    Msg("UIstyle  =%s", m_sysinfo.UIstyle);
    Msg("Name     =%s", m_sysinfo.Name);
    Msg("Author   =%s", m_sysinfo.Author);
    Msg("Solution =%s", m_sysinfo.Solution);
    Msg("Stylename=%s", m_sysinfo.Stylename);
    Msg("StyleID  =%x", m_sysinfo.StyleID);
    Msg("\n");

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    for (i = 0; i < m_resCount; i++)
    {
        resInfo = m_resinfo + i;
        Msg("[%d]", i);
        Msg("Ver	=%x", resInfo->Ver);
        Msg("Size	=%x", resInfo->Size);
        Msg("ID		=%x", resInfo->ID);
        Msg("Section=%s", resInfo->Section);
        Msg("Name	=%s", resInfo->Name);
        Msg("File	=%s", resInfo->File);
        Msg("CommentCN=%s", resInfo->CommentCN);
        Msg("CommentEN=%s", resInfo->CommentEN);
        Msg("\n");
    }


    return ret;
}
//------------------------------------------------------------------------------------------------------------
//
// ����˵��
//
//
// ����˵��
//
//
// ����ֵ
//
//
// ����
//    ��
//
//------------------------------------------------------------------------------------------------------------