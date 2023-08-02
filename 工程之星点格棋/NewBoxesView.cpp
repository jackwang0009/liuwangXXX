// NewBoxesView.cpp : implementation of the CNewBoxesView class
//

#include "stdafx.h"
#include "NewBoxes.h"

#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"
#include "NewBoxesView.h"

#include "Process.h"
#include "TestCreation.h"
#include "OptionsDialog.h"
#include "StatisticsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString Test(void);
DISPLAY_MODE displayMode = DISPLAY_GAME;

HWND windowHandle;
CNewBoxesView *pViewWindow;
extern bool tOption;
extern CString testFile;
extern CNewBoxesApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesView

IMPLEMENT_DYNCREATE(CNewBoxesView, CView)

BEGIN_MESSAGE_MAP(CNewBoxesView, CView)
	//{{AFX_MSG_MAP(CNewBoxesView)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_EnterPosition, OnEnterPosition)
	ON_WM_CHAR()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_ComputerMove, OnComputerMove)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(IDM_ComputerMove, OnUpdateComputerMove)
	ON_UPDATE_COMMAND_UI(IDM_EnterPosition, OnUpdateEnterPosition)
	ON_UPDATE_COMMAND_UI(IDM_UnDoTurn, OnUpdateUnDoTurn)
	ON_COMMAND(IDM_UnDoTurn, OnUnDoTurn)
	ON_UPDATE_COMMAND_UI(IDM_ReDoTurn, OnUpdateReDoTurn)
	ON_COMMAND(IDM_ReDoTurn, OnReDoTurn)
	ON_COMMAND(IDM_TestCreation, OnTestCreation)
	ON_UPDATE_COMMAND_UI(IDM_TestCreation, OnUpdateTestCreation)
	ON_COMMAND(IDM_Options, OnOptions)
	ON_UPDATE_COMMAND_UI(IDM_Options, OnUpdateOptions)
	ON_COMMAND(IDC_Statistics, OnStatistics)
	ON_COMMAND(IDM_Nimval, OnNimval)
	ON_UPDATE_COMMAND_UI(IDM_Nimval, OnUpdateNimval)
	ON_COMMAND(IDM_PlayGame, OnPlayGame)
	ON_UPDATE_COMMAND_UI(IDM_PlayGame, OnUpdatePlayGame)
	ON_COMMAND(IDM_GameMode, OnGameMode)
	ON_UPDATE_COMMAND_UI(IDM_GameMode, OnUpdateGameMode)
	ON_COMMAND(IDM_LoopWeights, OnLoopWeights)
	ON_UPDATE_COMMAND_UI(IDM_LoopWeights, OnUpdateLoopWeights)
	ON_COMMAND(IDM_Divide, OnDivide)
	ON_UPDATE_COMMAND_UI(IDM_Divide, OnUpdateDivide)
	//}}AFX_MSG_MAP
	// Standard printing commands
  ON_COMMAND(WM_THREADCOMPLETION, OnWM_ThreadCompletion)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesView construction/destruction

CNewBoxesView::CNewBoxesView()
{
	// TODO: add construction code here
  pViewWindow = this;
  //pCurrentProcess = new PROCESS_IDLE;
  ProcessManager(STATUS_STARTUP);
}

CNewBoxesView::~CNewBoxesView()
{
//  if (pCurrentProcess != NULL) free(pCurrentProcess);
//  pCurrentProcess = NULL;
  ProcessManager(STATUS_SHUTDOWN);
}

BOOL CNewBoxesView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesView drawing

unsigned long Weight2Color(int weight, POINT minMax)
{
  int shift;
  if (weight==0) return RGB(255,255,255);
  shift = 16*(weight-minMax.x)/(1+minMax.y-minMax.x);
  return RGB(0,0,255) >> shift;
}

