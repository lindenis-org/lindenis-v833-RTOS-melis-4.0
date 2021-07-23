// LiveFacePackDlg.h : header file

#if !defined(AFX_LIVEFACEPACKDLG_H__5E88E518_E32E_464D_8014_AFD0BF0BD0C9__INCLUDED_)
#define AFX_LIVEFACEPACKDLG_H__5E88E518_E32E_464D_8014_AFD0BF0BD0C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CLiveFacePackDlg dialog

#include "AInstall.h"

class CLiveFacePackDlg : public CDialog
{
// Construction
public:
    CLiveFacePackDlg(CWnd *pParent = NULL); // standard constructor
    AInstall        m_Install;

// Dialog Data
    //{{AFX_DATA(CLiveFacePackDlg)
    enum { IDD = IDD_LIVEFACEPACK_DIALOG };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLiveFacePackDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CLiveFacePackDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEFACEPACKDLG_H__5E88E518_E32E_464D_8014_AFD0BF0BD0C9__INCLUDED_)
