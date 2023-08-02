 // StatisticsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NewBoxes.h"
#include "StatisticsDialog.h"

extern int totalNimHashEntries;
extern int nimExistsCalls;
extern int nimExistsTrue;
extern int nimHashCacheCrash;
extern int pointsCacheUpdate;
extern int pointsExistsCalls;
extern int pointsExistsTrue;
extern int pointsHashCacheCrash;
extern int totalPointsCacheEntries;
extern int cdExistsCalls;
extern int cdExistsTrue;
extern int totalcdCacheEntries;
extern int cdHashCacheCrash;
extern int cdCacheUpdate;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// StatisticsDialog dialog


StatisticsDialog::StatisticsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(StatisticsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(StatisticsDialog)
	m_totalNimHashEntries = totalNimHashEntries;
	m_nimExistsTrue = nimExistsTrue;
	m_nimExistsCalls = nimExistsCalls;
	m_nimHashCacheCrash = nimHashCacheCrash;
	m_pointsCacheUpdate = pointsCacheUpdate;
	m_pointsExistsCalls = pointsExistsCalls;
	m_pointsExistsTrue = pointsExistsTrue;
	m_pointsHashCacheCrash = pointsHashCacheCrash;
	m_totalPointsCacheEntries = totalPointsCacheEntries;
	m_cdCacheUpdate = cdCacheUpdate;
	m_cdExistsCalls = cdExistsCalls;
	m_cdExistsTrue = cdExistsTrue;
	m_cdHashCachCrash = cdHashCacheCrash;
	m_totalCDCacheEntries = totalcdCacheEntries;
	m_nimHashSize = NIMHASHSIZE;
	m_cdHashSize = CDHASHSIZE;
	m_pointsHashSize = SWHASHSIZE;
	//}}AFX_DATA_INIT
}


void StatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StatisticsDialog)
	DDX_Text(pDX, IDC_TotalNimHashEntries, m_totalNimHashEntries);
	DDX_Text(pDX, IDC_NimExistsTrue, m_nimExistsTrue);
	DDX_Text(pDX, IDC_NimExistsCalls, m_nimExistsCalls);
	DDX_Text(pDX, IDC_NimHashCacheCrash, m_nimHashCacheCrash);
	DDX_Text(pDX, IDC_PointsCacheUpdate, m_pointsCacheUpdate);
	DDX_Text(pDX, IDC_PointsExistsCalls, m_pointsExistsCalls);
	DDX_Text(pDX, IDC_PointsExistsTrue, m_pointsExistsTrue);
	DDX_Text(pDX, IDC_PointsHashCacheCrash, m_pointsHashCacheCrash);
	DDX_Text(pDX, IDC_TotalPointsCacheEntries, m_totalPointsCacheEntries);
	DDX_Text(pDX, IDC_CDCacheUpdate, m_cdCacheUpdate);
	DDX_Text(pDX, IDC_CDExistsCalls, m_cdExistsCalls);
	DDX_Text(pDX, IDC_CDExistsTrue, m_cdExistsTrue);
	DDX_Text(pDX, IDC_CDHashCacheCrash, m_cdHashCachCrash);
	DDX_Text(pDX, IDC_TotalCDCacheEntries, m_totalCDCacheEntries);
	DDX_Text(pDX, IDC_NimHashSize, m_nimHashSize);
	DDX_Text(pDX, IDC_cdHashSize, m_cdHashSize);
	DDX_Text(pDX, IDC_pointsHashSize, m_pointsHashSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StatisticsDialog, CDialog)
	//{{AFX_MSG_MAP(StatisticsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// StatisticsDialog message handlers
