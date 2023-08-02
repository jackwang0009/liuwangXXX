#include "stdafx.h"
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
//#include "Nimstring.h"
#include "NewBoxesDoc.h"
#include "Process.h"
#include "NewBoxesView.h"
#include "Utility.h" 

//void SetupLookupTables(void);

ui32 MyRandom(ui32);

extern HWND windowHandle;
extern CNewBoxesView *pViewWindow;
extern int searchDepth;
extern bool enableDump;
SWHASHTABLE *swHashTable = NULL;
NIMHASHTABLE *nimHashTable = NULL;

MOVE_TYPE moveType;
PossibleMove computerMove; //The position following computer's move.
MOVE computerMoveDB;
int availableMoveStack;
//OPENING5x5 opening5x5;


static bool thinking = false;
bool stopThinking = false;
i64 timeLimit;
i64 currentTime;
bool searchAborted;



void ComputerMoveRandom(DOTS_AND_BOXES *pGame)
{
  ui16 possibleMoves[MAX_HEIGHT*(MAX_WIDTH-1)+MAX_WIDTH*(MAX_HEIGHT-1)];
  int row, col;
  int numPossibleMove = 0;
  int width, height;
  int n;
  width = pGame->Width();
  height = pGame->Height();
  //computerMove = 0;
  for (col=0; col<width; col++)
  {
    for (row=0; row<height; row++)
    {
      //Check horizontal line.
      if (col < width-1)
      {
        if (pGame->HLineType(col, row) == LINE_NONE)
        {
          possibleMoves[numPossibleMove++] = (ui16)(col + row*(2*width-1));
        };
      };
      //Check vertical line.
      if (row < height-1)
      {
        if (pGame->VLineType(col, row) == LINE_NONE)
        {
          possibleMoves[numPossibleMove++] = (ui16)(col + row*(2*width-1)+width-1);
        };
      };
    };
  };
  if (numPossibleMove == 0)
  {
    MessageBox(NULL,"Asked to find move.  None exist.","Error",MB_OK);
//    computerMove = -1;
    return;
  };
  n = pDoc->MyRandom(numPossibleMove);
  computerMoveDB.dotNum.y = (possibleMoves[n])/(2*width-1);
  computerMoveDB.dotNum.x = (possibleMoves[n])%(2*width-1);
  computerMoveDB.vertical = false;
  if (computerMoveDB.dotNum.x >= width-1)
  {
    computerMoveDB.dotNum.x -= width-1;
    computerMoveDB.vertical = true;
  };
  moveType = MT_RANDOM;
}



/*
bool CON::_GrabAnything(void) const
{
  
  NotImplemented("GrabAnything");

  // Grab any available coin.
  unsigned int f, l, lnum, linkIndex;
  f = 0;
  l = m_totalNumLink;
  for (lnum=f; lnum<l; lnum++)
  {
    linkIndex = LS*lnum;
    if (   (m_links[linkIndex] == DEADEND)
        || (m_links[linkIndex+1] == DEADEND))
    {
      //We found a link with a captureable coin at one of the ends.
      moveType = MT_GRABANYTHING;
//      computerMove = m_links[linkIndex + 3];
      return true;
    };
  };
  return false;
}
*/


#ifdef PREAMBLE01
#define NUM_DIVIDES_TO_CONSIDER 5
#endif
#ifdef PREAMBLE02
#define NUM_DIVIDES_TO_CONSIDER 2
#endif


void  DOTS_AND_BOXES::DivideAndConquer(void)
{
  int i, n;
  int width, height;
  int totalWeight;
  //POINT dotNum;
  //int numPossible;
  int  lines[NUM_DIVIDES_TO_CONSIDER];
  //int  weights[NUM_DIVIDES_TO_CONSIDER];
  //flow.ComputeFlow(this);
  width = Width();
  height = Height();
  totalWeight = 0;
  for (i=0; i<NUM_DIVIDES_TO_CONSIDER; i++)
  {
    totalWeight += m_divideAndConquerWeight[lines[i]];
  };
  n = MyRandom(totalWeight);
  for (i=0; i<NUM_DIVIDES_TO_CONSIDER; i++)
  {
    n -= m_divideAndConquerWeight[lines[i]];
    if (n <= 0)
    {
    };
  };
  die("Internal Error");
}


void FormRanks(int *weights, int *rank, int num)
{
  //Arrange so largest weight comes first.
  int i, ns, j, k;
  for (i=0; i<num; i++)
  {
    rank[i] = i;
  };
  //Shuffle so that after the sort lines with the same weight
  //are arranged at random.
  for (i=num-1; i>0; i--)
  {
    int j, temp;
    j = MyRandom(i+1);
    temp = rank[j];
    rank[j] = rank[i];
    rank[i] = temp;
  };
  for (ns=num/2; ns>0; ns/=2)
  {
    for (i=0; i<ns; i++)
    {
      for (j=i; j<num-ns; j+=ns)
      {
        for (k=j; (k>=0)&&(weights[rank[k]]<weights[rank[k+ns]]); k-=ns)
        {
          int temp;
          temp = rank[k];
          rank[k] = rank[k+ns];
          rank[k+ns] = temp;
        };
      };
    };
  };
}


void DOTS_AND_BOXES::FindAvoids()
{
  int i;
  int width, height, numEdges;
  POINT dotNum;
  width = Width();
  height = Height();
  numEdges = width*(height-1) + height*(width-1);
  for (i=0; i<numEdges; i++)
  {
    m_avoidFlgs[i] = 0;
  };
  for (dotNum.x=0; dotNum.x<width; dotNum.x++)
  {
    for (dotNum.y=0; dotNum.y<height; dotNum.y++)
    {
      int hline, vline, count;
      hline = dotNum.y*(2*width-1) + dotNum.x;
      vline = hline + width - 1;
      {
        // Mark the edges
        if ((dotNum.x == 0) && (dotNum.y < height-1))
        {
          m_avoidFlgs[vline] |= AV_PERIMETER;
        };
        if ((dotNum.x == width-1) && (dotNum.y < height-1))
        {
          m_avoidFlgs[vline] |= AV_PERIMETER;
        };
        if ((dotNum.y == 0) && (dotNum.x < width-1))
        {
          m_avoidFlgs[hline] |= AV_PERIMETER;
        };
        if ((dotNum.y == height-1) && (dotNum.x < width-1))
        {
          m_avoidFlgs[hline] |= AV_PERIMETER;
        };
      };
      if (   (dotNum.x < width-1)
           &&(HLineType(dotNum.x, dotNum.y) != LINE_NONE)
         )
      {
        m_avoidFlgs[hline] |= AV_OCCUPIED;
      };
      if (   (dotNum.y<height-1)
           &&(VLineType(dotNum.x, dotNum.y) != LINE_NONE)
         )
      {
        m_avoidFlgs[vline] |= AV_OCCUPIED;
      };
      if ((dotNum.x < width-1) && (dotNum.y<height-1))
      {
        count = CountLines(dotNum);
        if (count >= 2)
        {
          m_avoidFlgs[hline] |= AV_SACRIFICE;
          m_avoidFlgs[vline] |= AV_SACRIFICE;
          m_avoidFlgs[hline+2*width-1] |= AV_SACRIFICE;
          m_avoidFlgs[vline+1] |= AV_SACRIFICE;
        };
      };
    };
  };
}

bool DOTS_AND_BOXES::GiveNothing_BreakLoops(int minWeight, int *loopRank) const
{
  int width, height, numEdges;
  width = Width();
  height = Height();
  numEdges = NumEdges();
  if (m_breakUpLoopsWeight[loopRank[0]] >= minWeight)
  {
    int n, nLoop;
    for ( nLoop=0; 
                (nLoop<numEdges)
             && (    m_breakUpLoopsWeight[loopRank[nLoop]] 
                  == m_breakUpLoopsWeight[loopRank[0]]
                ); 
          nLoop++
        )
    {};
    n = loopRank[MyRandom(nLoop)];
    moveType = MT_BREAKLOOPS;
    computerMoveDB.moveType = MT_BREAKLOOPS;
    computerMoveDB.dotNum.y = n/(2*width-1);
    computerMoveDB.dotNum.x = n%(2*width-1);
    computerMoveDB.vertical = false;
    if (computerMoveDB.dotNum.x >= width-1)
    {
      computerMoveDB.dotNum.x -= width-1;
      computerMoveDB.vertical = true;
    };
    return true;
  };
  return false;
}


