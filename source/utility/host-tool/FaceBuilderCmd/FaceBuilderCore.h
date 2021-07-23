//------------------------------------------------------------------------------------------------------------
//
// FaceBuilderCore.h
//
// 2009-11-4 14:06:13
//
// sunny
//
//------------------------------------------------------------------------------------------------------------

#ifndef     __FACE_BUILDER_CORE_H__
#define     __FACE_BUILDER_CORE_H__

#include "if/config_if.h"
//#include "afx.h"
#include "LiveFaceCore/FaceDocument.h"

class FaceBuilderCore
{
public:

    CPlugInWrapper m_ConfigWrapper;
    CONFIG_IF     *m_ConfigIf;
    int LoadConfigPlug(void);

    int  LoadConfigFile(char *filename);
    int  Buildbin(void);
    int  Buildface(void);

    CString m_FileFace;
    CString m_FileBin;
    CString m_FileScript;
    CString m_FileHead;

    FaceBuilderCore();

    virtual ~FaceBuilderCore();

protected:

    int CreateNULLScriptFile(void);
    int SetFaceScriptNULL(void);
    int SetFaceDocment(void);

    FaceDocument m_FaceDocument;
    char  m_FileNULLScript[MAX_PATH];
};

#endif  // __FACE_BUILDER_CORE_H__
