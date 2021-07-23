; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "LiveFace.h"
LastPage=0

ClassCount=6
Class1=CLiveFaceApp
Class2=CLiveFaceDoc
Class3=CLiveFaceView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_LIVEFATYPE
Resource2=IDR_MAINFRAME
Class5=CChildFrame
Class6=CAboutDlg
Resource3=IDD_ABOUTBOX

[CLS:CLiveFaceApp]
Type=0
HeaderFile=LiveFace.h
ImplementationFile=LiveFace.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CLiveFaceDoc]
Type=0
HeaderFile=LiveFaceDoc.h
ImplementationFile=LiveFaceDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC

[CLS:CLiveFaceView]
Type=0
HeaderFile=LiveFaceView.h
ImplementationFile=LiveFaceView.cpp
Filter=C
LastObject=ID_UI_CHINESE
BaseClass=CListView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=ID_FACE_GUIDE


[CLS:CChildFrame]
Type=0
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp
Filter=M


[CLS:CAboutDlg]
Type=0
HeaderFile=LiveFace.cpp
ImplementationFile=LiveFace.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_VIEW_TOOLBAR
Command5=ID_VIEW_STATUS_BAR
Command6=ID_VIEW_PREVIEW_BAR_TEMP
Command7=ID_VIEW_PREVIEW_BAR_USER
Command8=ID_FACE_GUIDE
Command9=ID_APP_ABOUT
CommandCount=9

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVE_AS
Command4=ID_VIEW_PREVIEW_BAR_TEMP
Command5=ID_VIEW_PREVIEW_BAR_USER
Command6=ID_FACE_BUILD
Command7=ID_FACE_GUIDE
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_LIVEFATYPE]
Type=1
Class=CLiveFaceView
Command1=ID_FILE_OPEN
Command2=ID_FILE_CLOSE
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_VIEW_TOOLBAR
Command8=ID_VIEW_STATUS_BAR
Command9=ID_VIEW_PREVIEW_BAR_TEMP
Command10=ID_VIEW_PREVIEW_BAR_USER
Command11=ID_FACE_BUILD
Command12=ID_COMMENT_CN
Command13=ID_COMMENT_EN
Command14=ID_FACE_GUIDE
Command15=ID_APP_ABOUT
CommandCount=15

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