bool DOTS_AND_BOXES::GiveNothing_BothLoopAndDivide(
             int searchDepth, 
             int *loopRank, 
             int *divideRank)
{
  int nLoop, nDivide;
  int width, height, numEdges;
  width = Width();
  height = Height();
  numEdges = NumEdges();
  for (nLoop=nDivide=0; searchDepth>0; searchDepth--)
  {
    nLoop++;
    nDivide++;
    if (m_breakUpLoopsWeight[loopRank[nLoop]] == 0) return false;
    if (m_divideAndConquerWeight[divideRank[nDivide]] == 0) return false;
    while (    (nLoop<numEdges) 
            && (    m_breakUpLoopsWeight[loopRank[nLoop]] 
                 == m_breakUpLoopsWeight[loopRank[nLoop-1]]
               )
          )
    {
      nLoop++;
    };
    while (    (nDivide<numEdges) 
            && (    m_divideAndConquerWeight[divideRank[nDivide]] 
                 == m_divideAndConquerWeight[divideRank[nDivide-1]]
               )
          )
    {
      nDivide++;
    };
    //Look for an edge in both groups.
    for (int i=0; i<nLoop; i++)
    {
      int j;
      for (j=0; j<nDivide; j++)
      {
        if (divideRank[j] == loopRank[i])
        {
          moveType = MT_LOOPDIVIDE;
          computerMoveDB.moveType = MT_LOOPDIVIDE;
          computerMoveDB.dotNum.y = (divideRank[j])/(2*width-1);
          computerMoveDB.dotNum.x = (divideRank[j])%(2*width-1);
          computerMoveDB.vertical = false;
          if (computerMoveDB.dotNum.x >= width-1)
          {
            computerMoveDB.dotNum.x -= width-1;
            computerMoveDB.vertical = true;
          };
          return true;
        };
      };
    };
  };
  return false;
}

bool DOTS_AND_BOXES::GiveNothing_Divide(int *divideRank, int minWeight)
{
  if (m_divideAndConquerWeight[divideRank[0]] >= minWeight)
  {
    moveType = MT_DIVIDE;
    computerMoveDB.moveType = MT_DIVIDE;
    computerMoveDB.dotNum.y = (divideRank[0])/(2*m_width-1);
    computerMoveDB.dotNum.x = (divideRank[0])%(2*m_width-1);
    computerMoveDB.vertical = false;
    if (computerMoveDB.dotNum.x >= m_width-1)
    {
      computerMoveDB.dotNum.x -= m_width-1;
      computerMoveDB.vertical = true;
    };
    return true;
  };
  return false;
}


void DOTS_AND_BOXES::GiveNothing()
{
  int width, height, numEdges;
  int loopRank[MAX_EDGES], divideRank[MAX_EDGES];
  width = Width();
  height = Height();
  numEdges = height*(width-1) + width*(height-1);
  FindAvoids();
  m_loopWeightsValid = false;
  m_divideWeightsValid = false;
  ComputeDivideAndConquerWeights_3();
  FormRanks(m_divideAndConquerWeight, divideRank, numEdges);
  ComputeBreakUpLoopWeights(4);
  FormRanks(m_breakUpLoopsWeight, loopRank, numEdges);
  if (GiveNothing_BreakLoops(12, loopRank))
  {
//    m_loopWeightsValid = false;
//    ComputeBreakUpLoopWeights(3);
//    FormRanks(m_breakUpLoopsWeight, loopRank, numEdges);
//    if (GiveNothing_BreakLoops(6, loopRank))
    {
      return;
    };
  }
  if (GiveNothing_Divide(divideRank, 1)) return;
  ComputeBreakUpLoopWeights(2);
  if (GiveNothing_BreakLoops(9, loopRank)) return;
  if (GiveNothing_BothLoopAndDivide(5, loopRank, divideRank)) return;
  if (GiveNothing_BreakLoops(1, loopRank)) return;
  if (GiveNothing_Divide(divideRank, 1)) return;
}

/*
void CON::GiveNothing(void)
{
  //Our plan is to find a joint with three links attached
  //and see if one of those links has length zero.
  //Breaking such a link will not give our opponent
  //anthing to capture(at least immediately);
//  unsigned int stringNum;
  ui16 possibleMoves[MAX_HEIGHT*(MAX_WIDTH-1)+MAX_WIDTH*(MAX_HEIGHT-1)];
  int numPossible = 0;
//  for (stringNum=0; stringNum<NumString(); stringNum++)
//  {
//    SetupString(stringNum);
    _GiveNothing(possibleMoves, &numPossible);
//  };
  if (numPossible == 0) return;
  moveType = MT_GIVENOTHING;
//  computerMove = possibleMoves[pDoc->MyRandom(numPossible)];
}
*/

void CON::GiveLittle(void) const
{
  int nn, nnA, en;
  int length;
  int numPossible=0;
  int shortest = 99999;
  STOPNUM possibleNodes[MAX_NODES];
  EDGENUM possibleEdges[MAX_NODES];
  for (nn=0; nn<m_numNode; nn++)
  {
    length = -1;
    nnA = nd[nn].ix;
    if (   (sd[nnA][0].ns == LOOP)
        || (sd[nnA][0].ns == GND_GND))
    {
      length = ed[nnA].e[0].el;
      en = 0;
      if (length > shortest) continue;
      if (length < shortest) numPossible = 0;
      possibleNodes[numPossible] = (STOPNUM)nnA;
      possibleEdges[numPossible] = (EDGENUM)en;
      shortest = length;
      numPossible++;
    }
    else
    {
      for (en=0; en<nd[nnA].ec; en++)
      {
        if (    (sd[nnA][en].ns == GROUND)
             || (sd[nnA][en].ns >= nnA))
        {
          if (sd[nnA][en].ns == nnA)
          {
            if (nd[nnA].ec < 4) 
            {
              continue; //Deleting the loop sacrifices the tail, too!
            };
          };
          length = ed[nnA].e[en].el;
          if (length > shortest) continue;
          if (length < shortest) numPossible = 0;
          possibleNodes[numPossible] = (STOPNUM)nnA;
          possibleEdges[numPossible] = (EDGENUM)en;
          shortest = length;
          numPossible++;
        };
      };
    };
  };
  if (numPossible > 0)
  {
    int i;
    i = pDoc->MyRandom(numPossible);
    nnA = possibleNodes[i];
    en = possibleEdges[i];
    computerMove.con = *this;
    moveType = MT_GIVELITTLE;
    if (sd[nnA][0].ns == LOOP)
    {
        computerMove.con.DeleteNode(nd[nnA].os);
      //NotImplemented("Give Loop");
    }
    else if (sd[nnA][0].ns == GND_GND)
    {
      if (ed[nnA].e[0].el == 1)
      {
        computerMove.con.DeleteNode(nd[nnA].os);
      }
      else
      {
        //We need to change it into two GND_DE of length one and
        //length len-1;
        computerMove.con.DeleteNode(nd[nnA].os);
      };
    }
    else if (sd[nnA][en].ns == GROUND)
    {
      computerMove.con.DeleteEdge(nd[nnA].os, en);
    }
    else // Must be a node at the other end
    {
      computerMove.con.DeleteEdge(nd[nnA].os, en);
    };
    computerMove.con.Canon(0,computerMove.con.m_numNode);
  };
}

/*
void CON::GiveLittle()
{
  NotImplemented("GiveLittle");
  //Our plan is to find a joint with three links attached
  //Add the lengths of such chains to the list and then
  //choose from among the shortest at random.
  ui16 possibleMoves[MAX_HEIGHT*(MAX_WIDTH-1)+MAX_WIDTH*(MAX_HEIGHT-1)];
//  unsigned int stringNum;
  int numPossible = 0;
  int shortest = 9999999;
//  for (stringNum=0; stringNum<NumString(); stringNum++)
//  {
//    SetupString(stringNum);
    _GiveLittle(possibleMoves, &numPossible, &shortest);
//  };
  if (numPossible == 0) return;
  moveType = MT_GIVELITTLE;
//  computerMove = possibleMoves[pDoc->MyRandom(numPossible)];
}
*/




