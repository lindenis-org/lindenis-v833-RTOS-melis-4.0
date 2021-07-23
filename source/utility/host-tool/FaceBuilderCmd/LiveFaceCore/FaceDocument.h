// FaceDocument.h: interface for the FaceDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEDOCUMENT_H__89C36938_1B86_40F5_AD76_369CB5430BD7__INCLUDED_)
#define AFX_FACEDOCUMENT_H__89C36938_1B86_40F5_AD76_369CB5430BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "basetypes.h"
#include "../PlugInCenter/PlugInCenter.h"                           //

#include "FaceScript.h"                                             //

#include "FaceBuilder.h"                                            //

#include "FaceDocumentBuilder.h"                                    //

#include "FaceResource.h"                                           //

#include "FaceDocumentData.h"                                       //




class FaceDocument
{
public:
    char *GetFaceDocumentFile(void);                                //

    u32 BuildFace(char *resFile, u32 IsClone);                      //

    void Initial(void);                                             //

    FaceScriptData *GetFaceScriptData(void);                        //

    FaceResource *GetFaceResource(void);                            //

    char *GetModuleDir(void);                                       //

    FaceScriptData *GetScriptData(void);                            //

    u32 BuildFaceDocument(void);                                    //


    u32 LoadFaceDocumet(char *face_document, BOOL bTemplate);       //

    u32 StoreFaceDocumet(char *face_document);                      //

    u32 BuildFace(char *resFile,
                  char *resHeadFile,
                  char *script_path,
                  char *res_template,
                  u32 isClone);                                     //

    u32 ParserFaceScript(char *script_path);                        //

    FaceDocument();                                                 //

    virtual ~FaceDocument();                                        //


public:
    CPlugInCenter   m_PlugInCenter;                                 //

    char            m_szModuleDir[MAX_PATH];                        //

    FaceScript      m_FaceScript;                                   //

    FaceScriptData *m_FaceScriptData;                               //

    FaceResource    m_FaceResource;                                 //

    FaceBuilder     m_FaceBuilder;                                  //

    FaceDocumentBuilder m_FaceDocumentBuilder;                      //

    FaceDocumentData m_FaceDocumentData;                            //

    char            m_FaceDocumentFile[MAX_PATH];                   //face 模版文件

    char            m_FaceDir[MAX_PATH];                            //face 工作目录

    char            m_FaceDocumentCfg[MAX_PATH];                    //face.document 生成脚本

    char            m_FaceScriptFile[MAX_PATH];                     //face.script face配置脚本

    char            m_FaceTemplate[MAX_PATH];                       //face.bin    face模版资源文件

protected:
    void DeleteFaceTempFile(void);
    void *GetInterface(const char *plug_guid);                      //

    u32 GetFaceDocumentItem(IMGDECODE_IF   *imgdecode_if,
                            HIMAGE  hImage,
                            char *main, char *sub,
                            char *filepath);                        //


};

#endif // !defined(AFX_FACEDOCUMENT_H__89C36938_1B86_40F5_AD76_369CB5430BD7__INCLUDED_)