void CNewBoxesView::OnDraw(CDC* pDC)
{
  if (tOption) 
  {
    return;
  };
	CNewBoxesDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

  POINT minMax;
  VECTOR screenOffset;
  VECTOR dotXY, neighborXY;
  VECTOR gameSize;
  VECTOR dotSpacing;
  VECTOR dotSize;
  VECTOR score;
  RECT dotDesc;
  VECTOR dotNum;
  VECTOR location;
  int lineType, owner, moveCount=0;
  COLORREF fillColor = pDoc->BackgroundColor();
  CString msg;
  CPen whitePen, blackPen, dotPen, humanPen, computerPen, backgroundPen, *originalPen;
  gameSize = pDoc->GameSize();
  dotSpacing = pDoc->DotSpacing();
  screenOffset = pDoc->ScreenOffset();
  dotSize = pDoc->DotSize();
  dotPen.CreatePen(0,pDoc->DotPenWidth(),pDoc->DotColor());
  blackPen.CreatePen(0, pDoc->LinePenWidth(), RGB(0,0,0));
  whitePen.CreatePen(0, pDoc->LinePenWidth(), RGB(255,255,255));
  backgroundPen.CreatePen(0,pDoc->LinePenWidth(),pDoc->BackgroundColor());
  humanPen.CreatePen(0,pDoc->LinePenWidth(),pDoc->HumanLineColor());
  computerPen.CreatePen(0,pDoc->LinePenWidth(),pDoc->ComputerLineColor());
  originalPen = pDC->SelectObject(&dotPen);
  location = pDoc->StatusTextLocation();
  pDC->SetBkColor(pDoc->BackgroundColor());
  pDC->TextOut(location.x, location.y, CurrentProcess()->GetTitle());
  switch (displayMode)
  {
  case DISPLAY_LOOP:   minMax = pDoc->ComputeLoopWeights(0); break;
  case DISPLAY_DIVIDE: minMax = pDoc->ComputeDivideWeights(); break;
  default: minMax.x=0; minMax.y=1;
  };
  for (dotNum.y=0; dotNum.y<gameSize.y-1; dotNum.y++)
  {
    for (dotNum.x=0; dotNum.x<gameSize.x-1; dotNum.x++)
    {
      owner = pDoc->BoxOwner(dotNum);
      dotXY = screenOffset + dotSpacing * dotNum;
      switch (displayMode)
      {
      case DISPLAY_GAME:
        {
          switch (owner)
          {
          case OWNER_HUMAN:    fillColor = pDoc->HumanBoxColor();    break;
          case OWNER_COMPUTER: fillColor = pDoc->ComputerBoxColor(); break;
          default: fillColor = pDoc->BackgroundColor();
          };
          break;
        };
      default: fillColor = pDoc->BackgroundColor();
      };
      pDC->FillSolidRect(dotXY.x, dotXY.y,
                         dotSpacing.x, dotSpacing.y,
                         fillColor);
    };
  };
  for (dotNum.y=0; dotNum.y<gameSize.y; dotNum.y++)
  {
    for (dotNum.x=0; dotNum.x<gameSize.x; dotNum.x++)
    {
      dotXY = screenOffset + dotSpacing * dotNum;
      if (dotNum.y < gameSize.y-1)
      {
        lineType = pDoc->VerticalLineType(dotNum);
        if (lineType != LINE_NONE) moveCount++;
        switch (displayMode)
        {
        case DISPLAY_DIVIDE:
          {
            CPen specialPen;
            specialPen.CreatePen(
                        0,
                        3*pDoc->LinePenWidth(),
                        Weight2Color(pDoc->VerticalDivideWeight(dotNum), minMax));
            pDC->SelectObject(specialPen);
            pDC->MoveTo(dotXY);
            neighborXY = dotXY;
            neighborXY.y += dotSpacing.y;   // A vertical line down.
            pDC->LineTo(neighborXY);
          };
          break;
        case DISPLAY_LOOP:
          {
            CPen specialPen;
            specialPen.CreatePen(
                        0,
                        3*pDoc->LinePenWidth(),
                        Weight2Color(pDoc->VerticalLoopWeight(dotNum), minMax));
            pDC->SelectObject(specialPen);
            pDC->MoveTo(dotXY);
            neighborXY = dotXY;
            neighborXY.y += dotSpacing.y;   // A vertical line down.
            pDC->LineTo(neighborXY);
          };
          break;
        case DISPLAY_GAME: break;
        default: ImplementationError("Unknown Display Mode");
        };
        switch (displayMode)
        {
        case DISPLAY_GAME:
          switch (lineType)
          {
          case LINE_HUMAN:    pDC->SelectObject(humanPen);    break;
          case LINE_COMPUTER: pDC->SelectObject(computerPen); break;
          case LINE_NONE:     pDC->SelectObject(backgroundPen); break;
          };
          break;
        case DISPLAY_LOOP:
        case DISPLAY_DIVIDE:
          if (lineType == LINE_NONE)
          {
            pDC->SelectObject(whitePen);
          }
          else
          {
            pDC->SelectObject(blackPen);
          };
          break;
        };
        pDC->MoveTo(dotXY);
        neighborXY = dotXY;
        neighborXY.y += dotSpacing.y;   // A vertical line down.
        pDC->LineTo(neighborXY);
      };// if dotNum.y
      if (dotNum.x < gameSize.x-1)
      {
        CPen specialPen;
        lineType = pDoc->HorizontalLineType(dotNum);
        if (lineType != LINE_NONE) moveCount++;
        switch (displayMode)
        {
        case DISPLAY_DIVIDE:
          {
            CPen specialPen;
            specialPen.CreatePen(
                       0,
                       3*pDoc->LinePenWidth(),
                       Weight2Color(pDoc->HorizontalDivideWeight(dotNum), minMax));
            pDC->SelectObject(specialPen);
            pDC->MoveTo(dotXY);
            neighborXY = dotXY; 
            neighborXY.x += dotSpacing.x;
            pDC->LineTo(neighborXY);  // A horizontal line right
          };
          break;
        case DISPLAY_LOOP:
          {
            CPen specialPen;
            specialPen.CreatePen(
                       0,
                       3*pDoc->LinePenWidth(),
                       Weight2Color(pDoc->HorizontalLoopWeight(dotNum), minMax));
            pDC->SelectObject(specialPen);
            pDC->MoveTo(dotXY);
            neighborXY = dotXY; 
            neighborXY.x += dotSpacing.x;
            pDC->LineTo(neighborXY);  // A horizontal line right
          };
          break;
        case DISPLAY_GAME: break;
        default: ImplementationError("Unknown Display Mode");
        }; //switch (displayMode)
        switch (displayMode)
        {
        case DISPLAY_GAME:
          switch (lineType)
          {
          case LINE_HUMAN:    pDC->SelectObject(humanPen);    break;
          case LINE_COMPUTER: pDC->SelectObject(computerPen); break;
          case LINE_NONE:     pDC->SelectObject(backgroundPen); break;
          };
          break;
        case DISPLAY_LOOP:
        case DISPLAY_DIVIDE:
          if (lineType == LINE_NONE)
          {
            pDC->SelectObject(whitePen);
          }
          else
          {
            pDC->SelectObject(blackPen);
          };
          break;
        };
        pDC->MoveTo(dotXY);
        neighborXY = dotXY; 
        neighborXY.x += dotSpacing.x;
        pDC->LineTo(neighborXY);  // A horizontal line right
      };// if dotNum.x
      pDC->SelectObject(&dotPen);
      dotDesc.top = dotXY.y - dotSize.y/2;
      dotDesc.left= dotXY.x - dotSize.x/2;
      dotDesc.bottom = dotDesc.top + dotSize.y;
      dotDesc.right  = dotDesc.left + dotSize.x;
      pDC->Ellipse(&dotDesc);
    };// for dotNum.x
  }; // for dotNum.y
  pDC->SetBkColor(pDoc->BackgroundColor());
  pDC->SetTextColor(RGB(96,123,139));
  score = pDoc->Score();
  location = pDoc->HumanScoreLocation();
  msg.Format("您的得分：%3d",score.x);
  pDC->TextOut(location.x, location.y, msg);
  pDC->SetTextColor(RGB(0,0,0));
  location = pDoc->ComputerScoreLocation();
  msg.Format("电脑得分：%3d",score.y);
  pDC->TextOut(location.x, location.y, msg);
  pDC->SetTextColor(RGB(139,0,0));
  location = pDoc->MoveCountLocation();
  msg.Format("已走步数：%3d", moveCount);
  pDC->TextOut(location.x, location.y, msg);
  pDC->SelectObject(originalPen);
}



