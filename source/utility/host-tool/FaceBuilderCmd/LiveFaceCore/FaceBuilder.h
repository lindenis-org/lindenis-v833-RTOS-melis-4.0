// FaceBuilder.h: interface for the FaceBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEBUILDER_H__69462FEF_C51C_4627_868B_B061164EB11C__INCLUDED_)
#define AFX_FACEBUILDER_H__69462FEF_C51C_4627_868B_B061164EB11C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PlugInCenter/plugin.h"                                //

#include "FaceScriptData.h"                                     //


class FaceBuilder
{
public:
    u32 Build(FaceScriptData *pScriptData,
              char *resFile,
              char *resHeadFile,
              char *res_template,
              void *res_if,
              u32 isClone);                                   //

    u32 SetInterface(void *__if, const char *plug_guid);        //

    FaceBuilder();                                              //

    virtual ~FaceBuilder();                                     //

protected:

    FACE_IF      *m_face_if;                                    //

};

#endif // !defined(AFX_FACEBUILDER_H__69462FEF_C51C_4627_868B_B061164EB11C__INCLUDED_)
