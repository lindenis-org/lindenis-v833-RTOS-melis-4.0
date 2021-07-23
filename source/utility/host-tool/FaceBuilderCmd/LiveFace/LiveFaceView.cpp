// LiveFaceView.cpp : implementation of the CLiveFaceView class
//

#include "stdafx.h"
#include "LiveFace.h"

#include "LiveFaceDoc.h"
#include "LiveFaceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




typedef struct tag_LIST_COLUMN
{
    const char *ColumnStr;
    int    nFormat;
    int    Width;
} LIST_COLUMN_t;



/////////////////////////////////////////////////////////////////////////////
// CLiveFaceView

IMPLEMENT_DYNCREATE(CLiveFaceView, CListView)

BEGIN_MESSAGE_MAP(CLiveFaceView, CListView)
    //{{AFX_MSG_MAP(CLiveFaceView)
    ON_WM_CREATE()
    ON_MESSAGE(WM_LOAD_RESINFO, OnLoadResInfo)
    ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
    ON_COMMAND(ID_COMMENT_CN, OnCommentCn)
    ON_UPDATE_COMMAND_UI(ID_COMMENT_CN, OnUpdateCommentCn)
    ON_COMMAND(ID_COMMENT_EN, OnCommentEn)
    ON_UPDATE_COMMAND_UI(ID_COMMENT_EN, OnUpdateCommentEn)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceView construction/destruction


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
CLiveFaceView::CLiveFaceView()
{
    m_bChinaComment = (CHINA_ACP == GetACP()) ? TRUE : FALSE;

    m_nItemHit = 0;

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
CLiveFaceView::~CLiveFaceView()
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
BOOL CLiveFaceView::PreCreateWindow(CREATESTRUCT &cs)
{
    //Msg("PreCreateWindow:%s %d", __FILE__, __LINE__);

    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    //scott 报表形式 单选
    //cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_EX_TRACKSELECT | LVS_EX_UNDERLINEHOT | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
    //cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_EX_GRIDLINES | LVS_EX_TRACKSELECT;
    //cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_EX_GRIDLINES ;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT | LVS_SINGLESEL;

    return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceView drawing

void CLiveFaceView::OnDraw(CDC *pDC)
{
    CLiveFaceDoc *pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    // TODO: add draw code for native data here
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
void CLiveFaceView::OnInitialUpdate()
{
    //Msg("OnInitialUpdate:%s %d", __FILE__, __LINE__);

    CListView::OnInitialUpdate();

    CreateFont();

    CListCtrl &list = GetListCtrl();

    //设置ListCtrl属性
    DWORD oldStyle = list.GetExtendedStyle();

    list.SetExtendedStyle(LVS_SHOWSELALWAYS  |
                          LVS_REPORT  |
                          LVS_EX_FULLROWSELECT |
                          LVS_EX_TRACKSELECT |
                          LVS_EX_UNDERLINEHOT |
                          LVS_EX_GRIDLINES);
    list.ModifyStyle(0, LVS_REPORT);

    list.SetFont(&m_Font);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    LIST_COLUMN_t *pColumn = NULL;

    LIST_COLUMN_t  Column_CN[] =
    {
        {_T("#"),       LVCFMT_CENTER,  50},
        {_T("说明"),    LVCFMT_LEFT,    250},
        {_T("文件"),    LVCFMT_LEFT,    300},
        {_T("状态"),    LVCFMT_LEFT,    150},
    };

    LIST_COLUMN_t  Column_EN[] =
    {
        {_T("#"),       LVCFMT_CENTER,  50},
        {_T("Comment"), LVCFMT_LEFT,    250},
        {_T("File"),    LVCFMT_LEFT,    300},
        {_T("State"),   LVCFMT_LEFT,    150},
    };

    pColumn = m_bChinaComment ? Column_CN : Column_EN;

    int Count = sizeof(Column_CN) / sizeof(Column_CN[0]);
    for (int i = 0; i < Count; i++)
    {
        list.InsertColumn(i, pColumn[i].ColumnStr, pColumn[i].nFormat, pColumn[i].Width);
    }


    list.UpdateData();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    CHeaderCtrl *pHeader = (CHeaderCtrl *)list.GetDlgItem(0);
    pHeader->RedrawWindow();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    OnLoadResInfo(0, 0);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    int nItemCount    = list.GetItemCount();
    this->SetScrollRange(SB_VERT, 0, nItemCount, TRUE);

    SetItemSelect(0, TRUE);//设置选择的item

    Msg("ScrollRange %d\n", nItemCount);
}

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceView diagnostics

#ifdef _DEBUG
void CLiveFaceView::AssertValid() const
{
    CListView::AssertValid();
}

void CLiveFaceView::Dump(CDumpContext &dc) const
{
    CListView::Dump(dc);
}

CLiveFaceDoc *CLiveFaceView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLiveFaceDoc)));
    return (CLiveFaceDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLiveFaceView message handlers


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

int CLiveFaceView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    //Msg("OnCreate:%s %d", __FILE__, __LINE__);
    if (CListView::OnCreate(lpCreateStruct) == -1)
        return -1;

    //

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
void CLiveFaceView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    //Msg("OnUpdate:%s %d", __FILE__, __LINE__);

    //::SendMessage(this->GetSafeHwnd(), WM_LOAD_RESINFO, 0, 0);
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
LRESULT  CLiveFaceView::OnLoadResInfo(WPARAM wParam, LPARAM lParam)
{
    u32 ret = OK;
    CString csIndex, csComment, csFilePath, csState;
    CListCtrl &list = GetListCtrl();
    list.DeleteAllItems();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    u32 resCount = GetDocument()->GetResCount();
    //Msg("OnLoadResInfo resCount = %d", resCount);
    for (u32 i = 0; i < resCount; i++)
    {
        FACE_RES_ITEM resInfo;
        ret = GetDocument()->GetResInfo(i, &resInfo);
        if (OK != ret)
        {
            break;
        }

        csIndex.Format("%d",    resInfo.ID);
        csComment.Format("%s",  m_bChinaComment ? resInfo.CommentCN : resInfo.CommentEN);
        csFilePath.Format("%s", resInfo.File);

        //--------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------
        if (0 == csFilePath.CompareNoCase(EMPTY_PATH))
        {
            csState = m_bChinaComment ? "请选择" : "Please Select";
        }
        else
        {
            if (OK != IsFileExist(resInfo.File))
            {
                csState = m_bChinaComment ? "文件找不到" : "File Not exist";
            }
            else
            {
                csState = m_bChinaComment ? "有效" : "OK";
            }
        }

        //--------------------------------------------------------------------------
        //
        //--------------------------------------------------------------------------
        list.InsertItem(i, (LPCTSTR)csIndex);
        list.SetItemText(i, 1, (LPCTSTR)csComment);
        list.SetItemText(i, 2, (LPCTSTR)csFilePath);
        list.SetItemText(i, 3, (LPCTSTR)csState);
    }

    list.UpdateData();

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
// 对item进行单击
void CLiveFaceView::OnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    //HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

    NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
    //Msg("OnClick: %d", pNMListView->iItem);

    if (-1 == pNMListView->iItem)
    {
        *pResult = 0;
        return ;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    CListCtrl &list = GetListCtrl();

    int SelectItem = 0;
    POSITION pos = list.GetFirstSelectedItemPosition();
    while (pos)
    {
        SelectItem = list.GetNextSelectedItem(pos);
    }
    UpdateItemState(SelectItem);
    list.SetSelectionMark(SelectItem);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    GetDocument()->SelectItem(pNMListView->iItem);


    *pResult = 0;
}

//------------------------------------------------------------------------------------------------------------
//
// 函数说明
//     对item进行双击
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

void CLiveFaceView::OnDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
    u32 ret = OK;
    HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

    if (-1 == phdn->iItem)
    {
        *pResult = 0;
        return ;
    }
    ret = SelectUserResFile(phdn->iItem);
    if (OK != ret)
    {
        *pResult = 0;
        return ;
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    UpdateItemState(phdn->iItem);

    GetDocument()->SelectItem(phdn->iItem);

    GetDocument()->SetModifiedFlag(TRUE);

    *pResult = 0;

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
void CLiveFaceView::OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;

    //Msg("OnItemchanged: %d", phdn->iItem);

    //GetDocument()->SelectItem(pNMListView->iItem);

    *pResult = 0;
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
void CLiveFaceView::CreateFont()
{
    m_Font.CreateFont(20,                    //int nHeight,
                      0,                  //int nWidth,
                      0,                  //int nEscapement,
                      0,                  //int nOrientation,
                      FW_NORMAL,          //int nWeight,
                      FALSE,              //BYTE bItalic,
                      FALSE,              //BYTE bUnderline,
                      0,                  //BYTE cStrikeOut,
                      DEFAULT_CHARSET,    //BYTE nCharSet,
                      OUT_DEFAULT_PRECIS, //BYTE nOutPrecision,
                      CLIP_DEFAULT_PRECIS,//BYTE nClipPrecision,
                      DEFAULT_QUALITY,    //BYTE nQuality,
                      DEFAULT_PITCH | FF_SWISS, //BYTE nPitchAndFamily,
                      //DEFAULT_PITCH | FF_DONTCARE,
                      //LPCTSTR lpszFacename
                      //"Arial Unicode MS");
                      //"Arial");
                      //"Courier New");
                      "Comic Sans MS");
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

BOOL CLiveFaceView::DestroyWindow()
{
    m_Font.DeleteObject();

    return CListView::DestroyWindow();
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

void CLiveFaceView::OnCommentCn()
{
    m_bChinaComment = TRUE;

    ::SendMessage(this->GetSafeHwnd(), WM_LOAD_RESINFO, 0, 0);
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
void CLiveFaceView::OnUpdateCommentCn(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_bChinaComment);

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
void CLiveFaceView::OnCommentEn()
{
    m_bChinaComment = FALSE;

    ::SendMessage(this->GetSafeHwnd(), WM_LOAD_RESINFO, 0, 0);
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
void CLiveFaceView::OnUpdateCommentEn(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(!m_bChinaComment);

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
void CLiveFaceView::UpdateItemState(int index)
{
    CString csIndex, csComment, csFilePath, csState;

    CListCtrl &list = GetListCtrl();

    FACE_RES_ITEM resInfo;

    u32 ret = GetDocument()->GetResInfo(index, &resInfo);
    if (OK != ret)
    {
        Msg("Err: %s %d", __FILE__, __LINE__);
        return ;
    }

    csIndex.Format("%d",    resInfo.ID);
    csComment.Format("%s",  m_bChinaComment ? resInfo.CommentCN : resInfo.CommentEN);
    csFilePath.Format("%s", resInfo.File);
    //Msg("UpdateItemState(%d)resInfo.File=%s", index, resInfo.File);
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    if (0 == csFilePath.CompareNoCase(EMPTY_PATH))
    {
        csState = m_bChinaComment ? "请选择" : "Please Select";
    }
    else
    {
        if (OK != IsFileExist(resInfo.File))
        {
            csState = m_bChinaComment ? "文件找不到" : "File Not exist";
        }
        else
        {
            csState = m_bChinaComment ? "有效" : "OK";
        }
    }

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
//  list.InsertItem(index,      (LPCTSTR )csIndex);
    list.SetItemText(index, 1, (LPCTSTR)csComment);
    list.SetItemText(index, 2, (LPCTSTR)csFilePath);
    list.SetItemText(index, 3, (LPCTSTR)csState);

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

void CLiveFaceView::OnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
    LV_KEYDOWN *pLVKeyDow = (LV_KEYDOWN *)pNMHDR;

    int resCount = (int)GetDocument()->GetResCount();

    CListCtrl &list = GetListCtrl();
    int nCountPerPage = list.GetCountPerPage();
    int nTopIndex     = list.GetTopIndex();
    int nItemCount    = list.GetItemCount();

    int ItemCount = list.GetItemCount();
    int SelectItem = 0;
    POSITION pos = list.GetFirstSelectedItemPosition();
    while (pos)
    {
        SelectItem = list.GetNextSelectedItem(pos);
    }

    if (SelectItem == -1)
    {
        *pResult = 0;
        return ;
    }
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    switch (pLVKeyDow->wVKey)
    {
        case VK_UP: //向上一行
            if (SelectItem <= 0)
            {
            }
            else
            {
                SelectItem--;
                UpdateItemState(SelectItem);
                list.SetSelectionMark(SelectItem);
                GetDocument()->SelectItem(SelectItem);
                //Msg("do up SelectItem=%d", SelectItem);
            }
            break;

        case VK_DOWN://向下一行
            if (SelectItem <= 0)
            {
            }
            else if (SelectItem >= (resCount - 1))
            {
            }
            else
            {
                SelectItem++;
                UpdateItemState(SelectItem);
                list.SetSelectionMark(SelectItem);
                GetDocument()->SelectItem(SelectItem);
                //Msg("do down SelectItem=%d", SelectItem);
            }
            break;

        case VK_PRIOR:  //向上翻页
            if (SelectItem < nCountPerPage)
            {
                SelectItem = 0;
            }
            else
            {
                SelectItem -= nCountPerPage;
            }
            UpdateItemState(SelectItem);
            list.SetSelectionMark(SelectItem);
            GetDocument()->SelectItem(SelectItem);
            break;


        case VK_NEXT:   //向下翻页
            if ((nItemCount - 1 - SelectItem)  < nCountPerPage)
            {
                SelectItem = nItemCount - 1;
            }
            else
            {
                SelectItem += nCountPerPage;
            }
            UpdateItemState(SelectItem);
            list.SetSelectionMark(SelectItem);
            GetDocument()->SelectItem(SelectItem);
            break;

        case VK_HOME:   //到开头
            SelectItem = 0;
            UpdateItemState(SelectItem);
            list.SetSelectionMark(SelectItem);
            GetDocument()->SelectItem(SelectItem);
            break;

        case VK_END:    //到末尾
            SelectItem = nItemCount - 1;
            UpdateItemState(SelectItem);
            list.SetSelectionMark(SelectItem);
            GetDocument()->SelectItem(SelectItem);
            break;

        case VK_DELETE://删除文件信息
        {
            GetDocument()->DeleteUserFile(SelectItem);
            UpdateItemState(SelectItem);
            list.SetSelectionMark(SelectItem);
            GetDocument()->SelectItem(SelectItem);

            GetDocument()->SetModifiedFlag(TRUE);
        }
        break;

        case VK_RETURN: //回车
        {
            if (OK != SelectUserResFile(SelectItem))
            {
                break;
            }

            UpdateItemState(SelectItem);
            list.SetSelectionMark(SelectItem);
            GetDocument()->SelectItem(SelectItem);

            GetDocument()->SetModifiedFlag(TRUE);
        }
        break;

        default:
            break;
    }

    list.UpdateData(FALSE);

    *pResult = 0;
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

BOOL CLiveFaceView::SetItemSelect(int nItem, BOOL bSelect)
{
    CListCtrl &list = GetListCtrl();
    int nTopIndex = list.GetTopIndex();
    int nItemCount = list.GetItemCount();
    if (nItem < 0 || nItem >= nItemCount)
    {
        return FALSE;
    }

    //设置选择的item
    UINT mask = LVIS_SELECTED ;
    UINT state = 0;
    state = list.GetItemState(m_nItemHit, mask);
    if (bSelect)
    {
        state |= LVIS_SELECTED | LVIS_FOCUSED;
    }
    else
    {
        state = state ^ LVIS_SELECTED;
    }

    list.SetCheck(m_nItemHit, bSelect);//可以去掉

    return list.SetItemState(m_nItemHit, state, mask);
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

u32 CLiveFaceView::SelectUserResFile(int index)
{
    u32 ret = OK;
    //Msg("SelectUserResFile:index=%d", index);
    CString csFilePath;
    CListCtrl &list = GetListCtrl();
    int ItemCount = list.GetItemCount();
    if (index >= ItemCount)
    {
        Msg("Err:%s %d", __FILE__, __LINE__);
        return __LINE__;
    }

    CFileDialog fd(TRUE,
                   "BMP File",
                   NULL,
                   OFN_HIDEREADONLY,
                   "BMP File (*.bmp)|*.bmp|All Files (*.*)|*.*||", NULL);

    if (fd.DoModal() != IDOK)
    {
        return __LINE__;
    }

    csFilePath = fd.GetPathName();

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    FACE_RES_ITEM resInfo;
    ret = GetDocument()->GetResInfo(index, &resInfo);
    if (OK != ret)
    {
        Msg("Err:%s %d", __FILE__, __LINE__);
        goto Error_End;
    }

    memset(resInfo.File, 0, MAX_PATH);
    strcpy(resInfo.File, (LPCTSTR)csFilePath);

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    ret = GetDocument()->SetResInfo(index, &resInfo);
    if (OK != ret)
    {
        Msg("Err:%s %d", __FILE__, __LINE__);
        goto Error_End;
    }

    //Msg("SelectUserResFile: OK");
    return ret;

    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
Error_End:
    Msg("SelectUserResFile: erro %d", ret);
    return ret;
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