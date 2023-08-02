// NewBoxesDoc.cpp : implementation of the CNewBoxesDoc class
//

#include "stdafx.h"
#include "NewBoxes.h"

#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"
#include "NewDialog.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void Write(CArchive& ar, const char *line);
extern bool tOption;

CNewBoxesDoc *pDoc=NULL;

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesDoc

IMPLEMENT_DYNCREATE(CNewBoxesDoc, CDocument)

BEGIN_MESSAGE_MAP(CNewBoxesDoc, CDocument)
	//{{AFX_MSG_MAP(CNewBoxesDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesDoc construction/destruction

CNewBoxesDoc::CNewBoxesDoc()
{
	// TODO: add one-time construction code here
  m_pGame = NULL;
  m_currentDefaultWidth  = DEFAULT_WIDTH;
  m_currentDefaultHeight = DEFAULT_HEIGHT;
  m_currentDotSpaceX     = DEFAULT_DOT_SPACE_X;
  m_currentDotSpaceY     = DEFAULT_DOT_SPACE_Y;
  m_currentDotSizeX      = DEFAULT_DOT_SIZE_X;
  m_currentDotSizeY      = DEFAULT_DOT_SIZE_Y;
  m_currentScreenOffsetX = DEFAULT_SCREEN_OFFSET_X;
  m_currentScreenOffsetY = DEFAULT_SCREEN_OFFSET_Y;
  m_backgroundColor      = DEFAULT_BACKGROUND_COLOR;
  m_dotColor             = DEFAULT_DOT_COLOR;
  m_humanLineColor       = DEFAULT_HUMAN_LINE_COLOR;
  m_computerLineColor    = DEFAULT_COMPUTER_LINE_COLOR;
  m_humanBoxColor        = DEFAULT_HUMAN_BOX_COLOR;
  m_computerBoxColor     = DEFAULT_COMPUTER_BOX_COLOR;
  m_dotPenWidth          = DEFAULT_DOT_PEN_WIDTH;
  m_linePenWidth         = DEFAULT_LINE_PEN_WIDTH;
  m_currentMoveFlashCount= DEFAULT_MOVE_FLASH_COUNT;
  m_computerTextColor    = DEFAULT_COMPUTER_TEXT_COLOR;
  m_humanTextColor       = DEFAULT_HUMAN_TEXT_COLOR;
  m_humanScoreOffsetX    = DEFAULT_HUMAN_SCORE_X;
  m_humanScoreOffsetY    = DEFAULT_HUMAN_SCORE_Y;
  m_computerScoreOffsetX = DEFAULT_COMPUTER_SCORE_X;
  m_computerScoreOffsetY = DEFAULT_COMPUTER_SCORE_Y;
  m_statusTextOffsetX    = DEFAULT_STATUS_TEXT_X;
  m_statusTextOffsetY    = DEFAULT_STATUS_TEXT_Y;
  m_moveCountOffsetX     = DEFAULT_MOVECOUNT_X;
  m_moveCountOffsetY     = DEFAULT_MOVECOUNT_Y;
  m_currentRandomSeed    = 235;
  m_currentRandomize     = DEFAULT_RANDOMIZE;
  m_aiOptions            =  (1 << MT_RANDOM)
                          | (1 << MT_OBVIOUSCAPTURE)
                          | (1 << MT_GIVENOTHING)
                          | (1 << MT_GIVELITTLE)
                          | (1 << MT_GRABANYTHING)
                          | (1 << MT_SIMPLEWIN)
                          | (1 << MT_VERIFYNIMSTRINGVALUE);
  m_timeLimit            = DEFAULT_TIMELIMIT;
  m_autoPlay             = true;
  if (m_currentRandomize) MyRandomize();
  pDoc = this;
}

CNewBoxesDoc::~CNewBoxesDoc()
{
  if (m_pGame != NULL) delete m_pGame;
  m_pGame = NULL;
  pDoc = NULL;
}

