; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=StatisticsDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "NewBoxes.h"
LastPage=0

ClassCount=9
Class1=CNewBoxesApp
Class2=CNewBoxesDoc
Class3=CNewBoxesView
Class4=CMainFrame

ResourceCount=6
Resource1=IDR_MAINFRAME
Class5=CAboutDlg
Resource2=IDD_TestCreation
Class6=NewDialog
Resource3=IDD_NewDialog
Class7=TestCreation
Resource4=IDD_OptionsDialog
Class8=OptionsDialog
Resource5=IDD_ABOUTBOX
Class9=StatisticsDialog
Resource6=IDD_Statistics

[CLS:CNewBoxesApp]
Type=0
HeaderFile=NewBoxes.h
ImplementationFile=NewBoxes.cpp
Filter=N
LastObject=ID_APP_ABOUT
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CNewBoxesDoc]
Type=0
HeaderFile=NewBoxesDoc.h
ImplementationFile=NewBoxesDoc.cpp
Filter=N
LastObject=ID_APP_EXIT

[CLS:CNewBoxesView]
Type=0
HeaderFile=NewBoxesView.h
ImplementationFile=NewBoxesView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=IDM_Divide


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=NewBoxes.cpp
ImplementationFile=NewBoxes.cpp
Filter=D
LastObject=CAboutDlg

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
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=IDM_GameMode
Command13=IDM_LoopWeights
Command14=IDM_Divide
Command15=IDM_EnterPosition
Command16=IDM_ComputerMove
Command17=IDM_UnDoTurn
Command18=IDM_ReDoTurn
Command19=IDM_TestCreation
Command20=IDM_Options
Command21=IDC_Statistics
Command22=IDM_Nimval
Command23=IDM_PlayGame
Command24=ID_APP_ABOUT
CommandCount=24

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=IDM_ComputerMove
Command2=IDM_EnterPosition
Command3=ID_FILE_NEW
Command4=ID_FILE_OPEN
Command5=ID_FILE_PRINT
Command6=ID_FILE_SAVE
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_CUT
Command15=IDM_ReDoTurn
Command16=IDM_UnDoTurn
CommandCount=16

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_ABOUT
Command2=IDM_ComputerMove
CommandCount=2

[DLG:IDD_NewDialog]
Type=1
Class=NewDialog
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_Width,edit,1350631552
Control4=IDC_Height,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_RandomSeed,edit,1350631552
Control8=IDC_RandomSeedTitle,static,1342308352
Control9=IDC_Randomize,button,1342308361
Control10=IDC_SetRandomSeed,button,1342177289

[CLS:NewDialog]
Type=0
HeaderFile=NewDialog.h
ImplementationFile=NewDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_APP_ABOUT

[DLG:IDD_TestCreation]
Type=1
Class=TestCreation
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_RandomMove,button,1342242819
Control5=IDC_ObviousCapture,button,1342242819
Control6=IDC_GiveNothing,button,1342242819
Control7=IDC_GiveLittle,button,1342242819
Control8=IDC_GrabAnything,button,1342242819
Control9=IDC_SimpleWin,button,1342242819
Control10=IDC_VerifyNimstringValue,button,1342242819
Control11=IDC_NimstringValue,edit,1350631552

[CLS:TestCreation]
Type=0
HeaderFile=TestCreation.h
ImplementationFile=TestCreation.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_NimstringValue

[DLG:IDD_OptionsDialog]
Type=1
Class=OptionsDialog
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_TimeLimit,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_AutoPlay,button,1342242819

[CLS:OptionsDialog]
Type=0
HeaderFile=OptionsDialog.h
ImplementationFile=OptionsDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_AutoPlay

[DLG:IDD_Statistics]
Type=1
Class=StatisticsDialog
ControlCount=36
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_TotalNimHashEntries,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_NimExistsTrue,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_NimExistsCalls,edit,1350631552
Control9=IDC_NimHashCacheCrash,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_TotalPointsCacheEntries,edit,1350631552
Control12=IDC_PointsExistsCalls,edit,1350631552
Control13=IDC_PointsExistsTrue,edit,1350631552
Control14=IDC_PointsHashCacheCrash,edit,1350631552
Control15=IDC_PointsCacheUpdate,edit,1350631552
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_TotalCDCacheEntries,edit,1350631552
Control22=IDC_CDExistsCalls,edit,1350631552
Control23=IDC_CDExistsTrue,edit,1350631552
Control24=IDC_CDHashCacheCrash,edit,1350631552
Control25=IDC_CDCacheUpdate,edit,1350631552
Control26=IDC_STATIC,static,1342308352
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_NimHashSize,edit,1350631552
Control32=IDC_STATIC,static,1342308352
Control33=IDC_pointsHashSize,edit,1350631552
Control34=IDC_STATIC,static,1342308352
Control35=IDC_cdHashSize,edit,1350631552
Control36=IDC_STATIC,static,1342308352

[CLS:StatisticsDialog]
Type=0
HeaderFile=StatisticsDialog.h
ImplementationFile=StatisticsDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_NimHashSize
VirtualFilter=dWC

