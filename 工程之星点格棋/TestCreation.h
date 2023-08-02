#if !defined(AFX_TESTCREATION_H__276D9FA6_76F6_426C_A181_B3960486F18C__INCLUDED_)
#define AFX_TESTCREATION_H__276D9FA6_76F6_426C_A181_B3960486F18C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestCreation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TestCreation dialog

class TestCreation : public CDialog
{
// Construction
public:
	TestCreation(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TestCreation)
	enum { IDD = IDD_TestCreation };
	BOOL	m_obviousCapture;
	BOOL	m_randomMove;
	BOOL	m_giveLittle;
	BOOL	m_giveNothing;
	BOOL	m_grabAnything;
	BOOL	m_simpleWin;
	BOOL	m_verifyNimstringValue;
	int		m_nimstringValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TestCreation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TestCreation)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  void PackDataIntoGame(void);
  void ExtractDataFromGame(void);
  void RemoveOwners(OWNER_TYPE *ro);
  void ReplaceOwners(OWNER_TYPE *ro);


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCREATION_H__276D9FA6_76F6_426C_A181_B3960486F18C__INCLUDED_)
