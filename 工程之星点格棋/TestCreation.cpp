// TestCreation.cpp : implementation file
//

#include "stdafx.h"
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"
#include "TestCreation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void Write(CArchive& ar, char *line);


/////////////////////////////////////////////////////////////////////////////
// TestCreation dialog


TestCreation::TestCreation(CWnd* pParent /*=NULL*/)
	: CDialog(TestCreation::IDD, pParent)
{
	//{{AFX_DATA_INIT(TestCreation)
	m_obviousCapture = FALSE;
	m_randomMove = FALSE;
	m_giveLittle = FALSE;
	m_giveNothing = FALSE;
	m_grabAnything = FALSE;
	m_simpleWin = FALSE;
	m_verifyNimstringValue = FALSE;
	m_nimstringValue = 0;
	//}}AFX_DATA_INIT
}


void TestCreation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TestCreation)
	DDX_Check(pDX, IDC_ObviousCapture, m_obviousCapture);
	DDX_Check(pDX, IDC_RandomMove, m_randomMove);
	DDX_Check(pDX, IDC_GiveLittle, m_giveLittle);
	DDX_Check(pDX, IDC_GiveNothing, m_giveNothing);
	DDX_Check(pDX, IDC_GrabAnything, m_grabAnything);
	DDX_Check(pDX, IDC_SimpleWin, m_simpleWin);
	DDX_Check(pDX, IDC_VerifyNimstringValue, m_verifyNimstringValue);
	DDX_Text(pDX, IDC_NimstringValue, m_nimstringValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TestCreation, CDialog)
	//{{AFX_MSG_MAP(TestCreation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TestCreation message handlers

void TestCreation::PackDataIntoGame(void)
{
  pDoc->AI_Option(MT_RANDOM, m_randomMove);
  pDoc->AI_Option(MT_OBVIOUSCAPTURE, m_obviousCapture);
  pDoc->AI_Option(MT_GIVENOTHING, m_giveNothing);
  pDoc->AI_Option(MT_GIVELITTLE, m_giveLittle);
  pDoc->AI_Option(MT_GRABANYTHING, m_grabAnything);
  pDoc->AI_Option(MT_SIMPLEWIN, m_simpleWin);
  pDoc->AI_Option(MT_VERIFYNIMSTRINGVALUE, m_verifyNimstringValue);
  pDoc->NimstringValueToVerify(m_nimstringValue);
}

void TestCreation::RemoveOwners(OWNER_TYPE *ro)
{
  int row, col, boxNum;
  boxNum = 0;
  POINT p;
  for (row=0; row<pDoc->GameSize().y-1; row++)
  {
    p.y = row;
    for (col=0; col<pDoc->GameSize().x-1; col++,boxNum++)
    {
      p.x = col;
      ro[boxNum] = pDoc->BoxOwner(p);
      if (    (pDoc->HorizontalLineType(col, row) == LINE_COMPUTER)
           || (pDoc->VerticalLineType(col, row) == LINE_COMPUTER)
           || (pDoc->HorizontalLineType(col, row+1) == LINE_COMPUTER)
           || (pDoc->VerticalLineType(col+1, row) == LINE_COMPUTER))
      {
        pDoc->BoxOwner(p, OWNER_NONE);
      };
    };
  };
}

void TestCreation::ReplaceOwners(OWNER_TYPE *ro)
{
  int row, col, boxNum;
  POINT p;
  boxNum = 0;
  for (row=0; row<pDoc->GameSize().y-1; row++)
  {
    p.y = row;
    for (col=0; col<pDoc->GameSize().x-1; col++,boxNum++)
    {
      p.x = col;
      pDoc->BoxOwner(p, ro[boxNum]);
    };
  };
}


void TestCreation::OnOK() 
{
	// TODO: Add extra validation here
  OWNER_TYPE removedOwners[(MAX_WIDTH-1)*(MAX_HEIGHT-1)];
  CFileDialog fileDlg(false,"txt",NULL,OFN_OVERWRITEPROMPT,"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");
  UpdateData(true);
  if (fileDlg.DoModal() != IDOK) return;
  CFile f(fileDlg.GetPathName(),CFile::modeCreate | CFile::modeWrite);
  CArchive ar(&f,CArchive::store );
  PackDataIntoGame();
  RemoveOwners(removedOwners);
  pDoc->Serialize(ar);
  ReplaceOwners(removedOwners);
  ar.Close();
  f.Close();
  pDoc->SetModifiedFlag(false);
	CDialog::OnOK();
}

void TestCreation::ExtractDataFromGame(void)
{
  m_randomMove = pDoc->AI_Option(MT_RANDOM);
  m_obviousCapture = pDoc->AI_Option(MT_OBVIOUSCAPTURE);
  m_giveNothing = pDoc->AI_Option(MT_GIVENOTHING);
  m_giveLittle = pDoc->AI_Option(MT_GIVELITTLE);
  m_grabAnything = pDoc->AI_Option(MT_GRABANYTHING);
  m_simpleWin = pDoc->AI_Option(MT_SIMPLEWIN);
  m_verifyNimstringValue = pDoc->AI_Option(MT_VERIFYNIMSTRINGVALUE);
  m_nimstringValue = pDoc->NimstringValueToVerify();
}

BOOL TestCreation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  ExtractDataFromGame();
  UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
