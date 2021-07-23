// LiveFace.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LiveFace.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "LiveFaceDoc.h"
#include "LiveFaceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "..\share\error.cpp"
#include "..\share\file.cpp"

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceApp

BEGIN_MESSAGE_MAP(CLiveFaceApp, CWinApp)
    //{{AFX_MSG_MAP(CLiveFaceApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
    ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateFileMruFile1)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceApp construction

CLiveFaceApp::CLiveFaceApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLiveFaceApp object

CLiveFaceApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceApp initialization

BOOL CLiveFaceApp::InitInstance()
{
    if (!IsSingle("LiveFace"))
    {
        //::Msg("LiveSuit已经运行起来了!");
        return FALSE;
    }

    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();         // Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

    if (CHINA_ACP != GetACP())
    {
        HINSTANCE hinst = ::LoadLibrary("English.dll");
        if (hinst)
        {
            Msg("set hinst");
            ::AfxSetResourceHandle(hinst);
        }
    }

    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CMultiDocTemplate *pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(
        IDR_LIVEFATYPE,
        RUNTIME_CLASS(CLiveFaceDoc),
        RUNTIME_CLASS(CChildFrame), // custom MDI child frame
        RUNTIME_CLASS(CLiveFaceView));
    AddDocTemplate(pDocTemplate);

    // create main MDI Frame window
    CMainFrame *pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    m_pMainWnd = pMainFrame;

    // Enable drag/drop open
    m_pMainWnd->DragAcceptFiles();

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;

    //防止MDI程序运行时自动创建空白窗口
    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;    //scott


    ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // The main window has been initialized, so show and update it.
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CLiveFaceApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceApp message handlers


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
void CLiveFaceApp::OnFileOpen()
{
    CString csFaceTemplate;

    CFileDialog fd(TRUE,
                   "Face Template",
                   NULL,
                   OFN_HIDEREADONLY,
                   "Face File (*.face)|*.face|All Files (*.*)|*.*||", NULL);

    if (fd.DoModal() != IDOK)
    {
        return ;
    }

    csFaceTemplate = fd.GetPathName();

    CDocument *pDoc = OpenDocumentFile((LPCTSTR)csFaceTemplate);
    if (NULL == pDoc)
    {
        Msg("打开文档(%s)失败", csFaceTemplate);
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
void CLiveFaceApp::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
    int DocCount = 0;
    POSITION template_pos = this->GetFirstDocTemplatePosition();
    while (template_pos)
    {
        CMultiDocTemplate *pDocTemplate = (CMultiDocTemplate *)GetNextDocTemplate(template_pos);
        if (NULL != pDocTemplate)
        {
            POSITION doc_pos = pDocTemplate->GetFirstDocPosition();
            while (doc_pos)
            {
                CDocument *pDoc = pDocTemplate->GetNextDoc(doc_pos);
                if (NULL != pDoc)
                {
                    DocCount++;
                }
            }
        }
    }

    //Msg("DocCount=%d", DocCount);

    pCmdUI->Enable((DocCount == 0) ? TRUE : FALSE);
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
void CLiveFaceApp::OnUpdateFileMruFile1(CCmdUI *pCmdUI)
{
    int DocCount = 0;
    POSITION template_pos = this->GetFirstDocTemplatePosition();
    while (template_pos)
    {
        CMultiDocTemplate *pDocTemplate = (CMultiDocTemplate *)GetNextDocTemplate(template_pos);
        if (NULL != pDocTemplate)
        {
            POSITION doc_pos = pDocTemplate->GetFirstDocPosition();
            while (doc_pos)
            {
                CDocument *pDoc = pDocTemplate->GetNextDoc(doc_pos);
                if (NULL != pDoc)
                {
                    DocCount++;
                }
            }
        }
    }

    //Msg("DocCount=%d", DocCount);

    pCmdUI->Enable((DocCount == 0) ? TRUE : FALSE);

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
BOOL CLiveFaceApp::IsSingle(char *ID)
{
    u32 ret = OK;
    SINGLEPROCESS_IF *__if = NULL;
    char plug_path[MAX_PATH];

    char szModuleDir[MAX_PATH];
    memset(szModuleDir , 0, MAX_PATH);
    ret = GetModuleDirectory(szModuleDir);
    if (OK != ret)
        return FALSE;

    sprintf(plug_path, "%s\\Single.dll", szModuleDir);
    ret = m_single.SetPlugInPath(plug_path);
    if (OK != ret)
        return FALSE;

    ret = m_single.LoadPlugIn();
    if (OK != ret)
        return FALSE;

    ret = m_single.IsSingle(ID);
    if (OK != ret)
        return FALSE;


    return TRUE;
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