/////////////////////////////////////////////////////////////////////////////
// CNewBoxesView printing

BOOL CNewBoxesView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNewBoxesView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CNewBoxesView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesView diagnostics

#ifdef _DEBUG
void CNewBoxesView::AssertValid() const
{
	CView::AssertValid();
}

void CNewBoxesView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CNewBoxesDoc* CNewBoxesView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNewBoxesDoc)));
	return (CNewBoxesDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNewBoxesView message handlers


void InvalidateView(void)
{
  pDoc->UpdateAllViews(NULL);
}


void CNewBoxesView::OnEnterPosition() 
{
	// TODO: Add your command handler code here
  ProcessManager(CurrentProcess()->OnEnterPosition());
}

void CNewBoxesView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
  ProcessManager(CurrentProcess()->OnChar(nChar, nRepCnt, nFlags));
  //ProcessMaintenance(PM_MessageReturn);
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CNewBoxesView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
  ProcessManager(CurrentProcess()->OnLButtonDown(nFlags, point));
//  ProcessMaintenance(PM_MessageReturn);
	CView::OnLButtonDown(nFlags, point);
}


void CNewBoxesView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CView::OnLButtonDblClk(nFlags, point);
}

void CNewBoxesView::OnComputerMove() 
{
	// TODO: Add your command handler code here
  //if (!pCurrentProcess->IsIdle()) return;
  ProcessManager(CurrentProcess()->OnComputerMove());
}

void CNewBoxesView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
  ProcessManager(CurrentProcess()->OnTimer(nIDEvent));
//  ProcessMaintenance(PM_MessageReturn);
	CView::OnTimer(nIDEvent);
}


void CNewBoxesView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
  windowHandle = this->m_hWnd;
	
	// TODO: Add your specialized code here and/or call the base class
  if (tOption)
  {
    OnToption();
    PostMessage(WM_COMMAND,ID_APP_EXIT,0);
//    theApp.CloseAllDocuments(true);//PostThreadMessage(WM_COMMAND, ID_APP_EXIT,0);//ExitInstance();
    return;
  };
  SetTimer(1,50,NULL);	
}

