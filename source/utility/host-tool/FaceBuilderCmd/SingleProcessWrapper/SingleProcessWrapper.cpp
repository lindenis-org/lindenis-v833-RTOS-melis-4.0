// SingleProcessWrapper.cpp: implementation of the CSingleProcessWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "basetypes.h"
#include "SingleProcessWrapper.h"
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
CSingleProcessWrapper::CSingleProcessWrapper()
{
    m_hSingle = NULL;

    //CPlugInWrapper::CPlugInWrapper();
}


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
CSingleProcessWrapper::~CSingleProcessWrapper()
{
    u32 ret = OK;
    SINGLEPROCESS_IF *pIf = NULL;

    if (m_hSingle)
    {

        ret = QueryInterface(SINGLEPROCESS_PLUGIN_TYPE);
        if (OK != ret)
        {
            goto Error_End ;
        }

        pIf = (SINGLEPROCESS_IF *)GetInterface(SINGLEPROCESS_PLUGIN_TYPE);
        if (NULL == pIf)
        {
            goto Error_End ;
        }

        ret = pIf->SingleProcessClose(m_hSingle);
        if (OK != ret)
        {
            goto Error_End ;
        }

        m_hSingle = NULL;
    }


Error_End:

    if (GetPlugInHandle())
    {
        UnLoadPlugIn();
    }

    return ;
}


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
CSingleProcessWrapper::CSingleProcessWrapper(const char *plug_path): CPlugInWrapper(plug_path)
{
    m_hSingle = NULL;
}


//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
u32 CSingleProcessWrapper::IsSingle(char *ID)
{
    u32 ret = OK;
    SINGLEPROCESS_IF *pIF = NULL;

    if (!ID)
        return __LINE__;

    if (strlen(ID) > 256)
        return __LINE__;

    if (!GetPlugInHandle())
        return __LINE__;

    ret = QueryInterface(SINGLEPROCESS_PLUGIN_TYPE);
    if (OK != ret)
        return ret;

    pIF = (SINGLEPROCESS_IF *)GetInterface(SINGLEPROCESS_PLUGIN_TYPE);
    if (!pIF)
        return __LINE__;


    //----------------------------------------------------------
    //
    //----------------------------------------------------------
    m_hSingle = pIF->SingleProcessOpen("CSingleProcessWrapper", ID);
    if (!m_hSingle)
        return __LINE__;


    return OK;
}
