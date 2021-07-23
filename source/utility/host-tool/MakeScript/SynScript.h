//------------------------------------------------------------------------------------------------------------
//
//  SynScript.h
//
//  2009-11-9 17:54:52
//
//  sunny
//
//------------------------------------------------------------------------------------------------------------

#ifndef     __SYN_SCRIPT_H__
#define     __SYN_SCRIPT_H__

#include "config_if.h"

class SynScriptCore
{
public:

    int  LoadConfigPlug(void);                                          //
    int  OpenScriptFiles(const char *oldscript, const char *newscript); //
    int  SynScriptResItems(void);                                       //
    int  CloseScriptFiles(void);                                        //

    SynScriptCore();
    virtual ~SynScriptCore();

protected:

    //CPlugInWrapper     m_ConfigWrapper;
    CONFIG_IF       *m_ConfigIf;
    char             buffer[1024];

    HCONFIG          m_hNewScript;
    HCONFIG          m_hOldScript;

    int  SynScriptResItemsComment(void);
};

#endif  // __SYN_SCRIPT_H__