void CNewBoxesView::OnWM_ThreadCompletion(void)
{
  ProcessManager(CurrentProcess()->OnThreadCompletion());
}


void CNewBoxesView::OnUpdateComputerMove(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_ComputerMove());	
	
}

void CNewBoxesView::OnUpdateEnterPosition(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_EnterPosition());	
	
}

void CNewBoxesView::OnUpdatePlayGame(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_PlayGame());	
	
}

void CNewBoxesView::OnUpdateUnDoTurn(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_UnDoTurn());	
	
}

void CNewBoxesView::OnUnDoTurn() 
{
	// TODO: Add your command handler code here
  ProcessManager(CurrentProcess()->OnUnDoTurn());
}

void CNewBoxesView::OnUpdateReDoTurn(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_ReDoTurn());	
	
}

void CNewBoxesView::OnReDoTurn() 
{
	// TODO: Add your command handler code here
  ProcessManager(CurrentProcess()->OnReDoTurn());
	
}

void CNewBoxesView::OnTestCreation() 
{
	// TODO: Add your command handler code here
  TestCreation dlg;
  dlg.DoModal();
}

void CNewBoxesView::OnToption(void)
{
  CFile file;
  CString result;
  if (!file.Open((LPCSTR)testFile, CFile::modeRead))
  {
    die((LPCSTR)(CString ("Cannot open test file = ") + testFile));
  };
  CArchive arIn(&file, CArchive::load);
  pDoc->Serialize(arIn);
  arIn.Close();
  file.Close();
  if (pDoc->GameSize().x == 0)
  {
    die("No valid position found");
  };
  result = Test();
  if (result != "")
  {
    Information((LPCSTR)(result + "\n" + testFile));
  };
  pDoc->SetModifiedFlag(false);
  AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

void CNewBoxesView::OnUpdateTestCreation(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_TestCreation());	
	
}




//void CNewBoxesView::OnAppExit() 
//{
//	// TODO: Add your command handler code here
//	theApp.HideApplication();
//  theApp.CloseAllDocuments(true);
//}

void CNewBoxesView::OnOptions() 
{
	// TODO: Add your command handler code here
	OptionsDialog dlg;
  dlg.DoModal();
}

void CNewBoxesView::OnUpdateOptions(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_Options());	
	
}

void CNewBoxesView::OnStatistics() 
{
	// TODO: Add your command handler code here
	StatisticsDialog dlg;
  dlg.DoModal();
}

void CNewBoxesView::OnNimval() 
{
	// TODO: Add your command handler code here
  ProcessManager(CurrentProcess()->OnComputeNimval());
//	NIMCON nimcon;
//  CON con;
//  CString msg;
//  NIMHASHTABLE *origNimHashTable;
//  int nimval;
//  origNimHashTable = nimHashTable;
//  if (nimHashTable == NULL)
//  {
//    nimHashTable = new NIMHASHTABLE;
//  };
//  con = pDoc->Game();
//  nimval = nimcon.NimVal(&con);
//  if (origNimHashTable == NULL)
//  {
//    delete nimHashTable;
//    nimHashTable = NULL;
//  };
//  msg.Format("Nimval = %d", nimval);
//  MessageBox(msg, "Result", MB_OK);4
}

void CNewBoxesView::OnUpdateNimval(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->Enable(CurrentProcess()->IsOK_ComputeNimval());	
	
}

void CNewBoxesView::OnPlayGame() 
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
  ProcessManager(CurrentProcess()->OnPlayGame());
	
}


void CNewBoxesView::OnGameMode() 
{
	// TODO: Add your command handler code here
#ifdef DISPLAYINTERNALS
	displayMode = DISPLAY_GAME;
#endif
  Invalidate();
}

void CNewBoxesView::OnUpdateGameMode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->SetCheck((displayMode==DISPLAY_GAME) ? 1 : 0);
}

void CNewBoxesView::OnLoopWeights() 
{
	// TODO: Add your command handler code here
#ifdef DISPLAYINTERNALS
	displayMode = DISPLAY_LOOP;
#endif
  Invalidate();
}

void CNewBoxesView::OnUpdateLoopWeights(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->SetCheck((displayMode==DISPLAY_LOOP) ? 1 : 0);
	
}

void CNewBoxesView::OnDivide() 
{
	// TODO: Add your command handler code here
#ifdef DISPLAYINTERNALS
  displayMode = DISPLAY_DIVIDE;	
#endif
  Invalidate();
}

void CNewBoxesView::OnUpdateDivide(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
  pCmdUI->SetCheck((displayMode==DISPLAY_DIVIDE) ? 1 : 0);
	
}
