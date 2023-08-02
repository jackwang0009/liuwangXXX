#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"

MOVE FollowChain(DOTS_AND_BOXES *pGame, VECTOR box, MOVE prevLine)
{// Keep following the chain of boxes until we find the
 // last possible line that we can draw in the chain.
  VECTOR size, point, nextBox;
  int numSide;
  MOVE nextLine, origLine;
  origLine.dotNum = prevLine.dotNum;
  origLine.vertical = prevLine.vertical;
  nextLine.oldLineType = prevLine.oldLineType;
  nextLine.newLineType = prevLine.newLineType;
  size = pGame->Size();
  for (;;)
  {
    if (   (box.x < 0)
        || (box.y < 0) 
        || (box.x >= size.x-1)
        || (box.y >= size.y-1)) break; // The previous line is the last
    // See if the box has an other empty sides.  If so, loop
    // back and continue to follow the chain.
    // 
    // This box has four sides.  If it has 
    // two empty sides then we keep going.
    // If it has zero, one, three, or four empty
    // sides then we are at the end of the chain.
    numSide = 0;
    point = box;
    if (pGame->HLineType(point) == LINE_NONE)
    {
      numSide++;
      if (prevLine.vertical || (prevLine.dotNum!=point))
      {
        nextLine.dotNum = point; nextBox = point; nextBox.y--;
        nextLine.vertical = false;
      };
    };
    if (pGame->VLineType(point) == LINE_NONE)
    {
      numSide++;
      if (!prevLine.vertical || (prevLine.dotNum!=point))
      {
        nextLine.dotNum = point; nextBox = point; nextBox.x--;
        nextLine.vertical = true;
      };
    };
    point.x++;
    if (pGame->VLineType(point) == LINE_NONE)
    {
      numSide++;
      if (!prevLine.vertical || (prevLine.dotNum!=point))
      {
        nextLine.dotNum = point; nextBox = point;
        nextLine.vertical = true;
      };
    };
    point.x--;
    point.y++;
    if (pGame->HLineType(point) == LINE_NONE)
    {
      numSide++;
      if (prevLine.vertical || (prevLine.dotNum!=point))
      {
        nextLine.dotNum = point; nextBox = point;
        nextLine.vertical = false;
      };
    };
    if (numSide != 2) break;
    prevLine = nextLine;
    box = nextBox;
    if (    (prevLine.dotNum.x == origLine.dotNum.x) 
         && (prevLine.dotNum.y == origLine.dotNum.y)
         && (prevLine.vertical == origLine.vertical)) break;
  };
  prevLine.FindCaptures(pGame);
  return prevLine;
}

void LocateChainEnds(DOTS_AND_BOXES *pGame, MOVE *pMove, int computerMove)
{
  MOVE move;
  MOVE thisLine;
  VECTOR thisBox;
  move.ConstructFromLineNumber(computerMove);
  move.oldLineType = LINE_NONE;
  move.newLineType = LINE_COMPUTER;
  thisLine.oldLineType = LINE_NONE;
  thisLine.newLineType = LINE_COMPUTER;
  if (move.vertical)
  {
    thisBox = move.dotNum;
    thisLine = move;
    pMove[0] = FollowChain(pGame, thisBox, thisLine);
    thisBox.x--;
    pMove[1] = FollowChain(pGame, thisBox, thisLine);
  }
  else
  {
    thisBox = move.dotNum;
    thisLine = move;
    pMove[0] = FollowChain(pGame, thisBox, thisLine);
    thisBox.y--;
    pMove[1] = FollowChain(pGame, thisBox, thisLine);
  };
}