BOOL CNewBoxesDoc::OnNewDocument()
{
SetTitle("哈工程工程之星-2013博弈组");
  NewDialog newDlg;
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
  if (!tOption) newDlg.DoModal();
  if (m_pGame != NULL) delete m_pGame;
  m_pGame = new DOTS_AND_BOXES(newDlg.m_width, newDlg.m_height);
#ifdef HASH_CARRY
  if (nimHashTable != NULL) delete nimHashTable;
  nimHashTable = new NIMHASHTABLE;
  if (swHashTable != NULL) delete swHashTable;
  swHashTable = new SWHASHTABLE;
#endif
  pDoc->CurrentRandomize(newDlg.m_bRandomize);
  pDoc->CurrentRandomSeed(newDlg.m_randomSeed);
  pDoc->SetRandom(newDlg.m_randomSeed);
  pDoc->EraseMoveHistory();
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CNewBoxesDoc serialization

void AnalysisOption(CArchive& ar, CString name, MOVE_TYPE mt)
{
  Write(ar,"=Analysis ");
  Write(ar,LPCSTR (name));
  Write(ar, pDoc->AI_Option(mt) ? " Yes" : " No");
  Write(ar,"\n");
}

bool CheckAnalysisParameter(CString& line, MOVE_TYPE mt)
{
  MOVE temp;
  CString value;
  temp.moveType = mt;
  if (line.Find(temp.TypeName()) == 0)
  {
    if (mt == MT_VERIFYNIMSTRINGVALUE)
    {
      int v=0;
      sscanf((LPCSTR)line + temp.TypeName().GetLength(),"%d",&v);
      pDoc->NimstringValueToVerify(v);
      pDoc->AI_Option(mt, true);
      return true;
    }
    value = line.Mid(temp.TypeName().GetLength());
    if (value.Find(" Yes") == 0) pDoc->AI_Option(mt, true);
    else if (value.Find(" No") == 0)  pDoc->AI_Option(mt, false);
    else MessageBox(NULL, line, "Unknown value", MB_OK);
    return true;
  };
  return false;
}

void CNewBoxesDoc::InputAnalysisParameter(CString line)
{
  if (CheckAnalysisParameter(line, MT_RANDOM)) return;
  if (CheckAnalysisParameter(line, MT_OBVIOUSCAPTURE)) return;
  if (CheckAnalysisParameter(line, MT_GIVENOTHING)) return;
  if (CheckAnalysisParameter(line, MT_GIVELITTLE)) return;
  if (CheckAnalysisParameter(line, MT_GRABANYTHING)) return;
  if (CheckAnalysisParameter(line, MT_SIMPLEWIN)) return;
  if (CheckAnalysisParameter(line, MT_VERIFYNIMSTRINGVALUE)) return;
  if (CheckAnalysisParameter(line, MT_NIMVAL)) return;
  MessageBox(NULL,"Unknown Analysis Parameter",line,MB_OK);
}

void CNewBoxesDoc::InputParameter(CString& line)
{
  if (line.Find("=Analysis ") == 0) InputAnalysisParameter(line.Mid(10));
}

void CNewBoxesDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
    *m_pGame >> ar;
    MOVE temp;
    temp.moveType = MT_RANDOM;
    AnalysisOption(ar, temp.TypeName(), MT_RANDOM);
    temp.moveType = MT_OBVIOUSCAPTURE;
    AnalysisOption(ar, temp.TypeName(), MT_OBVIOUSCAPTURE);
    temp.moveType = MT_GIVENOTHING;
    AnalysisOption(ar, temp.TypeName(), MT_GIVENOTHING);
    temp.moveType = MT_GIVELITTLE;
    AnalysisOption(ar, temp.TypeName(), MT_GIVELITTLE);
    temp.moveType = MT_GRABANYTHING;
    AnalysisOption(ar, temp.TypeName(), MT_GRABANYTHING);
    temp.moveType = MT_SIMPLEWIN;
    AnalysisOption(ar, temp.TypeName(), MT_SIMPLEWIN);
    temp.moveType = MT_VERIFYNIMSTRINGVALUE;
    if (AI_Option(MT_VERIFYNIMSTRINGVALUE))
    {
      CString s;
      Write(ar,"=Analysis ");
      Write(ar, temp.TypeName());
      s.Format(" %d",m_nimstringValueToVerify);
      Write(ar, s);
      Write(ar,"\n");
    };
	}
	else
	{
		// TODO: add loading code here
    arInput arIn(&ar);
    CString line;
    m_aiOptions = 0;
    *m_pGame << arIn;
    while ((line = arIn.ReadLine()).GetLength() != 0)
    {
      if (line[0] == '=') InputParameter(line);
    };
    UpdateAllViews(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesDoc diagnostics

#ifdef _DEBUG
void CNewBoxesDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CNewBoxesDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesDoc commands


LINE_TYPE NextLineType(LINE_TYPE lt)
{
  switch (lt)
  {
  case LINE_NONE: return LINE_HUMAN;
  case LINE_HUMAN: return LINE_COMPUTER;
  };
  return LINE_NONE;
}

OWNER_TYPE NextOwnerType(OWNER_TYPE ot)
{
  switch (ot)
  {
  case OWNER_NONE: return OWNER_HUMAN;
  case OWNER_HUMAN: return OWNER_COMPUTER;
  };
  return OWNER_NONE;
}

LINE_TYPE CNewBoxesDoc::HorizontalLineType(const POINT& p) const
{
  return m_pGame->HLineType(p);
}

LINE_TYPE CNewBoxesDoc::HorizontalLineType(int x, int y) const
{
  return m_pGame->HLineType(x, y);
}

void CNewBoxesDoc::HorizontalLineType(const POINT& p, LINE_TYPE lt)
{
  if (m_pGame->HLineType(p) == lt) return;
  MODIFY
  m_pGame->SetHLineType(p, lt);
}

void CNewBoxesDoc::IncrementHLineType(const POINT& dotNum)
{
  MODIFY
  m_pGame->SetHLineType(dotNum, NextLineType(m_pGame->HLineType(dotNum)));
}

LINE_TYPE CNewBoxesDoc::VerticalLineType(const POINT& dotNum) const
{
  return m_pGame->VLineType(dotNum);
}

LINE_TYPE CNewBoxesDoc::VerticalLineType(int x, int y) const
{
  return m_pGame->VLineType(x, y);
}

void CNewBoxesDoc::VerticalLineType(const POINT& p, LINE_TYPE lt)
{
  if (m_pGame->VLineType(p) == lt) return;
  MODIFY
  m_pGame->SetVLineType(p, lt);
}

void CNewBoxesDoc::IncrementVLineType(const POINT& dotNum)
{
  MODIFY
  m_pGame->SetVLineType(dotNum, NextLineType(m_pGame->VLineType(dotNum)));
}

OWNER_TYPE CNewBoxesDoc::BoxOwner(const POINT& p) const
{
  return m_pGame->BoxOwner(p);
}

void CNewBoxesDoc::BoxOwner(const POINT& p, OWNER_TYPE ot)
{
  if (m_pGame->BoxOwner(p) == ot) return;
  MODIFY
  m_pGame->SetBoxOwner(p, ot);
}

void CNewBoxesDoc::IncrementBoxOwner(const POINT& dotNum)
{
  int numLine;
  MODIFY
  numLine = m_pGame->CountLines(dotNum);
  if (numLine < 4) 
  {
    m_pGame->SetBoxOwner(dotNum, OWNER_NONE);
    return;
  };
  m_pGame->SetBoxOwner(dotNum, NextOwnerType(m_pGame->BoxOwner(dotNum)));
  if (BoxOwner(dotNum) == OWNER_NONE)
  {
    m_pGame->SetBoxOwner(dotNum, NextOwnerType(m_pGame->BoxOwner(dotNum)));
  };
}

int CNewBoxesDoc::VerticalLoopWeight(POINT dotNum)
{
  return m_pGame->VerticalLoopWeight(dotNum);
}

int CNewBoxesDoc::HorizontalLoopWeight(POINT dotNum)
{
  return m_pGame->HorizontalLoopWeight(dotNum);
}

int CNewBoxesDoc::VerticalDivideWeight(POINT dotNum)
{
  return m_pGame->VerticalDivideWeight(dotNum);
}

int CNewBoxesDoc::HorizontalDivideWeight(POINT dotNum)
{
  return m_pGame->HorizontalDivideWeight(dotNum);
}

VECTOR CNewBoxesDoc::Score(void) const
{
  int row, col;
  VECTOR result(0,0);
  for (row=0; row<m_pGame->Height()-1; row++)
  {
    for (col=0; col<m_pGame->Width()-1; col++)
    {
      switch (m_pGame->BoxOwner(col, row))
      {
      case OWNER_COMPUTER: result.y++; break;
      case OWNER_HUMAN:    result.x++; break;
      };
    };
  };
  return result;
}

bool CNewBoxesDoc::AI_Option(MOVE_TYPE mt) const
{
  return (m_aiOptions & (1<<mt)) != 0;
}

void CNewBoxesDoc::AI_Option(MOVE_TYPE mt, BOOL value)
{
  if (value)
  {
    if ((m_aiOptions & (1 << mt)) != 0) return;
    MODIFY
    m_aiOptions |= (1 << mt);
  }
  else
  {
    if ((m_aiOptions & (1 << mt)) == 0) return;
    MODIFY
    m_aiOptions &= ~(1 << mt);
  };
}

bool CNewBoxesDoc::IsMoveAvailable(void) const
{
  VECTOR score;
  score = Score();
  return score.x + score.y < (m_pGame->Width()-1)*(m_pGame->Height()-1);
}

MOVE_HISTORY::MOVE_HISTORY(void)
{
  this->m_currentMoves = 0;
  this->m_totalMoves   = 0;
}

void MOVE_HISTORY::AddMove(const MOVE& move)
{
  VECTOR size;
  MODIFY
  size = pDoc->GameSize();
  m_totalMoves = m_currentMoves;
  if (m_currentMoves >= MAX_MOVE)
  {
    MessageBox(NULL, "Exceeded Move History Limit", "Error", MB_OK);
    m_totalMoves = m_currentMoves = 0;
    return;
  };
  this->m_moves[m_totalMoves++] = move;
  m_currentMoves = m_totalMoves;
}

ui32 MyRandom(ui32 n)
{
  return pDoc->MyRandom(n);
}

