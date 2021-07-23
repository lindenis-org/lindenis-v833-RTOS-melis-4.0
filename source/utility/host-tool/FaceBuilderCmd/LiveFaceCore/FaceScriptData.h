// FaceScriptData.h: interface for the FaceScriptData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACESCRIPTDATA_H__B83926AF_BD9A_4B39_A1E3_3290BF25CA95__INCLUDED_)
#define AFX_FACESCRIPTDATA_H__B83926AF_BD9A_4B39_A1E3_3290BF25CA95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "basetypes.h"

#include "script_type.h"                            //

class FaceScriptData
{
public:
    u32 Dump(void);                                             //

    u32 SetResInfo(FACE_RES_ITEM *resInfo, u32 index);          //

    u32 SetResCount(u32 resCount);                              //

    u32 SetSysInfo(FACE_SYS_INFO *sysinfo);                     //

    u32 GetResInfo(FACE_RES_ITEM *resInfo, u32 index);          //

    u32 GetResCount(void);                                      //

    FACE_SYS_INFO *GetSysInfo(void);                            //

    void Release(void);                                         //

    FaceScriptData();                                           //

    virtual ~FaceScriptData();                                  //

protected:

    FACE_SYS_INFO      m_sysinfo;                               //

    FACE_RES_ITEM     *m_resinfo;                               //

    u32                m_resCount;                              //
};

#endif // !defined(AFX_FACESCRIPTDATA_H__B83926AF_BD9A_4B39_A1E3_3290BF25CA95__INCLUDED_)
