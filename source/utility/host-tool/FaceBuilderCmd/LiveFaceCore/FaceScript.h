// FaceScript.h: interface for the FaceScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACESCRIPT_H__D196CCBE_70E2_49A4_B378_D8668D2CC8FE__INCLUDED_)
#define AFX_FACESCRIPT_H__D196CCBE_70E2_49A4_B378_D8668D2CC8FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "PlugInCenter/plugin.h"                                //

#include "FaceScriptData.h"                                     //

class FaceScript
{
public:
    u32 SaveDataToScript(char *script_path, void *config_if);   //

    u32 SaveDataToScript(char *script_path,
                         FaceScriptData *pScriptData,
                         void *config_if);                      //

    FaceScriptData *GetScriptData(void);                        //

    u32 ScriptParser(char *script_path, void *config_if);       //

    u32 SetInterface(void *__if, const char *plug_guid);        //

    FaceScript();                                               //

    virtual ~FaceScript();                                      //

protected:
    u32 __ScriptParser(char *script_path, void *config_if);     //

    void Release(void);                                         //


    void DumpResInfo(FACE_RES_ITEM *pResInfo);                  //

    u32 __SaveDataToScript(char *script_path,
                           FaceScriptData *pScriptData,
                           void *config_if);                       //

protected:

    SCRIPTPARSER_IF   *m_script_if;                             //

    FaceScriptData     m_ScriptData;                            //
};

#endif // !defined(AFX_FACESCRIPT_H__D196CCBE_70E2_49A4_B378_D8668D2CC8FE__INCLUDED_)
