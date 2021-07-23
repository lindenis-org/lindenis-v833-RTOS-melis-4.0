--------------------------------------------------------------------------------
								待办事项
--------------------------------------------------------------------------------

Msg("有待处理%s %d", __FILE__, __LINE__);

资源模版中的文件需要进行格式的识别 
如 bmp， jpg， png等

png图片转换为bmp图片

帮助文档的完善



--------------------------------------------------------------------------------
								2009-06-12 
--------------------------------------------------------------------------------

2009-06-12 17:40:24 scott
更新状态栏 资源文件的size 
测试通过


2009-06-12 17:01:53 scott
工具栏 字符串信息 中文 英文

2009-06-12 11:15:26 scott
应用程序开始运行则创建face目录
应用程序退出时删除face下

2009-06-12 9:29:44 scott
软件启动删除face目录下的文件 
DeleteFaceTempFile 两次调用


2009-06-12 9:03:23 scott
加载模版：非NULL的资源而文件又不存在时 提示用户相关信息

--------------------------------------------------------------------------------
								2009-06-10
--------------------------------------------------------------------------------


2009-06-10 10:42:34 scott
按键up，down更新显示bar home end


2009-06-10 8:56:18 scott
void CLiveFaceDoc::OnFaceBuild() 
	//--------------------------------------------------------------------------
	// 转换前先保存
	//--------------------------------------------------------------------------
	if (this->IsModified())
	{
		::AfxMessageBox("Please Save First !");
		return;
	}

--------------------------------------------------------------------------------
								2009-06-09
--------------------------------------------------------------------------------


2009-06-09 19:23:14 scott
void CLiveFaceDoc::OnFaceBuild() 
// 转换前先保存

2009-06-09 18:25:01 scott
唯一实例运行


2009-06-09 17:26:49 scott
resaccess获取资源文件存在bug
把代码重新写一遍，就oK

--------------------------------------------------------------------------------
								2009-06-08
--------------------------------------------------------------------------------

2009-06-08 15:52:32 scott
除帮助文档以外基本完成


2009-06-08 15:52:07 scott
设计基本的几个Toolbar图标，测试通过


2009-06-08 15:51:27 scott
UI多国语言根据操作系统语言来确定，中途不进行动态切换


2009-06-08 14:04:09 scott
软件启动时根据操作系统语言类型设置UI的语言资源
AfxSetResourceHandle


2009-06-08 11:43:37 scott
添加帮助文档 功能， 测试通过


2009-06-08 11:03:58 scott
void CLiveFaceDoc::OnFaceBuild() 
	this->BeginWaitCursor();
	ret = m_FaceDocument.BuildFace((char *)(LPCTSTR)csFaceBin, 1);
	this->EndWaitCursor();
打开等待光标

2009-06-08 10:46:53 scott
face目录下的文件，在liveface一开始运行就需要删除
liveface退出前删除face目录下的文件


2009-06-08 10:30:55 scott
选择资源文件之后更新item的显示信息


2009-06-08 9:27:27 scott
添加previewbar的显示隐藏控制的菜单项
测试通过


2009-06-08 9:02:41 scott
单击，双击 -1情况下直接返回，不做处理
处理一个资源条目选择Res文件的事件


2009-06-08 9:02:25 scott
listview Font设置，测试通过

--------------------------------------------------------------------------------
								2009-06-07
--------------------------------------------------------------------------------

2009-06-07 11:28:03 scott
用户选择的资源图片也可以显示出来了

2009-06-07 11:08:51 scott
显示资源模版中的图片，已经成功


--------------------------------------------------------------------------------
								2009-06-05
--------------------------------------------------------------------------------

2009-06-05 17:32:55 scott
getActiveDoc需要检查返回的指针是否为NULL


2009-06-05 17:32:37 scott
添加两个preViewbar


2009-06-05 10:34:00 scott
void CLiveFaceDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsModified());	
}


2009-06-05 10:29:37 scott
void CLiveFaceApp::OnUpdateFileOpen(CCmdUI* pCmdUI) 
限制只打开一个文档


2009-06-05 10:12:44 scott
void CLiveFaceApp::OnFileOpen() 
限制只打开一个文档


2009-06-05 9:48:13 scott
script_if 接口  修改了
SetSysInfo SetResInfo 将数据写入脚本存储
用户修改的结果生效了

--------------------------------------------------------------------------------
								2009-06-04
--------------------------------------------------------------------------------

2009-06-04 17:29:28 scott
//对item进行双击
void CLiveFaceView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 


2009-06-04 17:29:23 scott
// 对item进行单击
void CLiveFaceView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 


