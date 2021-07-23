#if !defined(AFX_PREVIEWBAR_H__426DCEB6_0A8E_4609_B433_2675C303E9DA__INCLUDED_)
#define AFX_PREVIEWBAR_H__426DCEB6_0A8E_4609_B433_2675C303E9DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewBar.h : header file
//

#include "LiveFaceDoc.h"                        //

#include "control_bar\scbarcf.h"                //



#define INVALID_RES             0               //
#define TEMPLATE_RES            1               //
#define USER_RES                2               //

#define TITLE_STANDARD          "Standard"      //
#define TITLE_USERRES           "User Res"      //

/////////////////////////////////////////////////////////////////////////////
// CPreviewBar window

class CPreviewBar : public CSizingControlBarCF
{
// Construction
public:
    CPreviewBar(int type);

// Attributes
public:
    CStatic m_wndCanvas;                    //
    int     m_type;                         //
// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPreviewBar)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPreviewBar();

    // Generated message map functions
protected:
    CxImage *GetImage(void);
    CLiveFaceDoc *GetCurrentDoc(void);
    //{{AFX_MSG(CPreviewBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWBAR_H__426DCEB6_0A8E_4609_B433_2675C303E9DA__INCLUDED_)