int CON::MaxCostOfNimZero(int direction,
                             int depth) const
{
  // result is the cost of reducing the nimValue to zero.
  // If direction == 1 then we maximize our opponent's cost of
  //     forcing a nimvalue of zero.
  // If direction == -1 then we minimize our cost of reducing the
  //     nimvalue to zero.
  int nn, maxCost, nimval;
  CON pm;
  NIMCON nimcon;
#ifdef _CHECK
  CheckEverything(0, m_numNode, false);
#endif
  if (depth == 0) return 0;
  maxCost = -9999*direction;
  for (nn=0; nn<m_numNode; nn++)
  {
    int nnA, en, ec, nnB, offering;
    nnA = nd[nn].ix;
    if (IS_ISOLATED(sd[nnA][0].ns))
    {
      if (nn < m_numNode-1)
      {
        nnB = nd[nn+1].ix;
        if (sd[nnA][0].ns == sd[nnB][0].ns)
        {
          continue; //The shorter one comes later.
        };
      };
      if (sd[nnA][0].ns == GND_GND)
      {
        int len;
        len = ed[nnA].e[0].el;
        if (len > 2) continue;
        pm = *this;
        offering = len;
        pm.DeleteNode(nn);
      }
      else if (sd[nnA][0].ns == LOOP)
      {
        continue;
      }
      else
      {
        offering = 0;
        NotImplemented("Unknown isolated chain");
      };
      if (pm.m_numNode == 0)
      {
        if (direction < 0)
        {
          maxCost = offering;
          //ImplementationError("Expected non-zero nimval");
        }
        else
        {
          int cost;
          cost = offering;
          if (cost > maxCost) maxCost = cost;
        };
      }
      else
      {
        pm.Canon(0, pm.m_numNode);
        nimval = nimcon.NimVal(&pm);
        if (direction > 0) // If looking for max
        {
          if (nimval == 0)
          {
            ImplementationError("Expected non-zero nimval");
          }
          else
          {
            int cost;
            cost = -offering + pm.MaxCostOfNimZero(-direction, depth-1);
            if (cost > maxCost) maxCost = cost;
          };
        }
        else
        {
          if (nimval != 0) continue; //We need to find a move with nimval==0.
          int cost;
          cost = offering + pm.MaxCostOfNimZero(-direction, depth-1);
          if (cost < maxCost) maxCost = cost;
        };
      };
    }
    else
    {
      if ((nn>0) && (ed[nnA] == ed[nd[nn-1].ix])) continue;
      ec = nd[nnA].ec;
      for (en=0; en<ec; en++)
      {
        int len;
        if ((en>0) && (ed[nnA].e[en-1] == ed[nnA].e[en])) 
        {
          if (sd[nnA][en-1].ns != nnA)
          {
            continue;
          };
        };
        nnB = sd[nnA][en].ns;
        if (IS_A_NODE(nnB) && (nd[nnB].os < nn)) continue; //Only one direction, please
        if (nnB == DEADEND)
        {
          //Well, this is unexpected (in general).  But taking
          //all these coins and/or offering a double-cross
          //is guaranteed to be a proper move.
          //LOGUIUI("Zero Length string=%d, link=%d", stringNum, ln);
          NotImplemented("Capturable to joint");
        };
        len = ed[nnA].e[en].el;
        if (nnB == nnA)
        {
          //A loop to ourselves.  A loony move at best.
          if (ec == 3) 
          {
            continue; //I cannot see how this would help us
          };
        };
        if (len > 2) continue;
        pm = *this;
        offering = len;
        pm.DeleteEdge(nn, en);
        pm.Canon(0, pm.m_numNode);

        nimval = nimcon.NimVal(&pm);
        if (direction > 0) // If looking for max
        {
          if (nimval == 0)
          {
            ImplementationError("Expected non-zero nimval");
          }
          else
          {
            int cost;
            cost = offering + pm.MaxCostOfNimZero(-direction, depth-1);
            if (cost > maxCost) maxCost = cost;
          };
        }
        else
        {
          if (nimval != 0) continue; //We need to find a move with nimval==0.
          int cost;
          cost = offering + pm.MaxCostOfNimZero(-direction, depth-1);
          if (cost < maxCost) maxCost = cost;
        };
      };
    };
  };
#ifdef SW_DUMP
  searchDepth = 8-depth;
  DUMP("MaxCostOfNimZero", 23, direction, maxCost, 0, m_numNode);
#endif
  return maxCost;
}


MOVE TranslateSimpleWin(const DOTS_AND_BOXES *pGame, const CON *computerMove)
{
  int row, col;
  POINT size;
  MOVE result;
  CON con;
  int initialCaptures, newCaptures;
  int finalCaptures = -1;
  result.dotNum.x = -1;
  size = pGame->Size();
  DOTS_AND_BOXES db(size.x, size.y);
  initialCaptures = pGame->CountCapturables();
  for (row=0; row<size.y; row++)
  {
    for (col=0; col<size.x; col++)
    {
      if (col < size.x-1)
      {
        if (pGame->HLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetHLineType(col,row,LINE_COMPUTER);
          newCaptures = db.CountCapturables();
          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
            if (newCaptures >= initialCaptures)
            {
              if (newCaptures > finalCaptures)
              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = false;
                result.moveType = moveType;
                finalCaptures = newCaptures;
              };
            };
          };
        };
      };
      if (row < size.y-1)
      {
        if (pGame->VLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetVLineType(col,row,LINE_COMPUTER);
          newCaptures = db.CountCapturables();
          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
            if (newCaptures >= initialCaptures)
            {
              if (newCaptures > finalCaptures)
              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = true;
                result.moveType = moveType;
                finalCaptures = newCaptures;
              };
            };
          };
        };
      };
    };
  };
  if (result.dotNum.x == -1)
  {
    ImplementationError("Cannot determine computer's move");
  };
  return result;
}

MOVE TranslateSimpleWinDecline(const DOTS_AND_BOXES *pGame, const CON *computerMove)
{
  int row, col;
  POINT size;
  MOVE result;
  CON con;
  int initialCaptures, newCaptures;
  int finalCaptures = -1;
  size = pGame->Size();
  DOTS_AND_BOXES db(size.x, size.y);
  db = *pGame;
  initialCaptures = db.CountCapturables();
  result.dotNum.x = -1;
  for (row=0; row<size.y; row++)
  {
    for (col=0; col<size.x; col++)
    {
      if (col < size.x-1)
      {
        if (pGame->HLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetHLineType(col,row,LINE_COMPUTER);
          newCaptures = db.CountCapturables();
          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
            if (newCaptures >= initialCaptures)
            {
              if (newCaptures > finalCaptures)
              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = false;
                result.moveType = moveType;
                finalCaptures = newCaptures;
              };
            };
          };
        };
      };
      if (row < size.y-1)
      {
        if (pGame->VLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetVLineType(col,row,LINE_COMPUTER);
          newCaptures = db.CountCapturables();
          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
            if (newCaptures >= initialCaptures)
            {
              if (newCaptures > finalCaptures)
              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = true;
                result.moveType = moveType;
                finalCaptures = newCaptures;
              };
            };
          };
        };
      };
    };
  };
  if (result.dotNum.x == -1)
  {
    ImplementationError("Cannot determine computer's move");
  };
  return result;
}

MOVE TranslateSimpleWinCapture(const DOTS_AND_BOXES *pGame, const CON *computerMove)
{
  int row, col;
  POINT size;
  MOVE result;
  CON con;
  int initialCaptures, newCaptures;
  int finalCaptures = 999999;
  size = pGame->Size();
  DOTS_AND_BOXES db(size.x, size.y);
  db = *pGame;
  initialCaptures = db.CountCapturables();
  result.dotNum.x = -1;
  for (row=0; row<size.y; row++)
  {
    for (col=0; col<size.x; col++)
    {
      if (col < size.x-1)
      {
        if (pGame->HLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetHLineType(col,row,LINE_COMPUTER);
          newCaptures = db.CountCapturables();
          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
            if (newCaptures <= initialCaptures)
            {
              if (newCaptures <= finalCaptures)
              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = false;
                result.moveType = moveType;
                finalCaptures = newCaptures;
              };
            };
          };
        };
      };
      if (row < size.y-1)
      {
        if (pGame->VLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetVLineType(col,row,LINE_COMPUTER);
          newCaptures = db.CountCapturables();
          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
            if (newCaptures <= initialCaptures)
            {
              if (newCaptures <= finalCaptures)
              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = true;
                result.moveType = moveType;
                finalCaptures = newCaptures;
              };
            };
          };
        };
      };
    };
  };
  if (result.dotNum.x == -1)
  {
    ImplementationError("Cannot determine computer's move");
  };
  return result;
}


