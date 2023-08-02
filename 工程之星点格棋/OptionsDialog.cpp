// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "NewBoxes.h"
#include "OptionsDialog.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 

/////////////////////////////////////////////////////////////////////////////
// OptionsDialog dialog


OptionsDialog::OptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(OptionsDialog)
	m_timeLimit = pDoc->TimeLimit();
  m_autoPlay  = pDoc->AutoPlay();
	//}}AFX_DATA_INIT
}


void OptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionsDialog)
	DDX_Text(pDX, IDC_TimeLimit, m_timeLimit);
	DDV_MinMaxUInt(pDX, m_timeLimit, 1, 999999);
	DDX_Check(pDX, IDC_AutoPlay, m_autoPlay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionsDialog, CDialog)
	//{{AFX_MSG_MAP(OptionsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsDialog message handlers

void OptionsDialog::OnOK() 
{
	// TODO: Add extra validation here
  UpdateData(true);
	pDoc->TimeLimit(m_timeLimit);
  pDoc->AutoPlay(m_autoPlay!=0);
	CDialog::OnOK();
}
