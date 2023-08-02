// NewBoxesDoc.h : interface of the CNewBoxesDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWBOXESDOC_H__46A66A58_0CB4_4DDB_9F88_283055A36193__INCLUDED_)
#define AFX_NEWBOXESDOC_H__46A66A58_0CB4_4DDB_9F88_283055A36193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DOTS_AND_BOXES;
enum LINE_TYPE;
enum OWNER_TYPE;

class CNewBoxesDoc : public CDocument
{
protected: // create from serialization only
	CNewBoxesDoc();
	DECLARE_DYNCREATE(CNewBoxesDoc)

// Attributes
  int m_currentDefaultWidth;
  int m_currentDefaultHeight;
  int m_currentDotSpaceX;
  int m_currentDotSpaceY;
  int m_currentScreenOffsetX;
  int m_currentScreenOffsetY;
  int m_humanScoreOffsetX;
  int m_humanScoreOffsetY;
  int m_computerScoreOffsetX;
  int m_computerScoreOffsetY;
  int m_statusTextOffsetX;
  int m_statusTextOffsetY;
  int m_moveCountOffsetX;
  int m_moveCountOffsetY;
  int m_currentDotSizeX;
  int m_currentDotSizeY;
  int m_dotPenWidth;
  int m_linePenWidth;
  int m_currentMoveFlashCount;
  int m_timeLimit;
  bool m_autoPlay;
  ui32 m_aiOptions;
  // Random number generation
  ui32 m_random;
  ui32 m_currentRandomSeed;
  bool m_currentRandomize;
  int  m_nimstringValueToVerify;
  //ui32 m_currentDefaultRandomSeed;
  //bool m_currentDefaultRandomize;

  COLORREF m_backgroundColor;
  COLORREF m_dotColor;
  COLORREF m_flashingLineColor;
  COLORREF m_humanLineColor;
  COLORREF m_computerLineColor;
  COLORREF m_humanBoxColor;
  COLORREF m_computerBoxColor;
  COLORREF m_flashingBoxColor;
  COLORREF m_computerTextColor;
  COLORREF m_humanTextColor;

  MOVE_HISTORY m_moveHistory;

  DOTS_AND_BOXES *m_pGame;
public:

// Operations
public:
  int   TimeLimit(void){return m_timeLimit;};
  bool  AutoPlay(void){return m_autoPlay;};
  void  TimeLimit(int tl){m_timeLimit=tl;};
  void  AutoPlay(bool autoplay){m_autoPlay = autoplay;};
  POINT GameSize(void){POINT p={m_pGame->Width(),m_pGame->Height()};return p;};
  POINT DotSpacing(void){POINT p ={m_currentDotSpaceX,m_currentDotSpaceY};return p;};
  POINT ScreenOffset(void){POINT p={m_currentScreenOffsetX,m_currentScreenOffsetY}; return p;};
  POINT DotSize(void){POINT p = {m_currentDotSizeX,m_currentDotSizeY}; return p;};
  VECTOR HumanScoreLocation(void)
  {
    return VECTOR(m_humanScoreOffsetX, m_humanScoreOffsetY);
  };
  VECTOR ComputerScoreLocation(void)
  {
    return VECTOR(m_computerScoreOffsetX, m_computerScoreOffsetY);
  };
  VECTOR StatusTextLocation(void)
  {
    return VECTOR(m_statusTextOffsetX, m_statusTextOffsetY);
  };
  VECTOR MoveCountLocation(void)
  {
    return VECTOR(m_moveCountOffsetX, m_moveCountOffsetY);
  };
  int MoveFlashCount(void){return m_currentMoveFlashCount;};
  COLORREF BackgroundColor(void){return m_backgroundColor;};
  COLORREF DotColor(void){return m_dotColor;};
  COLORREF HumanLineColor(void){return m_humanLineColor;};
  COLORREF ComputerLineColor(void){return m_computerLineColor;};
  COLORREF HumanBoxColor(void){return m_humanBoxColor;};
  COLORREF ComputerBoxColor(void){return m_computerBoxColor;};
  COLORREF HumanTextColor(void){return m_humanTextColor;};
  COLORREF ComputerTextColor(void){return m_computerTextColor;};
  int DotPenWidth(void){return m_dotPenWidth;};
  int LinePenWidth(void){return m_linePenWidth;};
  int CurrentDefaultWidth(void){return m_currentDefaultWidth;};
  int CurrentDefaultHeight(void){return m_currentDefaultHeight;};
  void CurrentRandomize(bool r){m_currentRandomize = r;};
  bool CurrentRandomize(void){return m_currentRandomize;};
  void CurrentRandomSeed(ui32 r){m_currentRandomSeed = r*1213141517;};
  ui32 CurrentRandomSeed(void){return m_currentRandomSeed;};
  void SetRandom(ui32 r){m_random = r;};
  void InputAnalysisParameter(CString line);
  void InputParameter(CString& line);