MOVE TranslateSimpleMove(const DOTS_AND_BOXES *pGame, const CON *computerMove)
{
  int row, col;
  POINT size;
  MOVE result;
  CON con;
//  int initialCaptures, newCaptures;
//  int finalCaptures = -1;
  size = pGame->Size();
  DOTS_AND_BOXES db(size.x, size.y);
//  initialCaptures = db.CountCapturables();
  result.dotNum.x = -1;
  for (row=0; row<size.y; row++)
  {
    for (col=0; col<size.x; col++)
    {
      if (col < size.x-1)
      {
        if (pGame->HLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetHLineType(col,row,LINE_COMPUTER);
//          newCaptures = db.CountCapturables();
//          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
//            if (newCaptures >= initialCaptures)
//            {
//              if (newCaptures > finalCaptures)
//              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = false;
                result.moveType = moveType;
//                finalCaptures = newCaptures;
//              };
//            };
          };
        };
      };
      if (row < size.y-1)
      {
        if (pGame->VLineType(col,row) == LINE_NONE)
        {
          db = *pGame;
          db.SetVLineType(col,row,LINE_COMPUTER);
//          newCaptures = db.CountCapturables();
//          db.RemoveCapturables();
          con = db;
          if (con == *computerMove)
          {
//            if (newCaptures >= initialCaptures)
//            {
//              if (newCaptures > finalCaptures)
//              {
                result.dotNum.x = col;
                result.dotNum.y = row;
                result.vertical = true;
                result.moveType = moveType;
//                finalCaptures = newCaptures;
//              };
//            };
          };
        };
      };
    };
  };
  if (result.dotNum.x == -1)
  {
    ImplementationError("Cannot determine computer's move");
  };
  return result;
}

MOVE TranslateOpeningMove(const DOTS_AND_BOXES *pGame)
{
  MOVE result;
  int width;
  result.moveType = moveType;
  width = pGame->Width();
  result.dotNum.y = computerMove.lineNum/(2*width-1);
  result.dotNum.x = computerMove.lineNum%(2*width-1);
  result.vertical = false;
  if (result.dotNum.x >= width-1)
  {
    result.dotNum.x -= width-1;
    result.vertical = true;
  };
  return result;
}




MOVE TranslateComputerMove(const DOTS_AND_BOXES *pGame,
                           MOVE_TYPE moveType, 
                           const CON *computerMove)
{
  // Given the Dots-and-Boxes gaame and the 
  // CON representation of the game after the computer's 
  // move.  We need to find which line to draw.
  // We will try **ALL** the lines, one-at-a-time, and see
  // which one will produce the desired result.
  POINT size;
  MOVE result;
  CON con;
  size = pGame->Size();
  DOTS_AND_BOXES db(size.x, size.y);
  db = *pGame;
  if (moveType == MT_RANDOM) return computerMoveDB;
  result.dotNum.x = -1;
//  if (moveType == MT_GRABANYTHING)
//          return TranslateSimpleWin(pGame, computerMove);
  if (moveType == MT_SIMPLEWIN)
          return TranslateSimpleWin(pGame, computerMove);
  if (moveType == MT_SIMPLETIE)
          return TranslateSimpleWin(pGame, computerMove);
  if (moveType == MT_SIMPLEWINDECLINE)
          return TranslateSimpleWinDecline(pGame, computerMove);
  if (moveType == MT_MAXDECLINE)
          return TranslateSimpleWinDecline(pGame, computerMove);
  if (moveType == MT_NIMVALDECLINE)
          return TranslateSimpleWinDecline(pGame, computerMove);
  if (moveType == MT_SIMPLEWINCAPTURE)
          return TranslateSimpleWinCapture(pGame, computerMove);
  if (moveType == MT_MAXCAPTURE)
          return TranslateSimpleWinCapture(pGame, computerMove);
  if (moveType == MT_NIMVALCAPTURE)
          return TranslateSimpleWinCapture(pGame, computerMove);
  if (moveType == MT_GRABANYTHING)
          return TranslateSimpleMove(pGame, computerMove);
  if (moveType == MT_OBVIOUSCAPTURE)
          return TranslateSimpleMove(pGame, computerMove);
  if (    (moveType == MT_GIVENOTHING)
       || (moveType == MT_BREAKLOOPS)
       || (moveType == MT_LOOPDIVIDE)
       || (moveType == MT_DIVIDE)
     )
       //return TranslateSimpleMove(pGame, computerMove);
          return computerMoveDB;
  if (moveType == MT_GIVELITTLE)
          return TranslateSimpleWin(pGame, computerMove);
  if ((moveType == MT_NIMVAL) || (moveType == MT_NONIMVAL))
          return TranslateSimpleWin(pGame, computerMove);
  if ((moveType == MT_OPENINGTIE) || (moveType == MT_OPENINGWIN))
          return TranslateOpeningMove(pGame);
  ImplementationError("Cannot determine computer's move");
  return result;
};


LONG_CHAIN_INFO CON::ComputeLongChainInfo(void)
{
  LONG_CHAIN_INFO result;
  result.longestChain = 0;
  result.numLongChain = 0;
  result.sumOfLongChains = 0;
  result.gain = 0;
  int nn, edge;
  for (nn=0; nn<m_numNode; nn++)
  {
    int nnA, len, neighNodeA;
    nnA = nd[nn].ix;
    neighNodeA = sd[nnA][0].ns;
    if (IS_ISOLATED(neighNodeA))
    {
      len = ed[nnA].e[0].el;
      if (neighNodeA == GND_GND)
      {
        if (len > 2)
        {
          if (result.longestChain < len) result.longestChain = len;
          result.numLongChain++;
          result.sumOfLongChains += len;
          result.gain += len-4;
        };
      }
      else if (neighNodeA == GND_DE)
      {
        if (len > 1)
        {
          if (result.longestChain < len) result.longestChain = len;
          result.numLongChain++;
          result.sumOfLongChains += len;
          result.gain += len-4;
        };
      }
      else if (neighNodeA == LOOP)
      {
        if (len > 3)
        {
          if (result.longestChain < len) result.longestChain = len;
          result.numLongChain++;
          result.sumOfLongChains += len;
          result.gain += len-8;
        };
      }
      else
      {
        NotImplemented("ISOLATED EDGE");
      };
    }
    else
    {
      for (edge=0; edge<nd[nnA].ec; edge++)
      {
        len = ed[nnA].e[edge].el;
        neighNodeA = sd[nnA][edge].ns;
        if (IS_A_NODE(neighNodeA))
        {
          if (    (neighNodeA > nnA)
              || ((neighNodeA == nnA) && (sd[nnA][edge].ne > edge))
            )
          {
            if (len > 2)
            {
              if (result.longestChain < len) result.longestChain = len;
              result.numLongChain++;
              result.sumOfLongChains += len;
              result.gain += len-4;
            };
          }
          else
          {
            continue; //Duplicate -  Don't count it twice
          };
        }
        else if (neighNodeA == GROUND)
        {
          if (len > 2)
          {
            result.numLongChain++;
            if (len > result.longestChain) result.longestChain = len;
            result.sumOfLongChains += len;
            result.gain += len-4;
          };
        }
        else if (neighNodeA == DEADEND)
        {
          if (len > 1)
          {
            result.numLongChain++;
            if (len > result.longestChain) result.longestChain = len;
            result.sumOfLongChains += len;
            result.gain += len-4;
          };
        }
        else
        {
          NotImplemented("Unknown edge type");
        };
      };
    }
  };
  return result;
}

