// NewBoxes.h : main header file for the NEWBOXES application
//

#if !defined(AFX_NEWBOXES_H__41632E73_5B79_48C3_AD0B_4DDA24C67703__INCLUDED_)
#define AFX_NEWBOXES_H__41632E73_5B79_48C3_AD0B_4DDA24C67703__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif 

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesApp:
// See NewBoxes.cpp for the implementation of this class
//

class CNewBoxesApp : public CWinApp
{
public:
	CNewBoxesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewBoxesApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CNewBoxesApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CBoxesCommandLineInfo: public CCommandLineInfo
{
  void ParseParam( LPCTSTR lpszParam, BOOL bFlag, BOOL bLast );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWBOXES_H__41632E73_5B79_48C3_AD0B_4DDA24C67703__INCLUDED_)
