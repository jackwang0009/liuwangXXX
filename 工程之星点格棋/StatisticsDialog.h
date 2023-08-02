#if !defined(AFX_STATISTICSDIALOG_H__8CFF76D8_1026_455B_A5A6_CCF27F440A38__INCLUDED_)
#define AFX_STATISTICSDIALOG_H__8CFF76D8_1026_455B_A5A6_CCF27F440A38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatisticsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StatisticsDialog dialog

class StatisticsDialog : public CDialog
{
// Construction
public:
	StatisticsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(StatisticsDialog)
	enum { IDD = IDD_Statistics };
	int		m_totalNimHashEntries;
	int		m_nimExistsTrue;
	int		m_nimExistsCalls;
	int		m_nimHashCacheCrash;
	int		m_pointsCacheUpdate;
	int		m_pointsExistsCalls;
	int		m_pointsExistsTrue;
	int		m_pointsHashCacheCrash;
	int		m_totalPointsCacheEntries;
	int		m_cdCacheUpdate;
	int		m_cdExistsCalls;
	int		m_cdExistsTrue;
	int		m_cdHashCachCrash;
	int		m_totalCDCacheEntries;
	UINT	m_nimHashSize;
	UINT	m_cdHashSize;
	UINT	m_pointsHashSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StatisticsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(StatisticsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATISTICSDIALOG_H__8CFF76D8_1026_455B_A5A6_CCF27F440A38__INCLUDED_)
