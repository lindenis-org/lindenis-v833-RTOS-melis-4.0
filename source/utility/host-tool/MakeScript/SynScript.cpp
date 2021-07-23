//------------------------------------------------------------------------------------------------------------
//
//  SynScript.cpp
//
//  Update commentCN and commentEN from old script  to new script;
//  Dup new script to old script
//
//  2009-11-9 17:49:44
//
//  sunny
//
//------------------------------------------------------------------------------------------------------------

#ifndef  __SYN_SCRIPT_CPP__
#define  __SYN_SCRIPT_CPP__     1

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "basetypes.h"
#include "error.h"

#include "config_if.h"
#include "config.h"

#include "SynScript.h"

int SynScriptCore::LoadConfigPlug(void)
{

    // first check config plug load already or not
    if (NULL != m_ConfigIf)
    {
        Err("LoadConfigPlug", __FILE__, __LINE__, "config plug load already");
        return __LINE__;
    }

    //  Load config plug
    m_ConfigIf = (CONFIG_IF *)GetConfigInterface();
    if (NULL == m_ConfigIf)
    {
        return __LINE__;
    }
    return OK;
}

int SynScriptCore::OpenScriptFiles(const char *oldscript, const char *newscript)
{
    if ((NULL == oldscript) || (NULL == newscript))
    {
        return __LINE__;
    }

    // first check config plug load already or not
    if (NULL == m_ConfigIf)
    {
        Err("OpenScriptFiles", __FILE__, __LINE__, "config plug not load");
        return __LINE__;
    }

    // check script files open already or not
    if ((NULL != m_hOldScript) || (NULL != m_hNewScript))
    {
        Err("OpenScriptFiles", __FILE__, __LINE__, "script files open already");
        return __LINE__;
    }

    m_hOldScript = m_ConfigIf->OpenConfig((char *)oldscript, NULL);
    if (NULL == m_hOldScript)
    {
        Err("OpenScriptFiles", __FILE__, __LINE__, "open config file failed");
        return __LINE__;
    }

    m_hNewScript = m_ConfigIf->OpenConfig((char *)newscript, NULL);
    if (NULL == m_hNewScript)
    {
        Err("OpenScriptFiles", __FILE__, __LINE__, "open file failed");
        m_ConfigIf->CloseConfig(m_hOldScript);
        return __LINE__;
    }
    return OK;
}

#define COMMENTCN_KEYNAME   "commentCN"
#define COMMENTEN_KEYNAME   "commentEN"

int SynScriptCore::SynScriptResItemsComment(void)
{
#define __FUNCTION__    "SynScriptResItemsComment"

    s8 keyString[MAX_KEY];
    s8 SectionName[MAX_SECTION];
    u32 SectionCount = 0;
    POS pos = NULL;
    u32 res = OK;
    u32 cnt = 0;

    res = m_ConfigIf->GetSectionCount(m_hOldScript, &SectionCount);
    if (OK != res)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "GetSectionCount failed ! %d", res);
        return __LINE__;
    }
    pos = m_ConfigIf->GetFirstSection(m_hOldScript);
    if (NULL == pos)
    {
        Err(__FUNCTION__, __FILE__, __LINE__, "GetFirstSection failed !");
        return __LINE__;
    }
    // syn comment information from old script to new script
    for (u32 i = 0; i < SectionCount && pos != NULL; i++)
    {
        memset(SectionName, 0, MAX_SECTION);
        pos = m_ConfigIf->GetNextSection(m_hOldScript, pos, (s8 *)SectionName);
        if (pos == NULL)
        {
            break;
        }
        Msg("\n%d[%s]\n", i, SectionName);

        // syn commentCN information
        memset(keyString, 0, MAX_KEY);
        m_ConfigIf->GetKeyString(m_hOldScript, SectionName, (s8 *)COMMENTCN_KEYNAME, keyString);
        Msg("\nupdate [%s] %s\n", SectionName, keyString);
        m_ConfigIf->SetKeyString(m_hNewScript, SectionName, (s8 *)COMMENTCN_KEYNAME, keyString, 0);

        // syn commentEN information
        memset(keyString, 0, MAX_KEY);
        m_ConfigIf->GetKeyString(m_hOldScript, SectionName, (s8 *)COMMENTEN_KEYNAME, keyString);
        Msg("\nupdate [%s] %s\n", SectionName, keyString);
        m_ConfigIf->SetKeyString(m_hNewScript, SectionName, (s8 *)COMMENTEN_KEYNAME, keyString, 0);

        cnt++;
    }
    if (cnt != SectionCount)
    {
        Warn(__FUNCTION__, __FILE__, __LINE__, "Syn sections number not match");
    }
    return OK;

#undef __FUNCTION__
}


int SynScriptCore::SynScriptResItems(void)
{
    u32 res = OK;

    // config plug must been loaded already
    if (NULL == m_ConfigIf)
    {
        Err("SynScriptResItemsComment", __FILE__, __LINE__, "config plug not load");
        return __LINE__;
    }

    // script files must been opened already
    if ((NULL == m_hOldScript) || (NULL == m_hNewScript))
    {
        Err("UpdateScriptResItems", __FILE__, __LINE__, "script files not open");
        return __LINE__;
    }

    // update script res item commnets
    res = SynScriptResItemsComment();
    if (res)
    {
        Err("UpdateScriptResItems", __FILE__, __LINE__, "Syn script comment failed");
        return __LINE__;
    }

    return OK;
}

int SynScriptCore::CloseScriptFiles(void)
{
    // first check config plug load already or not
    if (NULL == m_ConfigIf)
    {
        Err("CloseScriptFiles", __FILE__, __LINE__, "config plug not load");
        return __LINE__;
    }

    if ((NULL == m_hOldScript) || (NULL == m_hNewScript))
    {
        Err("OpenScriptFiles", __FILE__, __LINE__, "script files close already");
        return __LINE__;
    }

    m_ConfigIf->CloseConfig(m_hOldScript);
    m_ConfigIf->CloseConfig(m_hNewScript);

    return OK;
}

SynScriptCore::SynScriptCore()
{
    m_hOldScript = NULL;
    m_hNewScript = NULL;
    m_ConfigIf  = NULL;
}

SynScriptCore::~SynScriptCore()
{
    m_hOldScript = NULL;
    m_hNewScript = NULL;
    m_ConfigIf  = NULL;
}

#endif      // __SYN_SCRIPT_CPP__
