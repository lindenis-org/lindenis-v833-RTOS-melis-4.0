// LiveFaceDoc.h : interface of the CLiveFaceDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIVEFACEDOC_H__0343952D_44FF_4156_8173_A69ED133406C__INCLUDED_)
#define AFX_LIVEFACEDOC_H__0343952D_44FF_4156_8173_A69ED133406C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "..\LiveFaceCore\FaceDocument.h"                       //

#include "include\ximage.h"                                     //


class CLiveFaceDoc : public CDocument
{
protected: // create from serialization only
    CLiveFaceDoc();

    DECLARE_DYNCREATE(CLiveFaceDoc)

// Attributes
public:

    FaceDocument    m_FaceDocument;                             //

    CxImage        *m_pSelectImage;                             //

    CxImage        *m_pSelectImageUser;                         //
// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLiveFaceDoc)
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive &ar);
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
    virtual void SetTitle(LPCTSTR lpszTitle);
    //}}AFX_VIRTUAL

// Implementation
public:
    u32 GetResCount(void);                                      //

    u32 SetResInfo(int index, FACE_RES_ITEM *resInfo);          //

    u32 GetResInfo(int index, FACE_RES_ITEM *resInfo);          //

    void DeleteUserFile(int index);                             //

    void SelectItem(int index);                                 //

    virtual ~CLiveFaceDoc();                                    //

#ifdef _DEBUG
    virtual void AssertValid() const;                           //

    virtual void Dump(CDumpContext &dc) const;                  //
#endif

protected:

// Generated message map functions
protected:
    int GetTypeFromFileName(LPCTSTR pstr);                      //
    //{{AFX_MSG(CLiveFaceDoc)
    afx_msg void OnFaceBuild();
    afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEFACEDOC_H__0343952D_44FF_4156_8173_A69ED133406C__INCLUDED_)
