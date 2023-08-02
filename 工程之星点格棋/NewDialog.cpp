// NewDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NewBoxes.h"
#include "NewDialog.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static ui32 prevWidth = 0xffffffff;
static ui32 prevHeight = 0xffffffff;
static ui32 prevRandomSeed = 0;
static bool prevSeedValid = false;
static ui32 prevRandomize = 0xffffffff;

/////////////////////////////////////////////////////////////////////////////
// NewDialog dialog


NewDialog::NewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NewDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewDialog)
	m_randomSeed = 0;
	m_width = 0;
	m_randomize = 0;
	m_height = 0;
	//}}AFX_DATA_INIT
}


void NewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewDialog)
	//DDX_Control(pDX, IDC_Randomize, m_Randomize);
	//DDX_Text(pDX, IDC_RandomSeed, m_randomSeed);
	//DDV_MinMaxUInt(pDX, m_randomSeed, 0, 4294967295);
	DDX_Text(pDX, IDC_Width, m_width);
	DDV_MinMaxUInt(pDX, m_width, 0, 99);
	//DDX_Radio(pDX, IDC_Randomize, m_randomize);
	DDX_Text(pDX, IDC_Height, m_height);
	DDV_MinMaxUInt(pDX, m_height, 2, 999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NewDialog, CDialog)
	//{{AFX_MSG_MAP(NewDialog)
	//ON_BN_CLICKED(IDC_Randomize, OnRandomize)
	//ON_BN_CLICKED(IDC_SetRandomSeed, OnRandomize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewDialog message handlers

/*void NewDialog::OnRandomize() 
{
	// TODO: Add your control notification handler code here
  UpdateData(true);
  GetDlgItem(IDC_RandomSeed)->EnableWindow(m_randomize==1);
  GetDlgItem(IDC_RandomSeedTitle)->EnableWindow(m_randomize==1);
}*/

BOOL NewDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
  m_randomize = prevRandomize;
  if (m_randomize == 0xffffffff)
  {
    m_bRandomize = pDoc->CurrentRandomize();
    m_randomize = m_bRandomize ? 0 : 1;
  };
  m_width = prevWidth;
  if (m_width == 0xffffffff) m_width = pDoc->CurrentDefaultWidth();
  m_height = prevHeight;
  if (m_height == 0xffffffff) m_height = pDoc->CurrentDefaultHeight();
  if (prevSeedValid)
  {
    m_randomSeed = prevRandomSeed;
  }
  else
  {
    m_randomSeed = pDoc->CurrentRandomSeed();
  };
  UpdateData(false);
  //OnRandomize();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void NewDialog::OnOK() 
{
	// TODO: Add extra validation here
  UpdateData(true);
  prevRandomize = m_randomize;
  prevWidth = m_width;
  prevHeight = m_height;
  m_bRandomize = false;
  if (m_randomize == 0)
  {
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    m_randomSeed = fileTime.dwLowDateTime;
    m_bRandomize = true;
  };
  prevRandomSeed = m_randomSeed;
	CDialog::OnOK();
}
