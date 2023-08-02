#if !defined(AFX_OPTIONSDIALOG_H__05C8F1DF_E006_49C4_AA20_33074108B81F__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__05C8F1DF_E006_49C4_AA20_33074108B81F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OptionsDialog dialog

class OptionsDialog : public CDialog
{
// Construction
public:
	OptionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(OptionsDialog)
	enum { IDD = IDD_OptionsDialog };
	UINT	m_timeLimit;
	BOOL	m_autoPlay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OptionsDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDIALOG_H__05C8F1DF_E006_49C4_AA20_33074108B81F__INCLUDED_)