LONG_CHAIN_INFO CON::ComputeLongChainInfo(DOTS_AND_BOXES *pDB, MOVE *pMove)
{
  CON con;
  LONG_CHAIN_INFO result;
  if (pMove->vertical) pDB->SetVLineType(pMove->dotNum, LINE_COMPUTER);
  else pDB->SetHLineType(pMove->dotNum, LINE_COMPUTER);
  con = *pDB;
  result = con.ComputeLongChainInfo();
  if (pMove->vertical) pDB->SetVLineType(pMove->dotNum, LINE_NONE);
  else pDB->SetHLineType(pMove->dotNum, LINE_NONE);
  return result;
}


bool MoveQualityGT(int *breakWeights, LONG_CHAIN_INFO *lci, int i, int j)
{
  if (lci[i].longestChain > lci[j].longestChain) return true;
  if (lci[i].longestChain < lci[j].longestChain) return false;
  if (lci[i].gain > lci[j].gain) return true;
  if (lci[i].gain < lci[j].gain) return false;
  return breakWeights[i] > breakWeights[j];
}

void CON::TheGeneralCase(
                 ui width,
                 ui height,
                 ui computerScore, 
                 ui humanScore,
                 PossibleMove *pm)
{
  unsigned int totalCoins;
  SWVALUE moveValue;
#ifndef HASH_CARRY
  if (swHashTable != NULL) die("Forgot to release hash table");
  swHashTable = new SWHASHTABLE;
  if (nimHashTable != NULL) delete nimHashTable;
  nimHashTable = new NIMHASHTABLE;
#endif
//  MessageBox(NULL,"aaa","note",MB_OK);
//  Note("Start test capture");
  moveValue = CheckSWCapture(computerScore, humanScore, pm);
//  Note("end test capture");
//  MessageBox(NULL,"a","note",MB_OK);
  ASSERT(m_coinsRemaining == (width-1)*(height-1) - computerScore - humanScore);
  if (     (moveValue.moveType == MT_SIMPLEWINCAPTURE)
        || (moveValue.moveType == MT_SIMPLEWINDECLINE)
        || (moveValue.moveType == MT_NIMVALCAPTURE)
        || (moveValue.moveType == MT_NIMVALDECLINE)
        || (moveValue.moveType == MT_MAXCAPTURE)
        || (moveValue.moveType == MT_MAXDECLINE)
     )
  {
    //We are supposed to play in a capturable chain.
//    computerMove = moveValue.bestMove;
    //computerMove = pm[moveValue.bestMove];
    moveType = moveValue.moveType;
#ifndef HASH_CARRY
    delete swHashTable;
    swHashTable = NULL;
    delete nimHashTable;
    nimHashTable = NULL;
#endif
    free (pm);
    CLOSEDUMP
    return;
  }
  else if (moveValue.moveType == MT_SIMPLEWINDECLINE)
  {
    //computerMove = pm[moveValue.bestMove];
    moveType = moveValue.moveType;
#ifndef HASH_CARRY
    delete swHashTable;
    swHashTable = NULL;
    delete nimHashTable;
    nimHashTable = NULL;
#endif
    free (pm);
    CLOSEDUMP
    return;
  };
//for (int kkk=0; kkk<100000; kkk++)
  moveValue = SWEvaluateAllMoves(computerScore, humanScore, pm);
  if (searchAborted) 
  {
    moveValue.player1Min = 0;
    moveValue.player2Min = 0;
    moveValue.moveType = MT_NONE;
  };
  ASSERT(m_coinsRemaining == (width-1)*(height-1) - computerScore - humanScore);
  if (computerScore+moveValue.player1Min >= m_neededToWin)
  {
//    computerMove = moveValue.bestMove;
    moveType = MT_SIMPLEWIN;
    computerMove = pm[moveValue.bestMove];
#ifndef HASH_CARRY
    delete swHashTable;
    swHashTable = NULL;
    delete nimHashTable;
    nimHashTable = NULL;
#endif
    free (pm);
    CLOSEDUMP
    return;
  };
  // Accept a tie if that is the best we can get
  totalCoins = (width-1)*(height-1);
  if (computerScore+moveValue.player1Min >= m_neededToTie)
  {
//    computerMove = moveValue.bestMove;
    moveType = MT_SIMPLETIE;
    computerMove = pm[moveValue.bestMove];
#ifndef HASH_CARRY
    delete swHashTable;
    swHashTable = NULL;
    delete nimHashTable;
    nimHashTable = NULL;
#endif
    free (pm);
    CLOSEDUMP
    return;
  };
  //enableDump = true;
  if (moveValue.numPossibleMove > 0)
  {
    int i, move;
    int cost, bestCost;;
    ui8 bestMoves[MAX_EDGES];
    int numBestMove;
    bool zeroNimvalPossible;
    numBestMove = 0;
    //Among the moves that reduce the nimval to zero, we will
    //search for the one that will cost us the fewest coins in
    //this move and the next.
    bestCost = 9999;
    move = -1;
    searchAborted = false;
    zeroNimvalPossible = false;
    for (i=0; i<moveValue.numPossibleMove; i++)
    {
      if (pm[i].nimval == 0)
      {
        zeroNimvalPossible = true;
     //   if (pm[i].offering > 0) continue;//Don't sacrifice to maintain nimval.
        NIMCON nimcon;
        nimcon = &pm[i].con;
        //nimcon.Simplify();
        nimcon.Canon(0, nimcon.m_numNode);
        if (moveValue.allNimvalComputed)
        {
          cost = pm[i].offering + nimcon.MaxCostOfNimZero(1, 2); //Find the largest response.
          cost = 1000*cost + pm[i].offering;
        }
        else
        {
          cost = pm[i].offering;
        };
        if (cost < bestCost) numBestMove = 0;
        if (cost <= bestCost)
        {
          bestCost = cost;
          bestMoves[numBestMove++] = (ui8)i;
        };
      };
    };
    if (numBestMove != 0)
    {
      MOVE results[100];
      int moveNum[100];
      LONG_CHAIN_INFO longChainInfo[100];
      int  breakWeights[100];
      if (numBestMove > 100) numBestMove = 100;
      if (numBestMove > 1)
      {
        int j;
        for (i=0; i<numBestMove; i++)
        {
          pm[bestMoves[i]].con.Canon(0, pm[bestMoves[i]].con.m_numNode);
          results[i] = TranslateComputerMove(&pDoc->Game(), MT_NIMVAL, &pm[bestMoves[i]].con);
          moveNum[i] = i;
        };
        //Randomly choose among the equally good 'bestMoves'.
        pDoc->Game().FindAvoids();
        pDoc->Game().ComputeBreakUpLoopWeights(1);
        for(i=0; i<numBestMove; i++)
        {
          int line;
          line = pDoc->Game().Dot2Line(results[i].dotNum, results[i].vertical);
          breakWeights[i] = pDoc->Game().LoopWeight(results[i].dotNum, results[i].vertical);
          longChainInfo[i] = ComputeLongChainInfo(&pDoc->Game(), &results[i]);
        };
        for (i=0; i<numBestMove-1; i++)
        {
          int j;
          for (j=i; (j>=0) && MoveQualityGT(breakWeights, longChainInfo, j+1, j); j--)
          {
            MOVE tr;
            LONG_CHAIN_INFO tlci;
            int  tw;
            tw = breakWeights[j];
            breakWeights[j] = breakWeights[j+1];
            breakWeights[j+1] = tw;
            tr = results[j];
            results[j] = results[j+1];
            results[j+1] = tr;
            tw = moveNum[j];
            moveNum[j] = moveNum[j+1];
            moveNum[j+1] = tw;
            tlci = longChainInfo[j];
            longChainInfo[j] = longChainInfo[j+1];
            longChainInfo[j+1] = tlci;
          };
        };
        for (j=0; (j<numBestMove-1) && !MoveQualityGT(breakWeights, longChainInfo, j, j+1); j++){};
        i = pDoc->MyRandom(j+1);
        computerMove = pm[bestMoves[moveNum[i]]];
        moveType = MT_NIMVAL;
      }
      else
      {
        i = 0;
        computerMove = pm[bestMoves[i]];
        computerMove.con.Canon(0, computerMove.con.m_numNode);
        moveType = MT_NIMVAL;
      };
    }
    else if (moveValue.allNimvalComputed && !zeroNimvalPossible)
    {
      //All moves result in non-zero nimval.
      //Choose the one that will cost him most to keep
      //the nimval at zero.
      numBestMove = 0;

      
      
      
      
      
      bestCost = -9999;
      move = -1;
      for (i=0; i<moveValue.numPossibleMove; i++)
      {
        NIMCON nimcon;
        int nimval;
        nimcon = &pm[i].con;
        if (pm[i].offering > 0) continue; //Don't give things away to establish nimval.
        if (pm[i].nimval < 0)
        {
          int total, decline;
          if (pm[i].nimval == LOONY2)
          {
            total = pm[i].offering;
            decline = 2;
          }
          else
          {
            NotImplemented("Negative nimval");
            total=0;
            decline = 0;
            cost = 0;
          };
          // Here is the situation.  The nimvalue of the current
          // game is zero.  There is a loony move available to us.
          // If we make the loony move, what must our oppponent do
          // to restore the nimvalue to zero?
          // 1) Take them all.  Make the nimvalue non-zero. then move again.
          // 2) Decline some, force us to take the rest and reduce the 
          //    nimvalue to zero.
          // We can decide which is the case by determining the nimvalue
          // of the game without any of the loony-move chain.
          nimval = nimcon.NimVal(&pm[i].con);
          if (nimval != 0)
          {
            //It appears that if he takes them all then he will
            //get to move again from a non-zero game and thus
            //reduce the value to zero with his follow-up move.
            //So the cost to him is negative total chain length.
            cost = -total + nimcon.MaxCostOfNimZero(-1, 1);
          }
          else
          {
            //It appears that if our opponent taks all the long
            //chain which we offered, it will result in a zero valued
            //game.  So he dares not take them all.  He will decline 
            //a few.
            cost = 2*decline - total;
          };
        }
        else
        { //nimval >= 0
          //nimcon.Simplify();
          nimcon.Canon(0, nimcon.m_numNode);
          cost = -pm[i].offering + nimcon.MaxCostOfNimZero(-1, 1); //Find the largest response.
        };
        if (cost > bestCost) numBestMove = 0;
        if (cost >= bestCost)
        {
          bestCost = cost;
          bestMoves[numBestMove++] = (ui8)i;
        };
      };
      if (!searchAborted && (numBestMove != 0))
      {
        //
        {
          //Choose among the equally good 'bestMoves'.that result
          //in non-zero nimval.  Choose the one that will provide
          //our opponent with the most possible moves that will
          //leave the nimval non-zero.  The 'Enough Rope' principle.
          int rope[100];
          for (i=0; i<numBestMove; i++)
          {
            rope[i] = pm[bestMoves[i]].con.MeasureRope();
          };
          {
            int n, maxRope, numMaxRope;
            maxRope = -1;
            numMaxRope = 0;
            for (i=0; i<numBestMove; i++)
            {
              if (rope[i] > maxRope)
              {
                maxRope = rope[i];
                numMaxRope = 0;
              };
              if (rope[i] == maxRope)
              {
                numMaxRope++;
              };
            };
            n = pDoc->MyRandom(numMaxRope);
            for (i=0; n>=0; i++)
            {
              if (rope[i] == maxRope) n--;
            };
          };
          computerMove = pm[bestMoves[i-1]];
          computerMove.con.Canon(0, computerMove.con.m_numNode);
          moveType = MT_NONIMVAL;
        };
      };
      
      
      
      
      
      
      //move = MaxCostOfNimZero(pm, moveValue.numPossibleMove, +1, 1); //Find the largest
    };
  };
#ifndef HASH_CARRY
  delete swHashTable;
  swHashTable = NULL;
  delete nimHashTable;
  nimHashTable = NULL;
#endif
  free (pm);
  CLOSEDUMP
}


