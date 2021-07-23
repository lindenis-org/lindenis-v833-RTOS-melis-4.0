// SingleProcessWrapper.h: interface for the CSingleProcessWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEPROCESSWRAPPER_H__83107572_B815_4354_9867_926548594185__INCLUDED_)
#define AFX_SINGLEPROCESSWRAPPER_H__83107572_B815_4354_9867_926548594185__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../PlugInWrapper/PlugInWrapper.h"

#include "../if/SingleProcess_if.h"

class CSingleProcessWrapper : public CPlugInWrapper
{
public:
    CSingleProcessWrapper(const char *plug_path);
    u32 IsSingle(char *ID);
    CSingleProcessWrapper();
    virtual ~CSingleProcessWrapper();
private:
    HSINGLE          m_hSingle;
};

#endif // !defined(AFX_SINGLEPROCESSWRAPPER_H__83107572_B815_4354_9867_926548594185__INCLUDED_)
