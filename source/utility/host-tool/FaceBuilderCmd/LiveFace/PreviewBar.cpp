// PreviewBar.cpp : implementation file
//

#include "stdafx.h"

#include "PreviewBar.h"

#include "MainFrm.h"


#include "memdc.h"

// scott
#include "resource.h"                   //

#include "LiveFace.h"                   //

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewBar

CPreviewBar::CPreviewBar(int type)
{
    m_type = type;
}

CPreviewBar::~CPreviewBar()
{
}


BEGIN_MESSAGE_MAP(CPreviewBar, CSizingControlBarG)
    //{{AFX_MSG_MAP(CPreviewBar)
    ON_WM_CREATE()
    ON_WM_DRAWITEM()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewBar message handlers

int CPreviewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CSizingControlBarG::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO: Add your specialized creation code here
    CRect   rect;
    GetClientRect(&rect);

    switch (m_type)
    {
        case TEMPLATE_RES:
            m_wndCanvas.Create(_T(TITLE_STANDARD), WS_VISIBLE | WS_CHILD | SS_OWNERDRAW , rect, this, IDC_IMAGEPREVIEW);
            break;
        case USER_RES:
            m_wndCanvas.Create(_T(TITLE_USERRES), WS_VISIBLE | WS_CHILD | SS_OWNERDRAW , rect, this, IDC_IMAGEPREVIEW);
            break;
        default:
            break;
    }

    //m_wndCanvas.Create(_T("Image Preview Canvas"), WS_VISIBLE|WS_CHILD|SS_OWNERDRAW , rect, this, IDC_IMAGEPREVIEW);
    return 0;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
void CPreviewBar::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
    {
        CRect rect;
        GetClientRect(rect);
        HBRUSH hBrush =::CreateSolidBrush(RGB(255, 255, 255));
        ::FillRect(lpDrawItemStruct->hDC, rect, hBrush);
        DeleteObject(hBrush);

        CxImage *pImage = GetImage();
        if (NULL != pImage)
        {
            pImage->Draw(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem);
        }

    }

//  CSizingControlBarG::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
void CPreviewBar::OnSize(UINT nType, int cx, int cy)
{
    //cx++;
    CSizingControlBarG::OnSize(nType, cx, cy);

    CRect rc;
    GetClientRect(rc);

    CxImage *pImage = GetImage();
    if (pImage == NULL)
    {
        //Msg("OnSize: pImage==NULL m_type=%d", m_type);
        rc.SetRectEmpty();
        m_wndCanvas.MoveWindow(rc);
    }
    else
    {
#if 0
        const int   nWidth      = rc.Width();
        const int   nHeight     = rc.Height();
        const float fRatio      = (float)nHeight / nWidth;
        const float fImgRatio   = (float)pImage->GetHeight() / pImage->GetWidth();

        int XDest, YDest, nDestWidth, nDestHeight;
        if (fImgRatio > fRatio)
        {
            nDestWidth  = (int)(nHeight / fImgRatio);
            XDest       = (nWidth - nDestWidth) / 2;
            YDest       = 0;
            nDestHeight = nHeight;
        }
        else
        {
            XDest       = 0;
            nDestWidth  = nWidth;
            nDestHeight = (int)(nWidth * fImgRatio);
            YDest       = (nHeight - nDestHeight) / 2;
        }
        CRect rect(XDest, YDest, XDest + nDestWidth, YDest + nDestHeight);

#else
        const int   nWidth      = rc.Width();
        const int   nHeight     = rc.Height();
        const float fRatio      = (float)nHeight / nWidth;
        const float fImgRatio   = (float)pImage->GetHeight() / pImage->GetWidth();

        int XDest, YDest, nDestWidth, nDestHeight;
        if (nWidth > (int)pImage->GetWidth())
        {
            XDest = (nWidth  - pImage->GetWidth())  / 2;
        }
        else
        {
            XDest = 0;
        }
        if (nHeight > (int)pImage->GetHeight())
        {
            YDest = (nHeight - pImage->GetHeight()) / 2;
        }
        else
        {
            YDest = 0;
        }
        nDestWidth = pImage->GetWidth();
        nDestHeight = pImage->GetHeight();
        CRect rect(XDest, YDest, XDest + nDestWidth, YDest + nDestHeight);

#endif //
        m_wndCanvas.MoveWindow(rect);
        m_wndCanvas.Invalidate();
    }


    //---------------------------------------------------------------------
    //更新状态栏 资源文件的size
    //---------------------------------------------------------------------
    {
        CMainFrame *pMainFrame = (CMainFrame *) ::AfxGetMainWnd();
        if (NULL == pMainFrame)
        {
            return;
        }

        CString csSize, csInfo;
        if (m_type == TEMPLATE_RES)
        {
            csInfo = (pMainFrame->m_bChina) ? "模板资源尺寸" : "Template Res Size";
        }
        else
        {
            csInfo = (pMainFrame->m_bChina) ? "用户资源尺寸" : "User Res Size";
        }
        if (NULL != pImage)
        {
            csSize.Format("%s %d*%d",  csInfo, pImage->GetWidth(),  pImage->GetHeight());
        }
        else
        {
            csSize = csInfo;
        }
        int index = (m_type == TEMPLATE_RES) ? 0 : 1 ;
        pMainFrame->SetPaneText(index, csSize);
    }
}

// Not closing window, just hiding
void CPreviewBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    if (OnNcHitTest(point) == HTCLOSE)
    {
        CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
        switch (m_type)
        {
            case TEMPLATE_RES:
                pFrame->SendMessage(WM_COMMAND, ID_VIEW_PREVIEW_BAR_TEMP);
                break;
            case USER_RES:
                pFrame->SendMessage(WM_COMMAND, ID_VIEW_PREVIEW_BAR_USER);
                break;
            default:
                break;
        }


    }
    CSizingControlBarG::OnLButtonUp(nFlags, point);
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------
CLiveFaceDoc *CPreviewBar::GetCurrentDoc()
{
    CLiveFaceApp *pApp = NULL;
    CLiveFaceDoc *pDoc = NULL;

    pApp = (CLiveFaceApp *)::AfxGetApp();
    if (NULL == pApp)
    {
        return NULL;
    }

    POSITION template_pos = pApp->GetFirstDocTemplatePosition();
    while (template_pos)
    {
        CMultiDocTemplate *pDocTemplate = (CMultiDocTemplate *)pApp->GetNextDocTemplate(template_pos);
        if (NULL != pDocTemplate)
        {
            POSITION doc_pos = pDocTemplate->GetFirstDocPosition();
            while (doc_pos)
            {
                pDoc = (CLiveFaceDoc *)pDocTemplate->GetNextDoc(doc_pos);
                if (NULL != pDoc)
                {

                    return pDoc;
                }
            }
        }
    }

    return NULL;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------

CxImage *CPreviewBar::GetImage()
{
    CLiveFaceDoc *pDoc = GetCurrentDoc();
    if (NULL == pDoc)
    {
        return NULL;
    }


    CxImage *pImage = NULL;
    switch (m_type)
    {
        case INVALID_RES:
            return NULL;

        case TEMPLATE_RES:
            return pDoc->m_pSelectImage;

        case USER_RES:
            return pDoc->m_pSelectImageUser;

        default:
            return NULL;
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//
//
// 参数说明
//
//
// 返回值
//
//
// 其他
//    无
//
//------------------------------------------------------------------------------------------------------------