void CON::TheCCD55Case(DOTS_AND_BOXES * /*pGame*/)
{
//  SWVALUE moveValue;
}

void CON::SimpleWin(DOTS_AND_BOXES *pGame)
{
  int width, height;
  int row, col;
  SWVALUE moveValue;
  PossibleMove *pm;
  moveValue.numPossibleMove = 0;
  availableMoveStack = MOVE_STACK_SIZE;
  pm = (PossibleMove *)malloc(availableMoveStack*sizeof(PossibleMove));
  if (pm == NULL)
  {
    ImplementationError("Cannot allocate memory");
  };
//  int size = MAX_NODES*4*(sizeof(PossibleMove));
  ui computerScore, humanScore;
  width = pGame->Width();
  height = pGame->Height();
  computerScore = humanScore = 0;
  for (row=0; row<height-1; row++)
  {
    for (col=0; col<width-1; col++)
    {
      if (pGame->BoxOwner(col, row) == OWNER_COMPUTER) computerScore++;
      else if (pGame->BoxOwner(col, row) == OWNER_HUMAN) humanScore++;
    };
  };
  m_coinsRemaining = (width-1)*(height-1);
  m_neededToWin = m_coinsRemaining/2 + 1;
  m_neededToTie = (m_coinsRemaining+1)/2;
  m_coinsRemaining -= computerScore + humanScore;
  //if ((height==5) && (width==5))
  ////{
  //  TheCCD55Case(pGame);
  //}
  //else
  {
    TheGeneralCase(width, height, computerScore, humanScore, pm);
  };
}



void OPENINGp::Play(DOTS_AND_BOXES *pGame)
{
  CCDp ccdp(5,5);;
  ui64 move, originalPos, newPos;
  int i, j, k, line, numLine;
  ui8 winMoves[64], tieMoves[64];
  int numWinMove = 0, numTieMove = 0;
  numLine = pGame->NumberOfLines();
  ccdp = pGame;
  originalPos = ccdp.m_pos;
  ccdp.Canon();
  //Try every possible move except those that offer
  //our opponent a box.
  for (line=0,move=1; line<40; move<<=1,line++)
  {
    if ((ccdp.m_pos & move) != 0) continue; // That line is already taken!
    if (ccdp.IsCapture(line))
    {
      j=2; //Search Straits
    }
    else 
    {
      j = 0; //Search Islands
    };
    for (k=j; k<j+2; k++)
    {
      for (i=0; i<NumPositions(k); i++)
      {
        if (ccdp.GoodOpening(Position(k, i), move))
        {
          if (k==j) //if winning play
          {
            winMoves[numWinMove++] = (ui8)line;
          }
          else
          {
            tieMoves[numTieMove++] = (ui8)line;
          };
        };
      };
    };
  };
  if (numWinMove > 0)
  {
    moveType = MT_OPENINGWIN;
    computerMove.lineNum = winMoves[pDoc->MyRandom(numWinMove)];
  }
  else if (numTieMove > 0)
  {
    moveType = MT_OPENINGTIE;
    computerMove.lineNum = tieMoves[pDoc->MyRandom(numTieMove)];
  }
  else
  {
    return;
  };
  //Now our problem is to determine what line in the original
  //problem is the same line as the line in the canonical problem.
  //
  // First create the new resulting position after the move.
  ccdp.m_pos = ccdp.m_pos | (1I64<<computerMove.lineNum);
  ccdp.Canon();
  newPos = ccdp.m_pos;
  //Then apply all possible moves to the original until
  //we get a match.
  for (line=0, move=1; line<40; line++, move<<=1)
  {
    if (originalPos & move) continue; 
    ccdp.m_pos = originalPos | move;
    ccdp.Canon();
    if (ccdp.m_pos == newPos)
    {
      computerMove.lineNum = line;
      return;
    };
  };
  die("Internal Error");
}


void PlayOpening(DOTS_AND_BOXES *pGame)
{
  if (pGame->IsSpecial5x5())
  {
    OPENINGp opening(5, 5, pGame->NumberOfLines());
    opening.Play(pGame);
  }
  else
  {
    return;
  };
}


