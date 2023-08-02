#include "stdafx.h"
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"
#include "Process.h"


void MOVE::SetCaptures(DOTS_AND_BOXES *pGame)
{
  if (vertical)
  {
    pGame->SetVLineType(dotNum, newLineType);
  }
  else
  {
    pGame->SetHLineType(dotNum, newLineType);
  };
  if (boxesCaptured[0].x >= 0) pGame->SetBoxOwner(boxesCaptured[0], newOwnerType[0]);
  if (boxesCaptured[1].x >= 0) pGame->SetBoxOwner(boxesCaptured[1], newOwnerType[1]);
}

void MOVE::FindCaptures(DOTS_AND_BOXES *pGame)
{
  int oldNumLine, newNumLine;
  boxesCaptured[0].y = boxesCaptured[0].x = -1;
  boxesCaptured[1].y = boxesCaptured[1].x = -1;
  if (vertical)
  {
    VECTOR dotLeft;
    if (dotNum.x > 0) //check box left
    {
      dotLeft = dotNum + VECTOR(-1,0);
      oldOwnerType[0] = pGame->BoxOwner(dotLeft);
      newOwnerType[0] = oldOwnerType[0];
      pGame->SetVLineType(dotNum, oldLineType);
      oldNumLine = pGame->CountLines(dotLeft);
      pGame->SetVLineType(dotNum, newLineType);
      newNumLine = pGame->CountLines(dotLeft);
      pGame->SetVLineType(dotNum, oldLineType);
      if (newNumLine < 4) newOwnerType[0] = OWNER_NONE;
      else if (oldNumLine < 4) newOwnerType[0] = OwnerType(newLineType);
      if (oldOwnerType[0] != newOwnerType[0])
      {
        boxesCaptured[0] = dotLeft;
      };
    };
    if (dotNum.x < pGame->Size().x - 1) //check box right
    {
      oldOwnerType[1] = pGame->BoxOwner(dotNum);
      newOwnerType[1] = oldOwnerType[1];
      pGame->SetVLineType(dotNum, oldLineType);
      oldNumLine = pGame->CountLines(dotNum);
      //if (numLine == 4) oldOwnerType[1] = OwnerType(oldLineType);
      pGame->SetVLineType(dotNum, newLineType);
      newNumLine = pGame->CountLines(dotNum);
      pGame->SetVLineType(dotNum, oldLineType);
      if (newNumLine < 4) newOwnerType[1] = OWNER_NONE;
      else if (oldNumLine < 4) newOwnerType[1] = OwnerType(newLineType);
      if (oldOwnerType[1] != newOwnerType[1])
      {
        boxesCaptured[1] = dotNum;
      };
    };
  }
  else
  {
    VECTOR dotAbove;
    if (dotNum.y > 0) //check box above
    {
      dotAbove = dotNum + VECTOR(0,-1);
      oldOwnerType[0] = pGame->BoxOwner(dotAbove);
      newOwnerType[0] = oldOwnerType[0];
      pGame->SetHLineType(dotNum, oldLineType);
      oldNumLine = pGame->CountLines(dotAbove);
      //if (numLine == 4) oldOwnerType[0] = OwnerType(oldLineType);
      pGame->SetHLineType(dotNum, newLineType);
      newNumLine = pGame->CountLines(dotAbove);
      pGame->SetHLineType(dotNum, oldLineType);
      if (newNumLine < 4) newOwnerType[0] = OWNER_NONE;
      else if (oldNumLine < 4) newOwnerType[0] = OwnerType(newLineType);
      if (oldOwnerType[0] != newOwnerType[0])
      {
        boxesCaptured[0] = dotAbove;
      };
    };
    if (dotNum.y < pGame->Size().y - 1) //check box below
    {
      oldOwnerType[1] = pGame->BoxOwner(dotNum);
      newOwnerType[1] = oldOwnerType[1];
      pGame->SetHLineType(dotNum, oldLineType);
      oldNumLine = pGame->CountLines(dotNum);
      //if (numLine == 4) oldOwnerType[1] = OwnerType(oldLineType);
      pGame->SetHLineType(dotNum, newLineType);
      newNumLine = pGame->CountLines(dotNum);
      pGame->SetHLineType(dotNum, oldLineType);
      if (newNumLine < 4) newOwnerType[1] = OWNER_NONE;
      else if (oldNumLine < 4) newOwnerType[1] = OwnerType(newLineType);
      if (oldOwnerType[1] != newOwnerType[1])
      {
        boxesCaptured[1] = dotNum;
      };
    };
  };
}

void MOVE::ConstructFromLineNumber(int lineNum)
{
  VECTOR size;
  size = pDoc->GameSize();
  this->dotNum.y = lineNum/(2 * size.x - 1);
  dotNum.x = lineNum%(2 * size.x - 1);
  if (dotNum.x >= size.x - 1)
  {
    dotNum.x -= size.x - 1;
    vertical = true;
  }
  else
  {
    vertical = false;
  };
}

