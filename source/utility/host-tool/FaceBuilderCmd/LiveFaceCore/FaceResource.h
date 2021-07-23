// FaceResource.h: interface for the FaceResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACERESOURCE_H__77EBB1FF_29C5_4397_966A_F93260B90198__INCLUDED_)
#define AFX_FACERESOURCE_H__77EBB1FF_29C5_4397_966A_F93260B90198__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlugInCenter/plugin.h"                                //

class FaceResource
{

public:
    u32 SaveResAsFile(u16 ID, char *filepath);                  //

    u32 SetInterface(void *__if, const char *plug_guid);        //

    FaceResource(char *resource_path);                          //

    FaceResource();                                             //

    virtual ~FaceResource();                                    //

    u32 OpenResource(void);                                     //

    u32 OpenResource(char *resource_path);                      //

    u32 GetRes(u16 StyleID, u16 ID, void *Buffer, u32 Length);  //

    u32 GetResSize(u16 StyleID, u16 ID);                        //


protected:
    u32 CloseResource(void);                                    //

    char            m_ResourceFile[MAX_PATH];                   //

    RESACCESS_IF   *m_resAccess_if;                             //

    HRES            m_hRes;                                     //
};

#endif // !defined(AFX_FACERESOURCE_H__77EBB1FF_29C5_4397_966A_F93260B90198__INCLUDED_)
