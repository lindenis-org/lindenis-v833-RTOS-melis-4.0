// PlugInWrapper.h: interface for the CPlugInWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINWRAPPER_H__7EAF9213_FE7A_4264_9094_579C3EBF8B3E__INCLUDED_)
#define AFX_PLUGINWRAPPER_H__7EAF9213_FE7A_4264_9094_579C3EBF8B3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "basetypes.h"

#include "plug_shell.h"


class CPlugInWrapper
{
public:
    //
    CPlugInWrapper(const char *plug_path);

    //
    CPlugInWrapper();

    //
    virtual ~CPlugInWrapper();

    //
    u32 SetPlugInPath(const char *plug_path);

    //
    char *GetPlugInPath();

    //
    HPLUG GetPlugInHandle();

    //
    void *GetInterface(const char *plug_guid);

    //
    u32 QueryInterface(const char *plug_guid);

    //
    Common_t *GetCommonInterface();

    //
    virtual u32 UnLoadPlugIn();

    //
    virtual u32 LoadPlugIn();


protected:

    char            m_plug_path[MAX_PATH];  //

    HPLUG           m_hPlug;                //
};

#endif // !defined(AFX_PLUGINWRAPPER_H__7EAF9213_FE7A_4264_9094_579C3EBF8B3E__INCLUDED_)
