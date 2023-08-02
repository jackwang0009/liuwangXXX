#if !defined(AFX_NEWDIALOG_H__A39C55EA_58F4_4C67_A5E4_B9C4E8E531BA__INCLUDED_)
#define AFX_NEWDIALOG_H__A39C55EA_58F4_4C67_A5E4_B9C4E8E531BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once 
#endif // _MSC_VER > 1000
// NewDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NewDialog dialog

class NewDialog : public CDialog
{
// Construction
public:
	NewDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(NewDialog)
	enum { IDD = IDD_NewDialog };
	CButton	m_Randomize;
	UINT	m_randomSeed;
	UINT	m_width;
	int		m_randomize;
	UINT	m_height;
	//}}AFX_DATA
  bool m_bRandomize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewDialog)
	//afx_msg void OnRandomize();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWDIALOG_H__A39C55EA_58F4_4C67_A5E4_B9C4E8E531BA__INCLUDED_)
