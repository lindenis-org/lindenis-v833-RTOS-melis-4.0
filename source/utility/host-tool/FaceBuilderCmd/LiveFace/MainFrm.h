// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__021E8ECF_AE13_4235_B9C1_1942C288943B__INCLUDED_)
#define AFX_MAINFRM_H__021E8ECF_AE13_4235_B9C1_1942C288943B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "PreviewBar.h"


class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

// Attributes
public:

    CPreviewBar     m_PreViewTemplate;      //
    CPreviewBar     m_PreViewUser;          //

    BOOL            m_bShowTemplate;        //
    BOOL            m_bShowUser;            //

    BOOL            m_bChina;               //

    HINSTANCE       m_hinstanceChinese;     //

    HINSTANCE       m_hinstanceEnglish;     //
// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL DestroyWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    void SetPaneText(int index, const char *szText);
    void UpdatePreViewBar(void);
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;

// Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnViewPreviewBarTemp();
    afx_msg void OnUpdateViewPreviewBarTemp(CCmdUI *pCmdUI);
    afx_msg void OnViewPreviewBarUser();
    afx_msg void OnUpdateViewPreviewBarUser(CCmdUI *pCmdUI);
    afx_msg void OnUiChinese();
    afx_msg void OnUpdateUiChinese(CCmdUI *pCmdUI);
    afx_msg void OnUiEnglish();
    afx_msg void OnUpdateUiEnglish(CCmdUI *pCmdUI);
    afx_msg void OnFaceGuide();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__021E8ECF_AE13_4235_B9C1_1942C288943B__INCLUDED_)
