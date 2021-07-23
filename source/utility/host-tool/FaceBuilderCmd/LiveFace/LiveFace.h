// LiveFace.h : main header file for the LIVEFACE application
//

#if !defined(AFX_LIVEFACE_H__4F7ED8DD_E3E6_44A6_80FB_D54589687E05__INCLUDED_)
#define AFX_LIVEFACE_H__4F7ED8DD_E3E6_44A6_80FB_D54589687E05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceApp:
// See LiveFace.cpp for the implementation of this class
//

#include "..\LiveFaceCore\config.h"                             //
#include "..\SingleProcessWrapper\SingleProcessWrapper.h"       //

#define CHINA_ACP       936

class CLiveFaceApp : public CWinApp
{
public:
    CLiveFaceApp();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLiveFaceApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation
    //{{AFX_MSG(CLiveFaceApp)
    afx_msg void OnAppAbout();
    afx_msg void OnFileOpen();
    afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFileMruFile1(CCmdUI *pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    CSingleProcessWrapper   m_single;
protected:
    BOOL IsSingle(char *ID);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEFACE_H__4F7ED8DD_E3E6_44A6_80FB_D54589687E05__INCLUDED_)
