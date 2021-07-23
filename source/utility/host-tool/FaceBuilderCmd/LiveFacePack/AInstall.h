// AInstall.h: interface for the ASuitInstall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASUITINSTALL_H__DE50C4E5_889B_44C8_9F64_344BB72CA2D0__INCLUDED_)
#define AFX_ASUITINSTALL_H__DE50C4E5_889B_44C8_9F64_344BB72CA2D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\include\basetypes.h"

class AInstall
{
public:
    void Install(void);

    AInstall();
    virtual ~AInstall();

protected:
    u32 CopyDriver(void);

    void ExtractFile(const char *filename, int resid, const char *restype);

protected:
    u32 CopyDriverFile(char *destpath, char *filename);
    char m_szModuleDir[MAX_PATH];
};

#endif // !defined(AFX_ASUITINSTALL_H__DE50C4E5_889B_44C8_9F64_344BB72CA2D0__INCLUDED_)