UINT ComputerMoveProc(LPVOID param)
{
  DOTS_AND_BOXES *pGame;
  CON con;//  STRING cos;
  pGame = (DOTS_AND_BOXES *)param;
  moveType = MT_NONE;
  PlayOpening(pGame);
  if (moveType != MT_NONE)
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };
  con = *pGame;
  con.ObviousCapture();
  // Stop right here if there is an obvious capture move
  if (moveType == MT_OBVIOUSCAPTURE)
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };

  // *** If we can find a simple win in the time allowed
  // *** then we are finished.
  con.Canon(0,con.m_numNode);
  con.SimpleWin(pGame);
  if (    (moveType == MT_SIMPLEWIN)
       || (moveType == MT_SIMPLETIE)
       || (moveType == MT_SIMPLEWINDECLINE)
       || (moveType == MT_SIMPLEWINCAPTURE) 
       || (moveType == MT_MAXCAPTURE)
       || (moveType == MT_MAXDECLINE)
       || (moveType == MT_NIMVALCAPTURE)
       || (moveType == MT_NIMVALDECLINE)
     )
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };

  
  
  //A Simple win is impossible.
  //Let us see if a possible move results 
  //in a numval of zero.
  if (moveType == MT_NIMVAL)
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };

  //Well, perhaps we can find a move that will cost him
  //something in order to maintain a nimval of zero.
  if (moveType == MT_NONIMVAL)
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };
  
  //Taking anything is better than a random move.
  con.GrabAnything();
  if (moveType == MT_GRABANYTHING)
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };

  
  //Giving nothing is better than a random move.
  pGame->GiveNothing();
  if (    (moveType == MT_GIVENOTHING)
       || (moveType == MT_BREAKLOOPS)
       || (moveType == MT_LOOPDIVIDE)
       || (moveType == MT_DIVIDE) 
     )       
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };

  
  
  con.GiveLittle();
  if (moveType == MT_GIVELITTLE)
  {
    thinking = false;
    PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
    return 0;
  };

  ComputerMoveRandom(pGame);
  thinking = false;
  PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);
  return 0;
}







UINT ComputeNimvalProc(LPVOID param)
{
  DOTS_AND_BOXES *pGame;
  CON con;//  STRING cos;
  pGame = (DOTS_AND_BOXES *)param;
  moveType = MT_NONE;
  con = *pGame;

	NIMCON nimcon;
  CString msg;
  NIMHASHTABLE *origNimHashTable;
  int nimval;
  origNimHashTable = nimHashTable;
  nimHashTable = new NIMHASHTABLE;
  nimval = nimcon.NimVal(&con);
  if (nimHashTable != NULL) delete nimHashTable;
  nimHashTable = origNimHashTable;
  msg.Format("Nimval = %d", nimval);
  MessageBox(NULL, msg, "Result", MB_OK);
  
  
  
  thinking = false;  
  PostMessage(windowHandle, WM_COMMAND, WM_THREADCOMPLETION, 0);

  return 0;
}










void PROCESS_COMPUTER_MOVE::StartThread(void)
{
  FILETIME fileTime;
  while (thinking)
  {
    stopThinking = true;
    Sleep(100);
  };
  stopThinking = false;
  thinking = true;
  moveType = MT_NONE;
  GetSystemTimeAsFileTime(&fileTime);
  timeLimit = ((i64)fileTime.dwHighDateTime<<32)|fileTime.dwLowDateTime;
  currentTime = timeLimit;
  timeLimit += 10000000 * (_int64)pDoc->TimeLimit(); // five second
  searchAborted = false;
  AfxBeginThread(ComputerMoveProc, (LPVOID)m_pGame);
}

PROCESS_COMPUTER_MOVE::PROCESS_COMPUTER_MOVE(DOTS_AND_BOXES *pGame)
{
  m_pGame = pGame;
}


STATUS PROCESS_COMPUTER_MOVE::OnInitialize(void)
{
  if (pDoc->Game().IsFinished())
  {
    PopProcess();
    return STATUS_DONE;
  };
 // m_title="Thinking...Press 'Esc' to make me quit trying";
  StartThread();
  return STATUS_OK;
}