CString MOVE::TypeName(void)const
{
  switch (moveType)
  {
  case MT_NONE:                 return   CString("None");
  case MT_RANDOM:               return   CString("Random");
  case MT_OBVIOUSCAPTURE:       return   CString("Obvious Capture");
  case MT_GIVENOTHING:          return   CString("Give Nothing");
  case MT_BREAKLOOPS:           return   CString("Break Loops");
  case MT_LOOPDIVIDE:           return   CString("Break Divide");
  case MT_DIVIDE:               return   CString("Divide and Conquer");
  case MT_GIVELITTLE:           return   CString("Give Little");
  case MT_GRABANYTHING:         return   CString("Grab Anything");
  case MT_SIMPLEWIN:            return   CString("SimpleWin");
  case MT_SIMPLETIE:            return   CString("SimpleTie");
  case MT_EDIT:                 return   CString("Edit");
  case MT_SIMPLEWINCAPTURE:     return   CString("Simple Win Capture");
  case MT_NIMVALCAPTURE:        return   CString("Nimval Capture");
  case MT_NIMVALDECLINE:        return   CString("Nimval Decline");
  case MT_MAXCAPTURE:           return   CString("Maximize Capture");
  case MT_MAXDECLINE:           return   CString("Maximize Decline");
  case MT_SIMPLEWINDECLINE:     return   CString("Simple Win Decline");
  case MT_VERIFYNIMSTRINGVALUE: return   CString("Verify Nimstring Value");
  case MT_NIMVAL:               return   CString("Nimstring value = 0");
  case MT_NONIMVAL:             return   CString("Nimstring value != 0");
  case MT_OPENINGTIE:           return   CString("Opening book tie");
  case MT_OPENINGWIN:           return   CString("Opening book win");
  default:                      return   CString("Illegal");
  };
}


OWNER_TYPE OwnerType(LINE_TYPE lt)
{
  switch(lt)
  {
  default: return OWNER_NONE;
  case LINE_COMPUTER: return OWNER_COMPUTER;
  case LINE_HUMAN: return OWNER_HUMAN;
  };
}


MOVE MOVE_HISTORY::GetUndoMove(void)
{
  MOVE move;
  VECTOR size;
  move = m_moves[--m_currentMoves];
  move.newLineType = m_moves[m_currentMoves].oldLineType;
  move.oldLineType = m_moves[m_currentMoves].newLineType;
  move.oldOwnerType[0] = m_moves[m_currentMoves].newOwnerType[0];
  move.oldOwnerType[1] = m_moves[m_currentMoves].newOwnerType[1];
  move.newOwnerType[0] = m_moves[m_currentMoves].oldOwnerType[0];
  move.newOwnerType[1] = m_moves[m_currentMoves].oldOwnerType[1];
  return move;
}

MOVE MOVE_HISTORY::GetRedoMove(void)
{
  MOVE move;
  VECTOR size;
  if (m_currentMoves >= m_totalMoves)
  {
    die("Getting non-existent ReDo move");
    return move;
  };
  move = this->m_moves[m_currentMoves++];
  return move;
}


STATUS PROCESS_UNDO_TURN::OnInitialize(void)
{
 // m_title = "Reversing the latest move";
  if (!pDoc->IsUndoPossible())
  {
   // MessageBox(NULL,"There are no turns in the UnDo buffer","Warning",MB_OK);
    return STATUS_DONE;
  };
  OnReturn(NULL);
  return STATUS_OK;
}

STATUS PROCESS_UNDO_TURN::OnReturn(PROCESS *pFlashingProcess)
{
  if (pFlashingProcess != NULL) 
  {
    delete pFlashingProcess;
//    if (m_move.vertical) pDoc->VerticalLineType(m_move.dotNum, LINE_NONE);
//    else pDoc->HorizontalLineType(m_move.dotNum, LINE_NONE);
//    if (m_move.boxesCaptured[0].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[0], OWNER_NONE);
//    if (m_move.boxesCaptured[1].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[1], OWNER_NONE);
//    if (pDoc->IsUndoPossible())
//    {
//      m_move = pDoc->GetUndoMove();
//      m_move.Captures(&pDoc->Game());
//      if (m_move.boxesCaptured[0].x < 0)
//      {
//        pDoc->GetRedoMove();
//        return STATUS_DONE;
//      };
//    }
//    else 
//    {
    return STATUS_DONE;
//    };
  }
  else
  {
    if (pDoc->IsUndoPossible())
    {
      m_move = pDoc->GetUndoMove();
    }
    else
    {
      return STATUS_DONE;
    };
  };
  PushProcess(new PROCESS_FLASH(m_move));
  return STATUS_OK;
}

STATUS PROCESS_REDO_TURN::OnInitialize(void)
{
 // m_title = "Restoring the latest move";
  if (!pDoc->IsRedoPossible())
  {
   // MessageBox(NULL,"There are no turns in the ReDo buffer","Warning",MB_OK);
    return STATUS_DONE;
  };
  OnReturn(NULL);
  return STATUS_OK;
}

STATUS PROCESS_REDO_TURN::OnReturn(PROCESS *pFlashingProcess)
{
  if (pFlashingProcess != NULL) 
  {
    delete pFlashingProcess;
//    if (m_move.vertical) pDoc->VerticalLineType(m_move.dotNum, m_move.newLineType);
//    else pDoc->HorizontalLineType(m_move.dotNum, m_move.newLineType);
//    if (m_move.boxesCaptured[0].x >= 0) 
//    {
//      pDoc->BoxOwner(m_move.boxesCaptured[0],m_move.newOwnerType[0]);
//      if (m_move.boxesCaptured[1].x >= 0) 
//      {
//        pDoc->BoxOwner(m_move.boxesCaptured[1],m_move.newOwnerType[1]);
//      };
//    }
//    else
//    {
    return STATUS_DONE;
//    };
  };
  if (pDoc->IsRedoPossible())
  {
    m_move = pDoc->GetRedoMove();
    m_move.FindCaptures(&pDoc->Game());
    m_move.SetCaptures(&pDoc->Game());
    PushProcess(new PROCESS_FLASH(m_move));
    return STATUS_OK;
  };
  return STATUS_DONE;
}