2009-06-04 17:06:06 scott
LRESULT  CLiveFaceView::OnLoadResInfo(WPARAM wParam, LPARAM lParam)
消息处理函数，实际从doc中获取数据，然后显示出来


2009-06-04 15:57:04 scott
void CLiveFaceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	Msg("OnUpdate:%s %d", __FILE__, __LINE__);

	::PostMessage(this->GetSafeHwnd(), WM_LOAD_RESINFO, 0, 0);
}




2009-06-04 14:19:50 scott
void CLiveFaceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 

2009-06-04 13:43:18 scott
//scott
	CenterWindow(GetDesktopWindow());
	

2009-06-04 10:11:30 scott
BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
//防止在窗口标题栏上把文档名预置成应用程序名
	cs.style &= ~FWS_PREFIXTITLE;
	

2009-06-04 9:55:10 scott
int CLiveFaceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 

2009-06-04 9:40:20 scott
BOOL CLiveFaceDoc::OnOpenDocument(LPCTSTR lpszPathName) 


2009-06-04 9:39:26 scott
防止MDI程序运行时自动创建空白窗口 
cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;	//scott
测试通过

2009-06-04 9:35:40 scott
建立基于MFC MDI listview的LiveFace主程序


2009-06-04 8:41:05 scott
建立LiveFaceCore目录，将核心功能的类以及文件集中在这里


--------------------------------------------------------------------------------
								2009-06-03
--------------------------------------------------------------------------------


2009-06-03 15:52:18 scott
void FaceDocument::Initial()


2009-06-03 15:51:51 scott
void CLiveFaceDoc::OnFaceBuild() 测试通过


2009-06-03 15:05:06 scott
scriptparser getresinfo 从0开始才好，现在是从1开始的
sunny已经完成


2009-06-03 11:18:33 scott
FaceDocumentData Dump

--------------------------------------------------------------------------------
								2009-06-02
--------------------------------------------------------------------------------


2009-06-02 18:44:19 scott
u32 FaceDocument::StoreFaceDocumet(char *face_document)
处理中


2009-06-02 17:05:00 scott
BOOL CLiveFaceDoc::OnSaveDocument(LPCTSTR lpszPathName) 


2009-06-02 15:13:26 scott
commnet等信息可以在UI上显示出来了


2009-06-02 15:07:14 scott
BOOL CLiveFaceDoc::OnOpenDocument(LPCTSTR lpszPathName) 


2009-06-02 15:07:08 scott
BOOL CLiveFaceDoc::OnNewDocument()


2009-06-02 15:03:09 scott
void CCellTypesGridCtrl::AddItem(char *index, char *comment, char *filepath)


2009-06-02 15:02:50 scott
void CLiveFaceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 



2009-06-02 14:17:17 scott
FaceResource 基本功能实现，测试通过


2009-06-02 14:16:49 scott
FaceDocument::LoadFaceDocumet
添加res资源的提取保存文件的检测功能，测试通过


2009-06-02 11:01:49 scott
class FaceResource   负责处理res资源的提取


2009-06-02 10:41:48 scott
FaceDocument::LoadFaceDocumet 测试通过


2009-06-02 10:10:01 scott
修改LoadFaceDocumet中的bug


2009-06-02 9:45:40 scott
u32 FaceDocument::LoadFaceDocumet(char *face_document, BOOL bTemplate)
编写完成，需要进行测试


2009-06-02 8:58:13 scott
class FaceDocumentBuilder  完成，测试通过
facedocbuilder 创建psp.face文件以及用户的风格方案

face.document 脚本来生成风格文件格式
其实就是img文件格式，不过只是采用一层加密即可，不必太关心破解的问题


--------------------------------------------------------------------------------
								2009-05-27
--------------------------------------------------------------------------------

2009-05-27 17:52:06 scott
几个基本的插件已经完成，初步测试通过，还有一点需要调整的地方

2009-05-27 11:03:15 scott
class FaceBuilder  负责face资源的创建

--------------------------------------------------------------------------------
								2009-05-26
--------------------------------------------------------------------------------

2009-05-26 16:42:08 scott
class FaceScriptData  初步完成，有待测试


2009-05-26 16:41:44 scott
class FaceScript  初步完成，有待测试


2009-05-26 16:41:11 scott
class FaceManager  核心类，管理其他的类


2009-05-26 15:46:00 scott
facescript 解析风格资源脚本

2009-05-26 15:45:30 scott
基于BCG库的SDI工程

2009-05-26 15:45:13 scott
风格编辑器

--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
