#include "stdafx.h"
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"

#include "NewBoxesDoc.h" 

#include "Process.h"

/*
STATUS PROCESS_ENTER_POSITION::OnInitialize(void)
{
  m_title = "Enter a new position - Press 'ESC' when done";
  return STATUS_OK;
}

STATUS PROCESS_ENTER_POSITION::OnShutdown(void)
{
  return STATUS_DONE;
}

STATUS PROCESS_ENTER_POSITION::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	POINT screenOffset;
  POINT dotSpacing;
  POINT gameSize;
  POINT dotSize;
  POINT dotNum;
  MOVE move;
  int x, y;
  move.moveType = MT_EDIT;    
  move.boxesCaptured[0].x = -1;
  move.boxesCaptured[1].x = -1;
  move.dotNum.x = -1;
  screenOffset = pDoc->ScreenOffset();
  dotSpacing = pDoc->DotSpacing();
  gameSize = pDoc->GameSize();
  dotSize = pDoc->DotSize();
  point.x -= screenOffset.x - dotSize.x/2;
  point.y -= screenOffset.y - dotSize.y/2;
  dotNum.x = point.x / dotSpacing.x;
  dotNum.y = point.y / dotSpacing.y;
  if (dotNum.x < 0) return STATUS_OK;
  if (dotNum.y < 0) return STATUS_OK;
  x = point.x % dotSpacing.x;
  y = point.y % dotSpacing.y;
  if (     (y < dotSize.y)
        && (x > dotSize.x)
     )
  { // He clicked on a horizontal line
    if (dotNum.x >= gameSize.x-1) return STATUS_OK;
    if (dotNum.y >= gameSize.y) return STATUS_OK;
    move.oldLineType = pDoc->HorizontalLineType(dotNum);
    pDoc->IncrementHLineType(dotNum);
    move.dotNum = dotNum;
    move.vertical = false;
    move.newLineType = pDoc->HorizontalLineType(dotNum);
    move.FindCaptures(&pDoc->Game());
    move.SetCaptures(&pDoc->Game());
    pDoc->AddMove(move);
    INVALIDATE
  };
  if (     (x < dotSize.x)
        && (y > dotSize.y)
     )
  { // He clicked on a vertical line
    if (dotNum.y >= gameSize.y-1) return STATUS_OK;
    if (dotNum.x >= gameSize.x) return STATUS_OK;
    move.oldLineType = pDoc->VerticalLineType(dotNum);
    pDoc->IncrementVLineType(dotNum);
    move.dotNum = dotNum;
    move.vertical = true;
    move.newLineType = pDoc->VerticalLineType(dotNum);
    move.FindCaptures(&pDoc->Game());
    move.SetCaptures(&pDoc->Game());
    pDoc->AddMove(move);
    INVALIDATE
  };
  if (     (x > dotSize.x)
        && (y > dotSize.y)
     )

  {
    // He clicked on one of the squares.
    if (dotNum.y >= gameSize.y-1) return STATUS_OK;
    if (dotNum.x >= gameSize.x-1) return STATUS_OK;
    move.oldOwnerType[0] = pDoc->BoxOwner(dotNum);
    pDoc->IncrementBoxOwner(dotNum);
    move.newOwnerType[0] = pDoc->BoxOwner(dotNum);
    if (move.newOwnerType[0] != move.oldOwnerType[0])
    {
      move.boxesCaptured[0] = dotNum;
      pDoc->AddMove(move);
      INVALIDATE
    };
  };
  return STATUS_OK;
}


STATUS  PROCESS_ENTER_POSITION::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  if (nChar == 27)
  {
    CON con;
    con = pDoc->Game();
    PopProcess();
#if (defined SW_DUMP)||(defined NS_DUMP)
    CloseDump();
#endif
    return STATUS_DONE;
  };
  return STATUS_OK;
}

bool PROCESS_ENTER_POSITION::IsOK_UnDoTurn(void) const
{
  return pDoc->IsUndoPossible();
}

bool PROCESS_ENTER_POSITION::IsOK_ReDoTurn(void) const
{
  return pDoc->IsRedoPossible();
}

STATUS PROCESS_ENTER_POSITION::OnUnDoTurn(void)
{
  PushProcess(new PROCESS_UNDO_TURN);
  return STATUS_OK;
}

STATUS PROCESS_ENTER_POSITION::OnReDoTurn(void)
{
  PushProcess(new PROCESS_REDO_TURN);
  return STATUS_OK;
}
*/
