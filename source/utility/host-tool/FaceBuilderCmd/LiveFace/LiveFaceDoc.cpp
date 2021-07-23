// LiveFaceDoc.cpp : implementation of the CLiveFaceDoc class
//

#include "stdafx.h"
#include "LiveFace.h"

#include "LiveFaceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MainFrm.h"
// scott
#pragma comment(lib, "lib\\cximagecrt.lib")     //内存泄漏
//#pragma comment(lib, "lib\\cximagecrtd.lib")  //内存越界

#include "..\interinc\res_id.h"                 //

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceDoc

IMPLEMENT_DYNCREATE(CLiveFaceDoc, CDocument)

BEGIN_MESSAGE_MAP(CLiveFaceDoc, CDocument)
    //{{AFX_MSG_MAP(CLiveFaceDoc)
    ON_COMMAND(ID_FACE_BUILD, OnFaceBuild)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceDoc construction/destruction


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
CLiveFaceDoc::CLiveFaceDoc()
{
    m_FaceDocument.Initial();

    m_pSelectImage = NULL;

    m_pSelectImageUser = NULL;
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
CLiveFaceDoc::~CLiveFaceDoc()
{
    if (NULL != m_pSelectImage)
    {
        delete m_pSelectImage;
        m_pSelectImage = NULL;
    }

    if (NULL != m_pSelectImageUser)
    {
        delete m_pSelectImageUser;
        m_pSelectImageUser = NULL;
    }

    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    pFrame->UpdatePreViewBar();
}

BOOL CLiveFaceDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLiveFaceDoc serialization

void CLiveFaceDoc::Serialize(CArchive &ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceDoc diagnostics

#ifdef _DEBUG
void CLiveFaceDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CLiveFaceDoc::Dump(CDumpContext &dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceDoc commands

#include "LiveFaceView.h"

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
BOOL CLiveFaceDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    int DocCount = 0;
    CDocTemplate *pDocTemplate = GetDocTemplate();
    POSITION pos = pDocTemplate->GetFirstDocPosition();
    while (pos)
    {
        CDocument *pDoc = pDocTemplate->GetNextDoc(pos);
        if (NULL != pDoc)
        {
            DocCount++;
        }
    }
    if (DocCount > 1)
    {
        return FALSE;
    }

    if (!CDocument::OnOpenDocument(lpszPathName))
        return FALSE;


    u32 ret = OK;
    char face_document[MAX_PATH];
    BOOL bTemplate = TRUE;

    memset(face_document, 0, MAX_PATH);
    strcpy(face_document, lpszPathName);

    ret = m_FaceDocument.LoadFaceDocumet(face_document, bTemplate);
    if (OK != ret)
    {
        Msg("LoadFaceDocumet failed %d ", ret);
        return FALSE;
    }

    //UpdateAllViews(NULL, 0, NULL);

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
BOOL CLiveFaceDoc::OnSaveDocument(LPCTSTR lpszPathName)
{

    BOOL bSave = TRUE;
    //bSave CDocument::OnSaveDocument(lpszPathName);

    Msg("CLiveFaceDoc::OnSaveDocument %s", lpszPathName);
    u32 ret = OK;

    char face_document[MAX_PATH];

    memset(face_document, 0, MAX_PATH);
    strcpy(face_document, lpszPathName);

    ret = m_FaceDocument.StoreFaceDocumet(face_document);
    if (OK != ret)
    {
        Msg("StoreFaceDocumet failed %d ", ret);
        return FALSE;
    }

    SetModifiedFlag(FALSE);

    return bSave;
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
void CLiveFaceDoc::OnFaceBuild()
{
    u32 ret = OK;
    CString str;
    CString csFaceBin;

    //--------------------------------------------------------------------------
    // 转换前先保存
    //--------------------------------------------------------------------------
    if (this->IsModified())
    {
        ::AfxMessageBox("Please Save First !");
        return;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    CFileDialog fd(FALSE,
                   "Face Resource",
                   NULL,
                   OFN_HIDEREADONLY,
                   "Face Resource (*.bin)|*.bin|All Files (*.*)|*.*||", NULL);

    if (fd.DoModal() != IDOK)
    {
        return ;
    }

    csFaceBin = fd.GetPathName();


    this->BeginWaitCursor();

    //--------------------------------------------------------------------------
    //生成资源文件
    //--------------------------------------------------------------------------
    ret = m_FaceDocument.BuildFace((char *)(LPCTSTR)csFaceBin, 1);

    this->EndWaitCursor();


    if (OK == ret)
    {
        str.Format("BuildFace resource OK");
    }
    else
    {
        str.Format("BuildFace resource Failed(%d)", ret);

    }
    ::AfxMessageBox(str);
    return;

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
void CLiveFaceDoc::SetTitle(LPCTSTR lpszTitle)
{
    //Msg("SetTitle:%s", lpszTitle);

    CDocument::SetTitle(NULL);

    //CDocument::SetTitle(lpszTitle);
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
void CLiveFaceDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{

    pCmdUI->Enable(IsModified());

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
void CLiveFaceDoc::SelectItem(int index)
{
    //Msg("CLiveFaceDoc::SelectItem(%d)", index);
    BOOL    bUpdate = FALSE;
    u32     ret = OK;
    u16     ID = 0;
    char szResPath[MAX_PATH];

    FaceScriptData *pFaceScriptData = (FaceScriptData *)m_FaceDocument.GetFaceScriptData();
    ASSERT(pFaceScriptData != NULL);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    int ResCount = (int)pFaceScriptData->GetResCount();
    if (index < 0 || index > ResCount)
    {
        Msg("index overflow");
        return;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ID = (u16)index + RES_ID_BASE;

    memset(szResPath, 0, MAX_PATH);
    sprintf(szResPath, "%s\\face\\%d.bmp", m_FaceDocument.GetModuleDir(), ID);
    if (OK != IsFileExist(szResPath))
    {
        ret = m_FaceDocument.GetFaceResource()->SaveResAsFile(ID, szResPath);
        if (OK != ret)
        {
            Msg("SaveResAsFile: %d faile %d", ID, ret);
            return ;
        }

        //Msg("get res[%d] ok", ID);
    }

    int nImageType = GetTypeFromFileName(szResPath);
    if (nImageType != CXIMAGE_FORMAT_UNKNOWN)
    {
        if (NULL != m_pSelectImage)
        {
            delete m_pSelectImage;  //CxImage 对象存在 484字节的内存泄漏 note
            m_pSelectImage = NULL;
        }
        m_pSelectImage = new CxImage(szResPath, nImageType);
        if (NULL == m_pSelectImage)
        {
            Msg("create m_pSelectImage failed !");
        }
        bUpdate = TRUE;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    FACE_RES_ITEM resInfo;
    ret = pFaceScriptData->GetResInfo(&resInfo, index);
    if (OK == ret)
    {
        strcpy(szResPath, resInfo.File);
        if (OK == IsFileExist(szResPath))
        {
            if (NULL != m_pSelectImageUser)
            {
                delete m_pSelectImageUser;
                m_pSelectImageUser = NULL;
            }
            m_pSelectImageUser = new CxImage(szResPath, nImageType);
            if (NULL == m_pSelectImageUser)
            {
                Msg("create m_pSelectImageUser failed !");
            }
            bUpdate = TRUE;
        }
        else
        {
            if (NULL != m_pSelectImageUser)
            {
                delete m_pSelectImageUser;
                m_pSelectImageUser = NULL;
            }
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    //if (bUpdate)
    {
        CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
        pFrame->UpdatePreViewBar();
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
int CLiveFaceDoc::GetTypeFromFileName(LPCTSTR pstr)
{
    CString fileName(pstr);
    CString ext3 = fileName.Right(3);
    CString ext4 = fileName.Right(4);
#if CXIMAGE_SUPPORT_BMP
    if (ext3.CompareNoCase(_T("bmp")) == 0)
        return CXIMAGE_FORMAT_BMP;
#endif
#if CXIMAGE_SUPPORT_GIF
    if (ext3.CompareNoCase(_T("gif")) == 0)
        return CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_JPG
    if (ext3.CompareNoCase(_T("jpg")) == 0 || ext4.CompareNoCase(_T("jpeg")) == 0)
        return CXIMAGE_FORMAT_JPG;
#endif
#if CXIMAGE_SUPPORT_PNG
    if (ext3.CompareNoCase(_T("png")) == 0)
        return CXIMAGE_FORMAT_PNG;
#endif
#if CXIMAGE_SUPPORT_MNG
    if (ext3.CompareNoCase(_T("mng")) == 0 || ext3.CompareNoCase(_T("jng")) == 0 || ext3.CompareNoCase(_T("png")) == 0)
        return CXIMAGE_FORMAT_MNG;
#endif
#if CXIMAGE_SUPPORT_ICO
    if (ext3.CompareNoCase(_T("ico")) == 0)
        return CXIMAGE_FORMAT_ICO;
#endif
#if CXIMAGE_SUPPORT_TIF
    if (ext3.CompareNoCase(_T("tif")) == 0 || ext4.CompareNoCase(_T("tiff")) == 0)
        return CXIMAGE_FORMAT_TIF;
#endif
#if CXIMAGE_SUPPORT_TGA
    if (ext3.CompareNoCase(_T("tga")) == 0)
        return CXIMAGE_FORMAT_TGA;
#endif
#if CXIMAGE_SUPPORT_PCX
    if (ext3.CompareNoCase(_T("pcx")) == 0)
        return CXIMAGE_FORMAT_PCX;
#endif
#if CXIMAGE_SUPPORT_WBMP
    if (ext4.CompareNoCase(_T("wbmp")) == 0)
        return CXIMAGE_FORMAT_WBMP;
#endif
#if CXIMAGE_SUPPORT_WMF
    if (ext3.CompareNoCase(_T("wmf")) == 0 || ext3.CompareNoCase(_T("emf")) == 0)
        return CXIMAGE_FORMAT_WMF;
#endif
#if CXIMAGE_SUPPORT_J2K
    if (ext3.CompareNoCase(_T("j2k")) == 0 || ext3.CompareNoCase(_T("jp2")) == 0)
        return CXIMAGE_FORMAT_J2K;
#endif
#if CXIMAGE_SUPPORT_JBG
    if (ext3.CompareNoCase(_T("jbg")) == 0)
        return CXIMAGE_FORMAT_JBG;
#endif
#if CXIMAGE_SUPPORT_JP2
    if (ext3.CompareNoCase(_T("j2k")) == 0 || ext3.CompareNoCase(_T("jp2")) == 0)
        return CXIMAGE_FORMAT_JP2;
#endif
#if CXIMAGE_SUPPORT_JPC
    if (ext3.CompareNoCase(_T("j2c")) == 0 || ext3.CompareNoCase(_T("jpc")) == 0)
        return CXIMAGE_FORMAT_JPC;
#endif
#if CXIMAGE_SUPPORT_PGX
    if (ext3.CompareNoCase(_T("pgx")) == 0)
        return CXIMAGE_FORMAT_PGX;
#endif
#if CXIMAGE_SUPPORT_PNM
    if (ext3.CompareNoCase(_T("pnm")) == 0 || ext3.CompareNoCase(_T("pgm")) == 0 || ext3.CompareNoCase(_T("ppm")) == 0)
        return CXIMAGE_FORMAT_PNM;
#endif
#if CXIMAGE_SUPPORT_RAS
    if (ext3.CompareNoCase(_T("ras")) == 0)
        return CXIMAGE_FORMAT_RAS;
#endif
    return CXIMAGE_FORMAT_UNKNOWN;
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
void CLiveFaceDoc::DeleteUserFile(int index)
{
    FaceScriptData *pFaceScriptData = m_FaceDocument.GetFaceScriptData();
    int resCount = (int)pFaceScriptData->GetResCount();
    if (index < 0 || index >= resCount)
    {
        return ;
    }

    FACE_RES_ITEM resInfo;

    pFaceScriptData->GetResInfo(&resInfo, index);
    memset(resInfo.File, 0, MAX_PATH);
    strcpy(resInfo.File, EMPTY_PATH);

    pFaceScriptData->SetResInfo(&resInfo, index);

    if (NULL != m_pSelectImageUser)
    {
        delete m_pSelectImageUser;
        m_pSelectImageUser = NULL;
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
u32 CLiveFaceDoc::GetResInfo(int index, FACE_RES_ITEM *resInfo)
{
    FaceResource    *pFaceResource   = m_FaceDocument.GetFaceResource();
    FaceScriptData *pFaceScriptData = m_FaceDocument.GetFaceScriptData();
    int resCount = (int)pFaceScriptData->GetResCount();
    if (index < 0 || index >= resCount)
    {
        return __LINE__;
    }
    if (NULL == resInfo)
    {
        return __LINE__;
    }
    memset(resInfo, 0, sizeof(FACE_RES_ITEM));
    pFaceScriptData->GetResInfo(resInfo, index);

    return OK;
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
u32 CLiveFaceDoc::SetResInfo(int index, FACE_RES_ITEM *resInfo)
{

    FaceScriptData *pFaceScriptData = m_FaceDocument.GetFaceScriptData();

    int resCount = (int)pFaceScriptData->GetResCount();
    if (index < 0 || index >= resCount)
    {
        return __LINE__;
    }
    if (NULL == resInfo)
    {
        return __LINE__;
    }

    pFaceScriptData->SetResInfo(resInfo, index);

    return OK;
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

u32 CLiveFaceDoc::GetResCount()
{
    FaceScriptData *pFaceScriptData = m_FaceDocument.GetFaceScriptData();

    return pFaceScriptData->GetResCount();
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