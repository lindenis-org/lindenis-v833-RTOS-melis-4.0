// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "LiveFace.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define FACE_GUIDE  "LiveFaceGuide.pdf"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_PREVIEW_BAR_TEMP, OnViewPreviewBarTemp)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIEW_BAR_TEMP, OnUpdateViewPreviewBarTemp)
    ON_COMMAND(ID_VIEW_PREVIEW_BAR_USER, OnViewPreviewBarUser)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PREVIEW_BAR_USER, OnUpdateViewPreviewBarUser)
    ON_COMMAND(ID_UI_CHINESE, OnUiChinese)
    ON_UPDATE_COMMAND_UI(ID_UI_CHINESE, OnUpdateUiChinese)
    ON_COMMAND(ID_UI_ENGLISH, OnUiEnglish)
    ON_UPDATE_COMMAND_UI(ID_UI_ENGLISH, OnUpdateUiEnglish)
    ON_COMMAND(ID_FACE_GUIDE, OnFaceGuide)
    ON_WM_DROPFILES()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};
*/

static UINT indicators[] =
{
//  ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_TEMPLATE_SIZE,
    ID_INDICATOR_USERRES_SIZE,
    ID_INDICATOR_CAPS,
//  ID_INDICATOR_NUM,
//  ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

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

CMainFrame::CMainFrame() : m_PreViewTemplate(TEMPLATE_RES), m_PreViewUser(USER_RES)
{
    m_bShowTemplate = TRUE;
    m_bShowUser     = TRUE;

    m_bChina = (CHINA_ACP == GetACP()) ? TRUE : FALSE;

    m_hinstanceChinese = NULL;
    m_hinstanceEnglish = NULL;
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
CMainFrame::~CMainFrame()
{

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
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
                               | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
            !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    //-------------------------------------------------------------------------
    //根据操作系统语言类型设置工具栏字符,工具链按钮添加说明文字
    //-------------------------------------------------------------------------
    int index = 0;
    m_wndToolBar.SetButtonText(index++, m_bChina ? "打开"   : "Open");
    m_wndToolBar.SetButtonText(index++, m_bChina ? "保存"   : "Save");
    m_wndToolBar.SetButtonText(index++, m_bChina ? "另存为" : "Saveas");

    m_wndToolBar.SetButtonText(index++, m_bChina ? "模板"   : "Template");
    m_wndToolBar.SetButtonText(index++, m_bChina ? "用户"   : "User");
    m_wndToolBar.SetButtonText(index++, m_bChina ? "编译"   : "Compile");
    m_wndToolBar.SetButtonText(index++, m_bChina ? "帮助"   : "Help");
    m_wndToolBar.SetButtonText(index++, m_bChina ? "关于"   : "About");

    //调整工具条大小
    CRect rc(0, 0, 0, 0);
    CSize sizeMax(0, 0);
    CToolBarCtrl &bar = m_wndToolBar.GetToolBarCtrl();
    for (int nIndex = bar.GetButtonCount() - 1; nIndex >= 0; nIndex--)
    {
        bar.GetItemRect(nIndex, rc);
        rc.NormalizeRect();
        sizeMax.cx = __max(rc.Size().cx, sizeMax.cx);
        sizeMax.cy = __max(rc.Size().cy, sizeMax.cy);
    }
    m_wndToolBar.SetSizes(sizeMax, CSize(16, 15));

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    if (!m_wndStatusBar.Create(this) ||
            !m_wndStatusBar.SetIndicators(indicators,
                                          sizeof(indicators) / sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    int StatusPaneWidth[] =
    {
        //50,
        200,
        200,
        50,
        50,
        50,
    };
    for (int i = 0 ; i < 3 /*sizeof(indicators)/sizeof(UINT)*/; i++)
    {
        m_wndStatusBar.SetPaneInfo(i, indicators[i], SBPS_POPOUT, StatusPaneWidth[i]);
    }
    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);


    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    if (!m_PreViewTemplate.Create(_T(TITLE_STANDARD), this, CSize(256, 500),
                                  TRUE , AFX_IDW_CONTROLBAR_FIRST + 33))
    {
        Msg("%s %d", __FILE__, __LINE__);
        return -1;
        // fail to create
    }

    m_PreViewTemplate.SetBarStyle(m_PreViewTemplate.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);


    m_PreViewTemplate.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);


    EnableDocking(CBRS_ALIGN_ANY);

#ifdef _SCB_REPLACE_MINIFRAME
    m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
#endif


    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    if (!m_PreViewUser.Create(_T(TITLE_USERRES), this, CSize(256, 500),
                              TRUE , AFX_IDW_CONTROLBAR_FIRST + 33))
    {
        return -1;
        // fail to create
    }
    m_PreViewUser.SetBarStyle(m_PreViewUser.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    m_PreViewUser.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

    DockControlBar(&m_PreViewTemplate, AFX_IDW_DOCKBAR_LEFT);

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    RecalcLayout();
    CRect rBar;
    m_PreViewTemplate.GetWindowRect(rBar);
    rBar.OffsetRect(0, 1);

    DockControlBar(&m_PreViewUser, AFX_IDW_DOCKBAR_LEFT, rBar);


    m_bShowTemplate = TRUE;
    m_bShowUser     = TRUE;

    //-------------------------------------------------------------------------
    //scott
    //-------------------------------------------------------------------------
    CenterWindow(GetDesktopWindow());

    //Msg("%s %d", __FILE__, __LINE__);

    m_hinstanceChinese = AfxGetResourceHandle();//::AfxGetInstanceHandle();
    m_hinstanceEnglish = ::LoadLibrary("English.dll");

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

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    if (!CMDIFrameWnd::PreCreateWindow(cs))
        return FALSE;

    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    //cs.style |=WS_POPUP;            //使主窗口不可见
    //cs.dwExStyle |=WS_EX_TOOLWINDOW;//不显示任务按钮

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext &dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


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

void CMainFrame::UpdatePreViewBar()
{
    m_PreViewTemplate.SendMessage(WM_SIZE);

    m_PreViewUser.SendMessage(WM_SIZE);
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
void CMainFrame::OnViewPreviewBarTemp()
{

    if (m_bShowTemplate)
    {
        ::ShowWindow(m_PreViewTemplate.m_hWnd, SW_HIDE);
        DockControlBar(&m_PreViewTemplate, AFX_IDW_DOCKBAR_LEFT);
        m_bShowTemplate = FALSE;

        RecalcLayout(TRUE);
    }
    else
    {
        ::ShowWindow(m_PreViewTemplate.m_hWnd, SW_SHOW);
        DockControlBar(&m_PreViewTemplate, AFX_IDW_DOCKBAR_LEFT);
        RecalcLayout(TRUE);
        m_bShowTemplate = TRUE;
    }

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
void CMainFrame::OnUpdateViewPreviewBarTemp(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bShowTemplate);

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
void CMainFrame::OnViewPreviewBarUser()
{
    if (m_bShowUser)
    {
        ::ShowWindow(m_PreViewUser.m_hWnd, SW_HIDE);
        DockControlBar(&m_PreViewUser, AFX_IDW_DOCKBAR_LEFT);
        m_bShowUser = FALSE;
        RecalcLayout(TRUE);
    }
    else
    {
        ::ShowWindow(m_PreViewUser.m_hWnd, SW_SHOW);
        DockControlBar(&m_PreViewUser, AFX_IDW_DOCKBAR_LEFT);
        RecalcLayout(TRUE);
        m_bShowUser = TRUE;
    }

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
void CMainFrame::OnUpdateViewPreviewBarUser(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bShowUser);
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

void CMainFrame::OnUiChinese()
{
    m_bChina = TRUE;

    HINSTANCE hinst = m_hinstanceChinese;
    if (hinst)
    {
        ::AfxSetResourceHandle(hinst);
    }
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
void CMainFrame::OnUpdateUiChinese(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bChina);

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
void CMainFrame::OnUiEnglish()
{
    m_bChina = FALSE;
    Msg("OnUiEnglish");

    if (m_hinstanceEnglish)
    {
        Msg("set hinst");
        ::AfxSetResourceHandle(m_hinstanceEnglish);
    }
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
void CMainFrame::OnUpdateUiEnglish(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(!m_bChina);

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

void CMainFrame::OnFaceGuide()
{
    char szHelpFile[MAX_PATH];
    memset(szHelpFile, 0, MAX_PATH);

    char szDir[MAX_PATH];
    GetModuleDirectory(szDir);

    //----------------------------------------------------------
    //打开帮助文档
    //----------------------------------------------------------
    sprintf(szHelpFile, "%s\\%s",  szDir, FACE_GUIDE);
    ::ShellExecute(NULL, "open", (LPCTSTR)szHelpFile, "", NULL, SW_SHOWDEFAULT);

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

BOOL CMainFrame::DestroyWindow()
{
    ::FreeLibrary(m_hinstanceEnglish);

    m_hinstanceEnglish = NULL;

    return CMDIFrameWnd::DestroyWindow();
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
void CMainFrame::SetPaneText(int index, const char *szText)
{
    m_wndStatusBar.SetPaneText(index, szText);
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

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
    char           szFilePath[MAX_PATH];
    UINT           cFiles = 0;
    UINT           u    = 0;
    CLiveFaceApp *pApp = NULL;
    CDocument     *pDoc = NULL;
    CString csFaceTemplate;

    cFiles = DragQueryFile(hDropInfo, (UINT) - 1, NULL, 0);
    if (1 != cFiles)
    {
        ::AfxMessageBox("一次只能处理一个文件");
        goto OnDropFiles_end;
    }

    memset(szFilePath, 0, sizeof(szFilePath));
    DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
    csFaceTemplate.Format("%s", szFilePath);
    //Msg("OnDropFiles:%s", csFaceTemplate);
    pApp = (CLiveFaceApp *)::AfxGetApp();
    if (NULL == pApp)
    {
        Msg("AfxGetApp failed !");
        goto OnDropFiles_end;
    }

    pDoc = pApp->OpenDocumentFile((LPCTSTR)csFaceTemplate);
    if (NULL == pDoc)
    {
        Msg("打开文档(%s)失败", csFaceTemplate);
        goto OnDropFiles_end;
    }

    /*
    for (u = 0; u < cFiles; u++)
    {
       //  get the next file name and try to open it--if not a valid
       //  file, then skip to the next one (if there is one).
       memset(szFilePath, 0, sizeof(szFilePath));
       DragQueryFile(hDropInfo, u, szFilePath, sizeof(szFilePath));
       m_csFilePath.Empty();
       m_csFilePath.Format("[%d]=%s", u, szFilePath);
       this->UpdateData(FALSE);
       //Sleep(2000);
    }
    */

    CMDIFrameWnd::OnDropFiles(hDropInfo);
    return;

OnDropFiles_end:
    Msg("OnDropFiles_end");
    DragFinish(hDropInfo);

    CMDIFrameWnd::OnDropFiles(hDropInfo);
}
