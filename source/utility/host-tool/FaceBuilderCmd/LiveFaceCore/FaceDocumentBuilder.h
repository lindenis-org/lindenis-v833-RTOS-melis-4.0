// FaceDocumentBuilder.h: interface for the FaceDocumentBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEDOCUMENTBUILDER_H__9BCC5E35_55C0_4923_9550_077B9A2B7AB4__INCLUDED_)
#define AFX_FACEDOCUMENTBUILDER_H__9BCC5E35_55C0_4923_9550_077B9A2B7AB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlugInCenter/plugin.h"                            //

#include "FaceDocumentData.h"                               //


class FaceDocumentBuilder
{

public:

    u32 SetInterface(void *__if, const char *plug_guid);    //

    u32 ParserConfig(char *face_doc_config,
                     FaceDocumentData *pFaceDocumentData);  //

    u32 Build(FaceDocumentData *pFaceDocumentData);         //

    FaceDocumentBuilder();                                  //

    virtual ~FaceDocumentBuilder();                         //

protected:
    u32 BuildFaceImg(FaceDocumentData *pFaceDocumentData);  //

    u32 CheckItem(FaceDocumentData *pFaceDocumentData);     //


    u32 ParserItem(HCONFIG hConfig, int index,
                   FaceDocumentData *pFaceDocumentData);       //

    u32 ParserSystem(HCONFIG hConfig,
                     FaceDocumentData *pFaceDocumentData);       //


protected:

    IMGBUILDER_IF    *m_imgbuilder_if;                      //

    CONFIG_IF        *m_config_if;                          //

};

#endif // !defined(AFX_FACEDOCUMENTBUILDER_H__9BCC5E35_55C0_4923_9550_077B9A2B7AB4__INCLUDED_)
