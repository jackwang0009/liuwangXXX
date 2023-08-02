// NewBoxesView.h : interface of the CNewBoxesView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWBOXESVIEW_H__C8589207_784D_45AC_9393_A514B859B619__INCLUDED_)
#define AFX_NEWBOXESVIEW_H__C8589207_784D_45AC_9393_A514B859B619__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class PROCESS;

class CNewBoxesView : public CView
{
protected: // create from serialization only
	CNewBoxesView();
	DECLARE_DYNCREATE(CNewBoxesView)

// Attributes
public:
	CNewBoxesDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewBoxesView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewBoxesView();
  virtual void OnWM_ThreadCompletion();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


  void OnToption(void);

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNewBoxesView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnterPosition();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnComputerMove();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateComputerMove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEnterPosition(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnDoTurn(CCmdUI* pCmdUI);
	afx_msg void OnUnDoTurn();
	afx_msg void OnUpdateReDoTurn(CCmdUI* pCmdUI);
	afx_msg void OnReDoTurn();
	afx_msg void OnTestCreation();
	afx_msg void OnUpdateTestCreation(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnUpdateOptions(CCmdUI* pCmdUI);
	afx_msg void OnStatistics();
	afx_msg void OnNimval();
	afx_msg void OnUpdateNimval(CCmdUI* pCmdUI);
	afx_msg void OnPlayGame();
	afx_msg void OnUpdatePlayGame(CCmdUI* pCmdUI);
	afx_msg void OnGameMode();
	afx_msg void OnUpdateGameMode(CCmdUI* pCmdUI);
	afx_msg void OnLoopWeights();
	afx_msg void OnUpdateLoopWeights(CCmdUI* pCmdUI);
	afx_msg void OnDivide();
	afx_msg void OnUpdateDivide(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NewBoxesView.cpp
inline CNewBoxesDoc* CNewBoxesView::GetDocument()
   { return (CNewBoxesDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////




//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWBOXESVIEW_H__C8589207_784D_45AC_9393_A514B859B619__INCLUDED_)
