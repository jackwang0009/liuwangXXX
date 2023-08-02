# Microsoft Developer Studio Project File - Name="NewBoxes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **
 
# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=NewBoxes - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NewBoxes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NewBoxes.mak" CFG="NewBoxes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NewBoxes - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "NewBoxes - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NewBoxes - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FAcs /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "NewBoxes - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "HASHSTATS" /D "DISPLAYINTERNALS" /FAcs /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "NewBoxes - Win32 Release"
# Name "NewBoxes - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\DotBoxLib\Canon.cpp
# End Source File
# Begin Source File

SOURCE=.\CCD.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Combinations.cpp
# End Source File
# Begin Source File

SOURCE=.\ComputerMove.cpp
# End Source File
# Begin Source File

SOURCE=.\DeduceMove.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Divide.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\DotsAndBoxes.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\Flow.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Hash.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\IO.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\NewBoxes.cpp
# End Source File
# Begin Source File

SOURCE=.\NewBoxes.rc
# End Source File
# Begin Source File

SOURCE=.\NewBoxesDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\NewBoxesView.cpp
# End Source File
# Begin Source File

SOURCE=.\NewDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NimSimplify.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Nimstring.cpp
# End Source File
# Begin Source File

SOURCE=.\Openings.cpp
# End Source File
# Begin Source File

SOURCE=.\OpeningTables.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayGame.cpp
# End Source File
# Begin Source File

SOURCE=.\Process.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Simplify.cpp
# End Source File
# Begin Source File

SOURCE=.\StatisticsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Strings.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\SWEvaluate.cpp
# End Source File
# Begin Source File

SOURCE=.\Tables.cpp
# End Source File
# Begin Source File

SOURCE=.\TestCreation.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Translate.cpp
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Utility.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BoxDef.h
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\Combinations.h
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\DotsAndBoxes.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\NewBoxes.h
# End Source File
# Begin Source File

SOURCE=.\NewBoxesDoc.h
# End Source File
# Begin Source File

SOURCE=.\NewBoxesView.h
# End Source File
# Begin Source File

SOURCE=.\NewDialog.h
# End Source File
# Begin Source File

SOURCE=..\DotBoxLib\NIMCON.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\Process.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StatisticsDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TestCreation.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\NewBoxes.ico
# End Source File
# Begin Source File

SOURCE=.\res\NewBoxes.rc2
# End Source File
# Begin Source File

SOURCE=.\res\NewBoxesDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