MOVE LocatePenultimateLine(DOTS_AND_BOXES *pGame, MOVE *pMove)
{
  //Given the 'Ultimate' line in a chain of boxes,
  //attempt to locate the 'Penultimate' line.  If the
  //chain is zero boxes long then the result is the
  //same as the 'Ultimate' line.
  //
  //Look on both sides of the 'Ultimate' line.  If a 
  //box on one side or the other has exactly two
  //open sides then choose the other and return;
  VECTOR point;
  MOVE result;
  ui n;
  VECTOR size;
  size = pGame->Size();
  if (pMove->vertical)
  {
    //Look at the boxes to the left and right.
    if (pMove->dotNum.x > 0)
    { //Look to the left
      n = 0;
      point = pMove->dotNum;
      point.x--;
      if (pGame->HLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = false;
      };
      if (pGame->VLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = true;
      };
      point.y++;
      if (pGame->HLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = false;
      };
      if (n==1) return result;
    };
    if (pMove->dotNum.x < size.x-1)
    { //Look to the right
      n = 0;
      point = pMove->dotNum;
      if (pGame->HLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = false;
      };
      point.x++;
      if (pGame->VLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = true;
      };
      point.y++; point.x--;
      if (pGame->HLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = false;
      };
      if (n==1) return result;
    };
  }
  else
  {
    //Look at the boxes above and below.
    if (pMove->dotNum.y > 0)
    { //Look above
      n = 0;
      point = pMove->dotNum;
      point.y--;
      if (pGame->HLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = false;
      };
      if (pGame->VLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = true;
      };
      point.x++;
      if (pGame->VLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = true;
      };
      if (n==1) return result;
    };
    if (pMove->dotNum.y < size.y-1)
    { //Look below
      n = 0;
      point = pMove->dotNum;
      if (pGame->VLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = true;
      };
      point.x++;
      if (pGame->VLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = true;
      };
      point.y++; point.x--;
      if (pGame->HLineType(point) == LINE_NONE)
      {
        n++;
        result.dotNum = point;
        result.vertical = false;
      };
      if (n==1) return result;
    };
  };
  return *pMove;
}

// The problem we need to solve here is the fact that
// the line number associated with a chain may not
// be the line number we want to draw.  This is 
// because the line number associated with a chain
// could be any line within that chain.
//
// But the move type and one of the line numbers
// within a chain will suffice for us to decide
// the proper move unambiguously except in the
// case of a loop, where it makes no difference
// which end of the loop we pick.
//
MOVE DeduceMove(DOTS_AND_BOXES * /*pGame*/,
                const CON *  /*computerMove*/,
                MOVE_TYPE /*mt*/)
{
  MOVE move[2];// For the two end of the chain
  NotImplemented("Deduce Move");
  return move[0];
  /*
  move[0].oldLineType = LINE_NONE;
  move[0].newLineType = LINE_COMPUTER;
  move[1].oldLineType = LINE_NONE;
  move[1].newLineType = LINE_COMPUTER;
  switch (mt)
  {
  case MT_OBVIOUSCAPTURE:
  case MT_GRABANYTHING:
  case MT_SIMPLEWINCAPTURE:
    // This will always be the forth line of a box
    // containing three lines.
    // We need to search boxes both ways from the
    // given line number to find one we can capture.
    LocateChainEnds(pGame, move, computerMove);
    move[0].moveType = mt;
    move[1].moveType = mt;
    if (move[0].boxesCaptured[0].x >= 0) return move[0];
    if (move[1].boxesCaptured[0].x >= 0) return move[1];
    if (move[0].boxesCaptured[1].x >= 0) return move[0];
    if (move[1].boxesCaptured[1].x >= 0) return move[1];
    die ("MT_OBVIOUSCAPTURE cannot find capture");
    break;
  case MT_SIMPLEWINDECLINE:
    LocateChainEnds(pGame, move, computerMove);
    move[0].moveType = mt;
    move[1].moveType = mt;
    if ((move[0].boxesCaptured[0].x < 0)&&(move[0].boxesCaptured[1].x < 0)) 
    {
      return move[0];
    }
    else return move[1];
    break;
  case MT_SIMPLEWIN:
    //This is a non-capturing move.  We broke a link
    //between two stops.  There is only one special case
    //and that is when the link is two nodes long.  We want
    //to play a hard-hearted handout.  We can always play
    //one away from the end.
    LocateChainEnds(pGame, move, computerMove);
    //Choose an end at random;
    if (pDoc->MyRandom(2) == 0) move[0] = move[1];
    //LocateChainEnds has found the last line in the chain - the
    //one separating the chain from the stop.  We want to find
    //the next one.  In the case of a zero-length chain then
    //there is no next one.
    move[1] = LocatePenultimateLine(pGame, &move[0]);
    move[1].moveType = mt;
    return move[1];
  case MT_GIVELITTLE:
    move[0].ConstructFromLineNumber(computerMove);
    move[0].moveType = mt;
    return move[0];
  default: die("DeduceMove unexpected MOVE_TYPE");
  };
  return move[0];
  */
};