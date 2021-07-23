// LiveFaceView.h : interface of the CLiveFaceView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIVEFACEVIEW_H__74A952FC_68B3_4AEA_B978_063767D8F967__INCLUDED_)
#define AFX_LIVEFACEVIEW_H__74A952FC_68B3_4AEA_B978_063767D8F967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_LOAD_RESINFO     (WM_USER + 100)

class CLiveFaceView : public CListView
{
protected: // create from serialization only
    CLiveFaceView();

    DECLARE_DYNCREATE(CLiveFaceView)

// Attributes
public:
    CLiveFaceDoc   *GetDocument();

    BOOL            m_bChinaComment;    //

    int             m_nItemHit;         //当前选中的item的序号
// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLiveFaceView)
public:
    virtual void OnDraw(CDC *pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual BOOL DestroyWindow();
protected:
    virtual void OnInitialUpdate(); // called first time after construct
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    //}}AFX_VIRTUAL

// Implementation
public:
    u32 SelectUserResFile(int index);                       //

    BOOL SetItemSelect(int nItem, BOOL bSelect);            //

    void UpdateItemState(int index);                        //

    virtual ~CLiveFaceView();                               //

#ifdef _DEBUG
    virtual void AssertValid() const;                       //

    virtual void Dump(CDumpContext &dc) const;              //
#endif

protected:
    CFont           m_Font;                                 //
// Generated message map functions
protected:
    void CreateFont(void);
    //{{AFX_MSG(CLiveFaceView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnLoadResInfo(WPARAM wParam, LPARAM lParam);
    afx_msg void OnClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclk(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnCommentCn();
    afx_msg void OnUpdateCommentCn(CCmdUI *pCmdUI);
    afx_msg void OnCommentEn();
    afx_msg void OnUpdateCommentEn(CCmdUI *pCmdUI);
    afx_msg void OnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LiveFaceView.cpp
inline CLiveFaceDoc *CLiveFaceView::GetDocument()
{
    return (CLiveFaceDoc *)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEFACEVIEW_H__74A952FC_68B3_4AEA_B978_063767D8F967__INCLUDED_)