STATUS PROCESS_COMPUTER_MOVE::OnChar(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{

  if (nChar == 0x1b)
  {
    while (thinking)
    {
      stopThinking = true;
      Sleep(100);
    };
    //PopProcess();
    return STATUS_OK;
  };
  return STATUS_OK;
}

STATUS PROCESS_COMPUTER_MOVE::OnShutdown(void)
{
  while (thinking)
  {
    stopThinking = true;
    Sleep(100);
  };
  return STATUS_DONE;
}

STATUS PROCESS_COMPUTER_MOVE::OnTimer(UINT /*nIDEvent*/)
{
  FILETIME fileTime;
  i64 iTime;
  if (!thinking) 
  {
    return STATUS_DONE;
  };
  GetSystemTimeAsFileTime(& fileTime);
  iTime = ((i64)fileTime.dwHighDateTime << 32) | fileTime.dwLowDateTime;
  currentTime = iTime;
  if (iTime > timeLimit) stopThinking = true;
  return STATUS_OK;
}

STATUS PROCESS_COMPUTER_MOVE::OnThreadCompletion(void)
{
  if (moveType == MT_NONE)
  {
    PopProcess();
    return STATUS_DONE;
  };
  m_move = TranslateComputerMove(m_pGame, moveType, &computerMove.con);
  m_move.oldLineType = LINE_NONE;
  m_move.newLineType = LINE_COMPUTER;
  m_move.moveType = moveType;
  m_move.FindCaptures(&pDoc->Game());
  PushProcess(new PROCESS_FLASH(m_move));
  pDoc->AddMove(m_move);
  return STATUS_OK;
}

STATUS PROCESS_COMPUTER_MOVE::OnReturn(PROCESS *pProcess)
{
  delete pProcess;
  if (    ((m_move.boxesCaptured[0].x >= 0) || (m_move.boxesCaptured[1].x >= 0))
       && pDoc->IsMoveAvailable())
  {
    StartThread();
    return STATUS_OK;
  }
  else
  {
    return STATUS_DONE;
  };
}






















void PROCESS_NIMVALUE::StartThread(void)
{
  while (thinking)
  {
    stopThinking = true;
    Sleep(100);
  };
  stopThinking = false;
  thinking = true;
  searchAborted = false;
  timeLimit = 0x7fffffffffffffffI64;
  AfxBeginThread(ComputeNimvalProc, (LPVOID)m_pGame,0,80*sizeof(NIMCON));
}

PROCESS_NIMVALUE::PROCESS_NIMVALUE(DOTS_AND_BOXES *pGame)
{
  m_pGame = pGame;
}


STATUS PROCESS_NIMVALUE::OnInitialize(void)
{
 // m_title="Computing Nimval...Press 'Esc' to make me quit trying";
  StartThread();
  return STATUS_OK;
}


STATUS PROCESS_NIMVALUE::OnChar(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{

  if (nChar == 0x1b)
  {
    //if (thinking) MessageBox(NULL,"Received esc-thinking","",MB_OK);
    //else MessageBox(NULL,"Received esc-Not thinking","",MB_OK);
    while (thinking)
    {
      stopThinking = true;
      Sleep(0);
    };
    //PopProcess();
    return STATUS_OK;
  };
  return STATUS_OK;
}

STATUS PROCESS_NIMVALUE::OnShutdown(void)
{
  while (thinking)
  {
    stopThinking = true;
    Sleep(100);
  };
  return STATUS_DONE;
}

STATUS PROCESS_NIMVALUE::OnThreadCompletion(void)
{
  PopProcess();
  return STATUS_DONE;
}

STATUS PROCESS_NIMVALUE::OnReturn(PROCESS *pProcess)
{
  delete pProcess;
  return STATUS_DONE;
}





















PROCESS_FLASH::PROCESS_FLASH(const MOVE& move)
{
  m_move = move;
  m_lineType = m_move.newLineType;
  m_ownerType = (m_lineType==LINE_HUMAN)?OWNER_HUMAN:OWNER_COMPUTER;
}

//PROCESS_FLASH_VERTICAL::PROCESS_FLASH_VERTICAL(const MOVE& move)
//{
//  VerticalCapture(move.dotNum, m_boxesCaptured);
//  m_dotNum = move.dotNum;
//  m_lineType = move.lt;
//  m_ownerType = (m_lineType==LINE_HUMAN)?OWNER_HUMAN:OWNER_COMPUTER;
//}

STATUS PROCESS_FLASH::OnInitialize(void)
{
  m_flashCount = pDoc->MoveFlashCount();
#ifdef DISPLAYINTERNALS
  //m_title.Format("Making a move type %s", m_move.TypeName());
#endif
  return STATUS_OK;
}

//STATUS PROCESS_FLASH_VERTICAL::OnInitialize(void)
//{
//  m_flashCount = pDoc->MoveFlashCount();
//  m_title = "Making a move";
//  return STATUS_OK;
//}

STATUS PROCESS_FLASH::OnTimer(UINT /*event*/)
{
  if (m_flashCount == 0)
  {
    if (m_move.dotNum.x >= 0)
    {
      if (m_move.vertical)
      {
        pDoc->VerticalLineType(m_move.dotNum, m_move.newLineType);
      }
      else
      {
        pDoc->HorizontalLineType(m_move.dotNum, m_move.newLineType);
      };
    };
    if (m_move.boxesCaptured[0].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[0], m_move.newOwnerType[0]);
    if (m_move.boxesCaptured[1].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[1], m_move.newOwnerType[1]);
    INVALIDATE
    PopProcess();
    return STATUS_DONE;
  };
  m_flashCount--;
  if (m_move.dotNum.x >= 0)
  {
    if (m_move.vertical)
    {
      if (m_flashCount & 2)
      {
        pDoc->VerticalLineType(m_move.dotNum, m_move.oldLineType);
      }
      else
      {
        pDoc->VerticalLineType(m_move.dotNum, m_move.newLineType);
      };
    }
    else
    {
      if (m_flashCount & 2)
      {
        pDoc->HorizontalLineType(m_move.dotNum, m_move.oldLineType);
      }
      else
      {
        pDoc->HorizontalLineType(m_move.dotNum, m_move.newLineType);
      };
    };
  };
  if (m_flashCount & 2)
  {
    if (m_move.boxesCaptured[0].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[0], m_move.oldOwnerType[0]);
    if (m_move.boxesCaptured[1].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[1], m_move.oldOwnerType[1]);
  }
  else
  {
    if (m_move.boxesCaptured[0].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[0], m_move.newOwnerType[0]);
    if (m_move.boxesCaptured[1].x >= 0) pDoc->BoxOwner(m_move.boxesCaptured[1], m_move.newOwnerType[1]);
  };
  pViewWindow->Invalidate(false); //Don't clear background.
  return STATUS_OK;
}


STATUS  PROCESS_FLASH::OnChar(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
  if (nChar == 0x1b)
  {
    PopProcess();
    OnShutdown();
  }
  return STATUS_OK;
}


STATUS PROCESS_FLASH::OnShutdown()
{
  pDoc->VerticalLineType(m_move.dotNum, m_move.newLineType);
  pDoc->UpdateAllViews(NULL);
  return STATUS_DONE;
}


CString TestError(const MOVE& move)
{
  CString result;
  result.Format("Test Error\nComputer move to dot (%d,%d)%s\nMove Type = %s",
                   move.dotNum.x, move.dotNum.y,
                   move.vertical?"Vertical":"Horizontal",
                   move.TypeName());
  return result;
}

CString Test(void)
{
  // Make a copy of the current game.
  // Remove the computer moves.
  // Ask the computer to generate a move.
  // See if the generated move is an allowed move.
  // That's All, Folks!
  DOTS_AND_BOXES copyOfGame(pDoc->Game().Width(), pDoc->Game().Height());
  int row, col;
  MOVE move;
  CON con;
  copyOfGame = pDoc->Game();
  // First, remove all the computer lines from copyOfGame.
  for (row=0; row<copyOfGame.Height(); row++)
  {
    for (col=0; col<copyOfGame.Width(); col++)
    {
      if (col < copyOfGame.Width()-1)
      {
        if (copyOfGame.HLineType(col, row) == LINE_COMPUTER)
        {
          copyOfGame.SetHLineType(col, row, LINE_NONE);
        };
      };
      if (row < copyOfGame.Height()-1)
      {
        if (copyOfGame.VLineType(col, row) == LINE_COMPUTER)
        {
          copyOfGame.SetVLineType(col, row, LINE_NONE);
        };
      };
    };
  };
  //Remove the owner from any boxes that are no longer captured.
  for (row=0; row<copyOfGame.Height()-1; row++)
  {
    for (col=0; col<copyOfGame.Width()-1; col++)
    {
      if (copyOfGame.BoxOwner(col,row) != OWNER_NONE)
      {
        if (   (copyOfGame.HLineType(col,row) == LINE_NONE)
            || (copyOfGame.VLineType(col,row) == LINE_NONE)
            || (copyOfGame.HLineType(col,row+1) == LINE_NONE)
            || (copyOfGame.VLineType(col+1,row) == LINE_NONE))
        {
          copyOfGame.SetBoxOwner(col, row, OWNER_NONE);
        };
      };
    };
  };
//  computerMove = -1;
  if (pDoc->AI_Option(MT_RANDOM)) ComputerMoveRandom(&copyOfGame);
  con = copyOfGame;
#if (defined NS_DUMP) || (defined SW_DUMP)
  con.DUMP("After translation",0,0,0,0,con.m_numNode);
#endif
  if (pDoc->AI_Option(MT_OBVIOUSCAPTURE)) con.ObviousCapture();
  if (moveType != MT_OBVIOUSCAPTURE)
  {
    if (pDoc->AI_Option(MT_GRABANYTHING)) con.GrabAnything();
    if (moveType != MT_GRABANYTHING)
    {
      if (pDoc->AI_Option(MT_GIVENOTHING)) copyOfGame.GiveNothing();
      if (    (moveType != MT_GIVENOTHING)
           && (moveType != MT_BREAKLOOPS)
           && (moveType != MT_LOOPDIVIDE)
           && (moveType != MT_DIVIDE)
         )
      {
        if (pDoc->AI_Option(MT_GIVELITTLE)) con.GiveLittle();
      };
    };
    CON savecon;
    savecon = con;
    con.Canon(0,con.m_numNode);
    //for (int k=0;k<100;k++)
    if (   pDoc->AI_Option(MT_SIMPLEWIN)
         ||pDoc->AI_Option(MT_NIMVAL)) con.SimpleWin(&copyOfGame);
  };
  if (moveType != MT_NONE)
  {
    computerMove.con.Canon(0, computerMove.con.m_numNode);
    move = TranslateComputerMove(&copyOfGame, moveType, &computerMove.con);
    if (move.vertical)
    {
      if (pDoc->VerticalLineType(move.dotNum) != LINE_COMPUTER) return TestError(move);
    }
    else
    {
      if (pDoc->HorizontalLineType(move.dotNum) != LINE_COMPUTER) return TestError(move);
    };
  }
  else
  { //No move was made.  Make sure no moves were expected.
    for (row=0; row<copyOfGame.Height(); row++)
    {
      for (col=0; col<copyOfGame.Width(); col++)
      {
        if (col < copyOfGame.Width() - 1)
        {
          if (pDoc->HorizontalLineType(col, row) == LINE_COMPUTER)
          {
            return TestError(move);
          };
        };
        if (row < copyOfGame.Height() - 1)
        {
          if (pDoc->VerticalLineType(col, row) == LINE_COMPUTER)
          {
            return TestError(move);
          };
        };
      };
    };
  };
  if (pDoc->AI_Option(MT_VERIFYNIMSTRINGVALUE))
  {
    int v;
    NIMCON nimcon;
    //if (pDoc->Game().IsSpecial5x5())
    //{
    //  if (cdHashTable != NULL) delete cdHashTable;
    //  cdHashTable = new CDHASHTABLE;
    //  CDVALUE cdvalue;
    //  CCD55 ccd55;
    //  ccd55 = &pDoc->Game();
    //  cdvalue = ccd55.Value(0,0);
    //  v = cdvalue.player1Min - cdvalue.player2Min;
    //  delete cdHashTable;
    //  cdHashTable = NULL;
    //}
    //else
    {
      NIMHASHTABLE *origNimHashTable;
      origNimHashTable = nimHashTable;
      if (nimHashTable != NULL) delete nimHashTable;
      nimHashTable = new NIMHASHTABLE;
      v = nimcon.NimVal(&con);
      if (nimHashTable != NULL) delete nimHashTable;
      nimHashTable = origNimHashTable;
    };
    if (v != pDoc->NimstringValueToVerify())
    {
      CString err;
      err.Format("Expected Nimstring value=%d\nComputed value=%d",
        pDoc->NimstringValueToVerify(), v);
      return err;
    };
  };
  return CString("");
}