  LINE_TYPE HorizontalLineType(const POINT& p)const;
  LINE_TYPE HorizontalLineType(int x, int y)const;
  LINE_TYPE VerticalLineType(const POINT& p)const;
  LINE_TYPE VerticalLineType(int x, int y)const;
  void HorizontalLineType(const POINT& p, LINE_TYPE lt);
  void VerticalLineType(const POINT& p, LINE_TYPE lt);
  OWNER_TYPE BoxOwner(const POINT& p)const;
  void BoxOwner(const POINT& p, OWNER_TYPE ot);
  void IncrementHLineType(const POINT& dotNum);
  void IncrementVLineType(const POINT& dotNum);
  void IncrementBoxOwner(const POINT& dotNum);
  VECTOR Score(void)const; // x=human; y=computer.
  void AI_Option(MOVE_TYPE mt, BOOL value);
  bool AI_Option(MOVE_TYPE mt)const;
  int  NimstringValueToVerify(void){return m_nimstringValueToVerify;};
  void NimstringValueToVerify(int v){m_nimstringValueToVerify=v;};
  bool IsMoveAvailable(void)const;
  int  VerticalLoopWeight(POINT dotNum);
  int  HorizontalLoopWeight(POINT dotNum);
  int  VerticalDivideWeight(POINT dotNum);
  int  HorizontalDivideWeight(POINT dotNum);
  POINT ComputeLoopWeights(int MINLOOPBREAK)
  {
    m_pGame->FindAvoids();
    m_pGame->ComputeBreakUpLoopWeights(MINLOOPBREAK);
    return m_pGame->MinMaxLoopWeight();
  };
  POINT ComputeDivideWeights(void)
  {
    m_pGame->FindAvoids();
    m_pGame->ComputeDivideAndConquerWeights_3();
    return m_pGame->MinMaxDivideWeight();
  };

  // ******************************************************
  // ****************  Move History ***********************
  // ******************************************************
  void EraseMoveHistory(void){m_moveHistory.Erase();};
  void AddMove(const MOVE& move){m_moveHistory.AddMove(move);};
  bool IsUndoPossible(void){return m_moveHistory.IsUndoPossible();};
  bool IsRedoPossible(void){return m_moveHistory.IsRedoPossible();};
  MOVE GetUndoMove(void){return m_moveHistory.GetUndoMove();};
  MOVE GetRedoMove(void){return m_moveHistory.GetRedoMove();};

  ui32 MyRandom(ui32 n)
  {
    m_random = m_random*0xbb40e62d + 11;
    return (m_random>>13)% n;
  }

  void MyRandomize(void)
  {
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    m_random = fileTime.dwLowDateTime;
    m_currentRandomSeed = m_random;
  };


  DOTS_AND_BOXES& Game(void){return *m_pGame;};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewBoxesDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewBoxesDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNewBoxesDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern CNewBoxesDoc *pDoc;
#define MODIFY pDoc->SetModifiedFlag(true);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWBOXESDOC_H__46A66A58_0CB4_4DDB_9F88_283055A36193__INCLUDED_)
