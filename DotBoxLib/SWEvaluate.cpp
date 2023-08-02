#include "stdafx.h"
#include <malloc.h>
//#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"
//#include "NewBoxesDoc.h"
//#include "Process.h"
//#include "NewBoxesView.h"
//#include "Utility.h"

void Break(void);

ui32 MyRandom(ui32 n);

extern i64 timeLimit;
extern i64 currentTime;
extern bool searchAborted;
extern SWHASHTABLE *swHashTable;
extern bool stopThinking;
extern int enableDump;
extern int searchDepth;
extern PossibleMove computerMove;
extern int availableMoveStack;
extern MOVE_TYPE moveType;

#ifdef SW_DUMP
#define TRACERETURN(x,y)                   \
  TraceReturn(x,y);searchDepth--;
#else
#define TRACERETURN(x,y)
#endif

#ifdef SW_DUMP
#define BREAKIF(expr) if(expr)Break();
#else
#define BREAKIF(expr)
#endif

#ifdef SW_DUMP
#define LOGUIUI(format,i1,i2) LogUiUi(format,i1,i2);
#else
#define LOGUIUI(format,i1,i2)
#endif



#define __Neighbor(x,i) \
  (m_jointInfo[(unsigned)m_links[LS*(x)+(i)]/JS].groupNum)

#define __CompareNeighbor(x,y,i) \
  ((__Neighbor((x),(i))) == (__Neighbor((y),(i))))

#define __CompareNeighborAB(x,y) \
  (  (__CompareNeighbor((x),(y),0)) && (__CompareNeighbor((x),(y),1))    )

#define __Compare2Node(x,y) \
  ((m_links[LS*(x)+2] == m_links[LS*(y)+2]) ? (__CompareNeighborAB((x),(y))) : false)

#define __This2Node(x,y) \
  ((m_links[LS*(y)]>=0) ? (__Compare2Node((x),(y))) : (false))

#define __CompareGroundedNodes(x,y) \
  ((m_links[LS*(x)+2]==m_links[LS*(y)+2])?(__CompareNeighbor((x),(y),1)):(false))

#define __CompareGroundNode(x,y) \
  ((m_links[LS*(y)+1] >= 0)?(__CompareGroundedNodes((x),(y))):(false))

//link[x] links GROUND to node
#define __CompareGroundedNode(x,y) \
  ( (m_links[LS*(y)]==GROUND) ? (__CompareGroundNode((x),(y))) : (false))  

//link[x] is an isolated chain.
#define __CompareIsolatedChain(x) \
  ( (chainCount++)!=0) )

#define __CompareGround(x,y) \
  ((m_links[LS*(x)+1] == GROUND)?(__CompareIsolatedChain((x),(y))):(__CompareGroundNode((x),(y))))

//link[x] has GROUND on left
#define __ThisIsGround(x,y) \
  ( (m_links[LS*(x)+1]==GROUND) ? (__CompareIsolatedChain((x)) : (__CompareGroundedNode((x),(y))) )

#define __ThisIsLoop(x,y) \
  ( (m_links[LS*(y)]==LOOP) ? ((loopCount++)!=0) : (false) )

//link[x] has left stop of GROUND or LOOP
#define __ThisNot2Node(x,y) \
  ((m_links[LS*(x)] == GROUND) ? (__ThisIsGround((x),(y))) : (__ThisIsLoop((x),(y))))


//Test if link[x] is equivalent to link[y]
#define LINKS_EQUIV(x,y) \
  (m_links[LS*(x)]>=0) ? (__This2Node((x),(y))) : (__ThisNot2Node((x),(y))) 


#define SAVEPOS                                                \
    _SavePos(saveArea);

#define RESTOREPOS _RestorePos(saveArea);

struct GOODMOVE
{
  i16 stringNum;
  i16 linkNum;
  i16 offering; //#coins to be lost
};


void CON::Capture(int nnA, int edge)
{
  if (sd[nnA][0].ns == DE_DE)
  {
    computerMove.con = *this;
    if (ed[nnA].e[0].el == 2)
    {
      computerMove.con.DeleteNode(nd[nnA].os);
      computerMove.con.m_coinsRemaining -= 2;
    }
    else
    {
      computerMove.con.ed[nnA].e[0].el--;
      computerMove.con.m_coinsRemaining -= 1;
    };
    computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
    computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
    moveType = MT_OBVIOUSCAPTURE;
  }
  else if (sd[nnA][0].ns == GND_DE)
  {
    computerMove.con = *this;
    if (ed[nnA].e[0].el == 1)
    {
      computerMove.con.DeleteNode(nd[nnA].os);
      computerMove.con.m_coinsRemaining -= 1;
    }
    else
    {
      computerMove.con.ed[nnA].e[0].el--;
      computerMove.con.m_coinsRemaining -= 1;
    };
    computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
    computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
    moveType = MT_OBVIOUSCAPTURE;
    return;
  }
  else if (sd[nnA][edge].ns == DEADEND)
  {
    computerMove.con = *this;
    moveType = MT_OBVIOUSCAPTURE;
    if (ed[nnA].e[edge].el == 1)
    {
      computerMove.con.DeleteEdge(nd[nnA].os, edge);
      computerMove.con.m_coinsRemaining -= 1;
    }
    else
    {
      computerMove.con.ed[nnA].e[edge].el--;
      computerMove.con.m_coinsRemaining -= 1;
    };
    computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
    computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
    return;
#endif
  }
  else
  {
    ImplementationError("Unknown capture type");
  };
}


void CON::ObviousCapture(void)
{
  // Grab a terminal coin at the end of a
  // chain unless the chain is length one.
  int nn, nnA, en;
  int nn2=-1, nn4=-1, en2=-1;
  int capture1nnA, capture2nnA;
  int capture1len, capture2len;
  int capture1edge, capture2edge;
  int deCount = 0;
  //If there are two chains with capturable coins then
  //we will certainly capture something.
  capture1nnA = capture1len = capture1edge = -1;
  capture2nnA = capture2len = capture2edge = -1;
  for (nn=0; nn<m_numNode; nn++)
  {
    nnA = nd[nn].ix;
    if (sd[nnA][0].ns == DE_DE)
    {
      capture2nnA = capture1nnA;
      capture1nnA = nnA;
      capture2len = capture1len;
      capture1len = ed[nnA].e[0].el;
    }
    else if (sd[nnA][0].ns == GND_DE)
    {
      capture2nnA = capture1nnA;
      capture1nnA = nnA;
      capture2len = capture1len;
      capture1len = ed[nnA].e[0].el;
    }
    else if (   (sd[nnA][0].ns == GND_GND)
             || (sd[nnA][0].ns == LOOP)
            )
    {
      continue;
    }
    else
    {
      for (en=0; en<nd[nnA].ec; en++)
      {
        if (sd[nnA][en].ns == DEADEND)
        {
          capture2nnA  = capture1nnA;
          capture2edge = capture1edge;
          capture2len  = capture1len;
          capture1nnA  = nnA;
          capture1edge = en;
          capture1len  = ed[nnA].e[en].el;
        };
      };
    };
  };
  if (capture2nnA != -1)
  {
    //There are two capturable chains.  We need to capture
    //something!!!
    if (capture2len < capture1len)
    {
      int temp;
      temp = capture1len;
      capture1len = capture2len;
      capture2len = temp;
  
      temp = capture1nnA;
      capture1nnA = capture2nnA;
      capture2nnA = temp;
  
      temp = capture1edge;
      capture1edge = capture2edge;
      capture2edge = temp;
    };
    //The shorter one comes first.
    if (sd[capture1nnA][0].ns != DE_DE)
    {
      if (capture1len == 1)
      {
        Capture(capture1nnA, capture1edge);
      }
      else
      {
        Capture(capture2nnA, capture2edge);
      };
    }
    else
    {
      Capture(capture1nnA, 0);
    };
    return;
  };        
  //There are not two capturable chains.
  for (nn=0; nn<m_numNode; nn++)
  {
    nnA = nd[nn].ix;
    if (sd[nnA][0].ns == DE_DE)
    {
      //The chain has capturable coins on both
      //ends. If the chain is four long then further
      //analysis is necessary.   Otherwise take!
      if ((ed[nnA].e[0].el != 4) || (m_numNode == 1))
      {
        Capture(nnA, 0);
        return;
      }
      else
      {
        nn4 = nnA;
        deCount++;
      };
    } 
    else if (sd[nnA][0].ns == GND_DE)
    {
      //The chain has a DEADEND at one end and a ground at 
      //the other end.  Unless it is two coins long, take it.
      if ((ed[nnA].e[0].el != 2) || (m_numNode == 1))
      {
        Capture(nnA, 0);
        return;
      }
      else
      {
        nn2 = nnA;
        en2 = 0;
        deCount++;
      };
    }
    else
    {
      //Look for a DEADEND attached to a node.
      for (en=0; en<4; en++)
      {
        if (sd[nnA][en].ns == DEADEND)
        {
          if (ed[nnA].e[en].el != 2)
          {
            Capture(nnA, en);
            return;
          }
          else
          {
            nn2 = nnA;
            en2 = en;
            deCount++;
          };
        };
      };
    };
  };
  if (deCount < 2) return;
  computerMove.con = *this;
  moveType = MT_OBVIOUSCAPTURE;
  if (nn4 >= 0)
  {
    computerMove.con.ed[nn4].e[0].el--;
    computerMove.con.m_coinsRemaining -= 1;
  }
  else
  {
    computerMove.con.ed[nn2].e[en2].el--;
    computerMove.con.m_coinsRemaining -= 1;
  };
}




//A function we call when we have to decide how to 
//handle a long chain.

SWVALUE CON::MYCHOICE(ui ourScore, ui hisScore, 
                  ui total, ui decline,
                  PossibleMove *pm)
{
  ui ourNewScoreV, ourNewScoreW;
  ui ourNewMinV, ourNewMinW, hisNewMinV, hisNewMinW;
  int nimvalTake;
  SWVALUE v, w;
  SWVALUE result;
  ASSERT(total>=2);
  ASSERT(total>=decline);
  NIMCON nimcon;
  nimvalTake = nimcon.NimVal(this);
  if (nimvalTake == 99)
  {
    //We did not have time to compute the nimstring value.
    //Therefore it is unlikely that we can compute a winning move.
    //Since we are completely in the dark, it seems best to
    //simply take all that we can.
    result.moveType = MT_SIMPLEWINCAPTURE;
    computerMove.con = *this;
    if (computerMove.con.m_numNode <= SW_MAX_CANON)
           computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
    computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
    return result;
  };
  m_coinsRemaining -= total; //One of us will take them!
//  Note("MYCHOICE A");
  v = SWEvaluateMove(ourScore+(total), hisScore, pm+1);//We take them all and move again.
  if (searchAborted)
  {
    //Time expired before we could find a winning move.
    //We will make our decision based on the nimstring
    //value.
    computerMove.con = *this;
    if (computerMove.con.m_numNode <= SW_MAX_CANON)
            computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
    computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
    if (nimvalTake == 0)
    {
      result.moveType = MT_NIMVALDECLINE;
    }
    else
    {
      result.moveType = MT_NIMVALCAPTURE;
    };
    m_coinsRemaining += total;
    return result;
  };
  ourNewMinV = (total) + v.player1Min;
  ourNewScoreV = ourScore + ourNewMinV;
  if (ourNewScoreV >= m_neededToWin) 
  { 
    //Taking them all won the game for us
    //so we won't try anything else.
    computerMove.con = *this;
    m_coinsRemaining += total;
    //computerMove.offering = total;
    result.moveType =  MT_SIMPLEWINCAPTURE; //Capture all coin
  }
  else
  {
    hisNewMinV = v.player2Min;
    //Well, we did not win by taking them all.  We will try declining
    //a few coins and force him to move.
  //  Note("MYCHOICE B");
    w = SWEvaluateMove(hisScore+(decline), ourScore+(total)-(decline), pm+1);
    if (searchAborted)
    {
      //Time expired before we could find a winning move.
      //We will make our decision based on the nimstring
      //value.
      computerMove.con = *this;
      if (computerMove.con.m_numNode <= SW_MAX_CANON)
              computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
      computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
      if (nimvalTake == 0)
      {
        result.moveType = MT_NIMVALDECLINE;
      }
      else
      {
        result.moveType = MT_NIMVALCAPTURE;
      };
      m_coinsRemaining += total;
      return result;
    };
    ourNewMinW = (ui16)((total) - (decline) + v.player2Min);
    ourNewScoreW = ourScore + ourNewMinW;
    if (ourNewScoreW >= m_neededToWin)
    {
      //A hard-hearted handout won the game for us.
      //We can use this move.
      ui ourNewMinW;
      hisNewMinW = (decline) + w.player1Min;
      ourNewMinW = (total) - (decline) + w.player2Min;
      computerMove.con = *this;
      m_coinsRemaining += total;
      result.moveType = MT_SIMPLEWINDECLINE; //No Capture.
    }
    else
    {
      // Neither of our options won the game for us.  So
      // we will take the option that will maximize our score.
      if (ourNewScoreV >= ourNewScoreW)
      {
        //We will choose to take the whole string and move again
        computerMove.con = *this;
        result.moveType = MT_MAXCAPTURE;
      }
      else
      {
        // We will decline the coins and make him move next.
        computerMove.con = *this;
        result.moveType = MT_MAXDECLINE;
      };
      m_coinsRemaining += total;
    };
  };
  return result;
}

#ifdef _DEBUG
#define CHECK_MOVE_STACK                      \
  availableMoveStack--;                       \
  if (availableMoveStack == 0)                \
  {                                           \
    ImplementationError("MoveStackExceeded"); \
  };
#else
#define CHECK_MOVE_STACK
#endif

#define COPYPOS                               \
{                                             \
  cpm++;                                      \
  cpm->con = *this;                           \
  cpm->nimval = 0;                          \
  CHECK_MOVE_STACK                            \
};



SWVALUE CON::CheckSWCapture(ui player1Score, ui player2Score, PossibleMove *pm) const
{
  SWVALUE result;
  int nn;
  PossibleMove *cpm = pm-1;
#ifdef SW_DUMP
  static int id=0;
  int myId = ++id;
//  LOGUIUI("SWEvaluateAllMoves id=%d",myId,0);
  this->DUMP("CheckSWCapture",myId,player1Score,player2Score,0,m_numNode);
#endif
  for (nn=0; nn<m_numNode; nn++)
  {
    int nnA, en;
    nnA = nd[nn].ix;
    if (IS_ISOLATED(sd[nnA][0].ns))
    {
      if (sd[nnA][0].ns == GND_DE)
      {
        int len;
        len = ed[nnA].e[0].el;
        if (len!= 2)
        {//We should have found this easy capture before getting here!
          ImplementationError("Capturable chain not two long");
        };
        COPYPOS
        cpm->con.DeleteNode(nd[nnA].os);
        result = cpm->con.MYCHOICE(player1Score, player2Score, len, 2, pm);
        result.bestMove = 0;
        return result;
      }
      else if (sd[nnA][0].ns == GND_GND)
      {
        continue;
      }
      else if (sd[nnA][0].ns == DE_DE)
      {
        int len;
        len = ed[nnA].e[0].el;
        if (len!= 4)
        {//We should have found this easy capture before getting here!
          ImplementationError("Capturable chain not four long");
        };
        COPYPOS
        cpm->con.DeleteNode(nn);
        result = cpm->con.MYCHOICE(player1Score, player2Score, len, 4, pm);
        result.bestMove = 0;
        return result;
      }
      else if (sd[nnA][0].ns == LOOP)
      {
        continue;
      }
      else
      {
        NotImplemented("CheckSWCapture non-node");
      };
    };
    for (en=nd[nnA].ec-1; en>=0; en--)
    {
      if (sd[nnA][en].ns != DEADEND) continue;
      {
        int len;
        len = ed[nnA].e[en].el;
        if (len!= 2)
        {//We should have found this easy capture before getting here!
          ImplementationError("Capturable chain not two long");
        };
        COPYPOS
        cpm->con.DeleteEdge(nn, en);
        result = cpm->con.MYCHOICE(player1Score, player2Score, len, 2, pm);
        result.bestMove = 0;
        return result;
      }
    }; //for (en)
  }; //for (nn)
  result.moveType = MT_NONE;
  return result;
}


#ifdef _DEBUG
class MarkAvailableMoveStack
{
  int remember;
public:
  MarkAvailableMoveStack(void){remember = availableMoveStack;};
  ~MarkAvailableMoveStack(void){availableMoveStack = remember;};
};
#endif


SWVALUE CON::SWEvaluateAllMoves(ui ourScore, ui hisScore, PossibleMove *pm) const
{
  int nn, nnA;
  PossibleMove *cpm = pm-1;
  ui8 sortedMoves[MAX_NODES*4];
  int npm;
#ifdef _DEBUG
  MarkAvailableMoveStack mams;
#endif
#ifdef _CHECK
  CheckEverything(0, m_numNode, true);
#endif
#ifdef SW_DUMP
  static int id=0;
  int myId = ++id;
  if (myId == 459749)
  {
    int kkk=1;
  };
  LOGUIUI("SWEvaluateAllMoves id=%d",myId,0);
  this->DUMP("EvaluateAllMoves",myId,ourScore,hisScore,0,m_numNode);
//  enableDump = false;
#endif
//  int loopCount = 0, chainCount = 0; //Isolated loops and chains
//  LINKTYPE lt;
  SWVALUE result;
  result.player1Min = 0;
  result.player2Min = MAX_NODES + 1;
  result.bestMove = -1;
  result.numPossibleMove = 0;
  result.allNimvalComputed = true;
  {
  //There are generally no capturing moves.  
  //The previous mover would have removed them at 
  //less cost than having us search for them.
  //However, at startup the user may have presented
  //us with capturable coins.  If we find any
  //during our tour of the nodes then we quit right
  //there and quit trying any other moves.  Taking
  //them has to be the right thing to do.  If we
  //have any choice we will both try taking them all
  //and try the hard-hearted handout.
  // 
  //We go throungh the nodes and make list of all
  //possible positions after a single move.  We rate each
  //of the possibilities.  The rating is based on
  //two things:
  // 1 - The nimstring value - smaller is better.
  // 2 - The number of coins offered our opponent - smaller is better.
  // 3   provided both are short or both are long.
    for (nn=0; nn<m_numNode; nn++)
    {
      int en, ec, nnB;
      nnA = nd[nn].ix;
      if (IS_ISOLATED(sd[nnA][0].ns))
      {
        if (nn < m_numNode-1)
        {
          nnB = nd[nn+1].ix;
          if (sd[nnA][0].ns == sd[nnB][0].ns)
          {
            //The shorter one comes later.
            int longLen;
            if (sd[nnA][0].ns == GND_GND) longLen = 3;
            else if (sd[nnA][0].ns == LOOP) longLen = 4;
            else 
            {
              longLen = 0; //keep compiler quiet
              ImplementationError("Unknown isolated chain type");
            };
            if ((ed[nnA].e[0].el < longLen) || (ed[nnB].e[0].el >= longLen))
            {
              continue;
            };
          };
        };
        if (sd[nnA][0].ns == GND_GND)
        {
          int len;
          len = ed[nnA].e[0].el;
          if (len + hisScore >= m_neededToWin) 
          {
            if (len < result.player2Min) result.player2Min = (ui16)len;
            continue;
          };
          COPYPOS
          cpm->offering = len;
          if (len > 2) cpm->nimval = LOONY2;
          cpm->con.DeleteNode(nn);
          continue;
        }
        else if (sd[nnA][0].ns == LOOP)
        {
          int len;
          len = ed[nnA].e[0].el;
          if (len + hisScore >= m_neededToWin)
          {
            if (len < result.player2Min) result.player2Min = (ui16)len;
            continue;
          };
          COPYPOS
          cpm->offering = len;
          if (len > 3) cpm->nimval = LOONY4;
          cpm->con.DeleteNode(nn);
          continue;
        }
        else
        {
          NotImplemented("Unknown isolated chain");
        };
      };
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
            if (m_coinsRemaining < result.player2Min) 
                         result.player2Min = (ui16)m_coinsRemaining;
            continue; //I cannot see how this would help us
          };
        };
        if (len + hisScore >= m_neededToWin) 
        {
          if (len < result.player2Min) result.player2Min = (ui16)len;
          continue;
        };
        COPYPOS
        cpm->offering = len;
        if (len > 2) cpm->nimval = LOONY2;
        cpm->con.DeleteEdge(nn, en);
      }; //for each link	hisScore	0x0000000d

    };
  };
  npm = cpm - pm + 1;
  //if (npm == 0) 
  //{
  //  result.player2Min = 0;
  //};
  result.numPossibleMove = npm;
  {
    //Sort the possible moves.
    int n;
    for (n=0; n<npm; n++)
    {
      NIMCON nimcon;
      sortedMoves[n] = (ui8)n;
      if (pm[n].nimval == 0)
      {
        if (pm[n].con.m_numNode != 0)
        {
          pm[n].nimval = nimcon.NimVal(&pm[n].con);
          if (pm[n].nimval == 99)
          {
            result.numPossibleMove = n; //The only ones we have computed.
            result.player2Min = 0;
            result.allNimvalComputed = false;
            return result;
          };
        };
      };
    };
    //Now that we have the nimval and the offering for
    //each of the possible move, we will sort the possible
    //moves so that we try the best moves first.
    //A nimval of zero is best.  Then the lower offer is better.
    //Then bigger nimvals are better (the Enough Rope principle).
    {
      unsigned int ns, i, j;
      int k;
      ui8 t;
      for (ns=npm/2; ns>0; ns/=2)
      {
        for (i=0; i<ns; i++)
        {
          for (j=i; j<npm-ns; j+=ns)
          {
            if (pm[sortedMoves[j]] > pm[sortedMoves[j+ns]])
            {
              k=j;
              do
              {
                t=sortedMoves[k];
                sortedMoves[k] = sortedMoves[k+ns];
                sortedMoves[k+ns] = t;
                k-=ns;
              } while ((k>=0) && (pm[sortedMoves[k]] > pm[sortedMoves[k+ns]]));
            };
          };
        };
      };
    };
    // There! The moves are in order from smallest to largest.      
  }; //End of sort block.
  // 
  // Now we will go through the possible moves and see if any
  // of them will provide us with a win.
  for (int n=0; n<npm; n++)
  {
    int cpm;
    SWVALUE value;
    if (result.player1Min + ourScore >= m_neededToWin)
    {
      result.player2Min = 0; //Because we have not tried every possibility.
      if (result.bestMove == -1)
      {
        result.bestMove = MyRandom(npm);
// Moved to after the for loop
//        if (pm[result.bestMove].con.m_numNode <= SW_MAX_CANON)
//                pm[result.bestMove].con.Canon(0,pm[result.bestMove].con.m_numNode);
//#ifdef _CHECK
//        pm[result.bestMove].con.CheckEverything(0,pm[result.bestMove].con.m_numNode,false);
//#endif
      };
      break;
    };
    if (result.player1Min + ourScore >= m_neededToTie)
    {
      if (result.bestMove == -1)
      {
        result.bestMove = n;
      };
    };
    cpm = sortedMoves[n];
    if (pm[cpm].nimval >= 0)
    {
      //Let him make his move;
//      if (myId == 4)
//      {
//        int kkk=1;
//      };
      pm[cpm].con.m_coinsRemaining -= pm[cpm].offering;
      value = pm[cpm].con.SWEvaluateMove(hisScore+pm[cpm].offering, ourScore, pm+npm);
//      if (myId == 5)
//      {
//        int kkk=1;
//      };
      if (value.player2Min > result.player1Min)
      {
        result.player1Min = value.player2Min;
        result.bestMove = cpm;
      };
      if (value.player1Min + pm[cpm].offering < result.player2Min) 
      {
        result.player2Min = (ui16)(value.player1Min + pm[cpm].offering);
      };
    }
    else
    {
      //Our opponent has a choice.  Take them all or
      //decline some coins to retain control.
//      pm[cpm].con.HISCHOICE(ourScore, hisScore, 
//                            pm[cpm].offering, -pm[cpm].nimval, 
//                            value, pm+npm);





      //A function that we invoke after making a loony move.
      //There are a 'total' number of coins in the string.
      //Our opponent can take them all and move again or he
      //can leave 'decline' coins for us, forcing us to 
      //make the next move.
//      bool CON::HISCHOICE(ui ourScore, ui hisScore, 
//                          ui total, ui decline, 
//                          SWVALUE& result, PossibleMove *pm)
//      { //Return true if this move wins the game for us.
      SWVALUE v, w;
      ui hisNewScoreV, hisNewScoreW;
      ui hisNewMinV, hisNewMinW, ourNewMinV, ourNewMinW, ourNewMin;
      int total, decline;
      total = pm[cpm].offering;
      decline = -pm[cpm].nimval;
      ASSERT(total>2);
      ASSERT(decline <= total);
      //result.player1Min = 0;
      //result.player2Min = 0x7fff;
      pm[cpm].con.m_coinsRemaining -= total; //One of us will take them!
//      if (myId == 5)
//      {
//        int kkk=1;
//      };
      v = pm[cpm].con.SWEvaluateMove(hisScore+(total), ourScore, pm+npm);//He takes them all
      hisNewMinV = (total) + v.player1Min;
      hisNewScoreV = hisScore + hisNewMinV;
      if (hisNewScoreV >= m_neededToWin) 
      { 
        //Taking them all won the game for him
        //so he won't try anything else.
        if (hisNewMinV < result.player2Min) result.player2Min = (ui16)hisNewMinV;
        //ourNewMinV = v.player2Min;
        //if (result.player1Min < ourNewMinV) 
          //{
          //  result.bestMove = move;
          //};
          //Try another move
          //result.player1Min = 0; //We don't really know because he quit
          //                       //trying to reduce our score when he
          //                       //succeeded in winning.
      }
      else
      {
        //Well, he did not win by taking them all.  He will try declining
        //a few coins and force us to move.
        w = pm[cpm].con.SWEvaluateMove(ourScore+(decline), hisScore+(total)-(decline), pm+npm);
//        if (myId == 5)
//        {
//          int kkk=1;
//        };
        hisNewMinW = (total) - (decline) + w.player2Min;
        hisNewScoreW = hisScore + hisNewMinW;
        if (hisNewScoreW >= m_neededToWin)
        {
          //A hard-hearted handout won the game for him
          //We must continue looking for a better move.
          if (hisNewMinW < result.player2Min) result.player2Min = (ui16)hisNewMinW;
          ourNewMinW = w.player1Min + decline;
          if (ourNewMinW > result.player1Min) result.player1Min = (ui16)ourNewMinW;
          //Try another move
        }
        else
        {
          // Neither of his options won the game for him.  So we will assume
          // that he will take the option that will maximize his score.
          ourNewMinV = v.player2Min;
          ourNewMinW = decline + w.player1Min;
          ourNewMin = ourNewMinV;
          if (ourNewMinW < ourNewMin) ourNewMin = ourNewMinW;
          if (ourNewMin > result.player1Min)
          {
            result.player1Min = (ui16)ourNewMin;
            result.bestMove = cpm;
          };
          if (hisNewScoreV > hisNewScoreW)
          {
            //He will choose to take the whole string and move again
            if (hisNewMinV < result.player2Min) result.player2Min = (ui16)hisNewMinV;
//            if (ourScore + ourNewMinV >= m_neededToWin)
//            {
//              //result.player2Min = 0;
//              break;
//            };
          }
          else
          {
            // He will decline the coins and make us move next.
            if (hisNewMinW < result.player2Min) result.player2Min = (ui16)hisNewMinW;
//            if (ourScore + ourNewMinW >= m_neededToWin)
//            {
//              //result.player2Min = 0;
//            };
          };
          //This loony move did not result in a win for either of us.
          //Must be a tie!!!!
        };
      };




    };
  }; // For each move in the possible moves list.
  if (result.bestMove != -1)
  {
    if (pm[result.bestMove].con.m_numNode <= SW_MAX_CANON)
            pm[result.bestMove].con.Canon(0,pm[result.bestMove].con.m_numNode);
#ifdef _CHECK
    pm[result.bestMove].con.CheckEverything(0,pm[result.bestMove].con.m_numNode,false);
#endif
  };
  ASSERT(result.player2Min != MAX_NODES+1);
  return result;
}


#ifdef SW_DUMP
#define TRACEENTRY\
  searchDepth++;\
  BREAKIF(count==-1);\
  BREAKIF(count==-1);\
  this->DUMP("Enter SWEvaluateMove ======================", count,Player1Score, Player2Score,0,m_numNode);
#else
#define TRACEENTRY
#endif


#ifdef _CHECK
#define CHECK_EVERYTHING CheckEverything(0, m_numNode, true);
#else
#define CHECK_EVERYTHING
#endif


SWVALUE CON::SWEvaluateMove(ui Player1Score, ui Player2Score, PossibleMove *pm)
{
  static int count=0;
  int myCount;
  ui64 hash;
  int hashIndex;
  count++;
  myCount=count;  

//  if ((myCount >= 2590)&&(myCount<=5000))
//  {
//    enableDump = 1;
//  }
//  else enableDump = 0;
  if (m_numNode <= SW_MAX_CANON)
            Canon(0, m_numNode);
#ifdef _CHECK
  CheckEverything(0,m_numNode,false);
#endif
  TRACEENTRY
  SWVALUE moveValue;
  CHECK_EVERYTHING
//  CHECK_REMAINING
  //Note("a");
  if ((currentTime > timeLimit) || stopThinking) 
  {
    moveValue.player1Min = 0;
    moveValue.player2Min = 0;
    TRACERETURN(myCount, &moveValue)
    searchAborted = true;
    return moveValue;
  };
  if (m_numNode == 0)
  {
    moveValue.player1Min = 0;
    moveValue.player2Min = 0;
    TRACERETURN(myCount, &moveValue)
    return moveValue;
  };
  if (m_numNode == 1)
  {
    int nnA;
    nnA = nd[0].ix;
    if (IS_ISOLATED(sd[nnA][0].ns))
    {
      moveValue.player1Min = 0;
      moveValue.player2Min = ed[nnA].e[0].el;
      TRACERETURN(myCount, &moveValue);
      return moveValue;
    };
  };
  hash = SWHash();
  if (hash == 0xcce2814646c4308fI64)
  {
    enableDump = myCount;
//    this->DUMP("Enter SWEvaluateMove ======================", count,Player1Score, Player2Score,0,m_numNode);
  };
  LOGUIUI("Hash = 0x%08x%08x",(ui32)(hash>>32),(ui32)hash)
  hashIndex = swHashTable->Exists(hash);
  if (hashIndex >= 0)
  {
//    hashFinds++;
    swHashTable->Value(hashIndex, &moveValue);
    if (  (moveValue.player1Min + Player1Score >= m_neededToWin)
        ||(moveValue.player2Min + Player2Score >= m_neededToWin) 
        ||(moveValue.player1Min + moveValue.player2Min == (signed)m_coinsRemaining) )
    {
      TRACERETURN(myCount, &moveValue)
//      hashUses++;
      if (enableDump == myCount) 
      {
//        enableDump = 0;
      };
      return moveValue;
    };
  };
//  if (myCount == 0x41)
//  {
//    int kkk=1;
//  };
  moveValue = SWEvaluateAllMoves(Player1Score, Player2Score, pm);
  if (moveValue.player1Min+moveValue.player2Min > (signed)m_coinsRemaining)
  {
    ImplementationError("Capture more than all the coins");
  };
  if (    (Player1Score + moveValue.player1Min < m_neededToWin)
       && (Player2Score + moveValue.player2Min < m_neededToWin))
  {
    if (   (Player1Score + moveValue.player1Min != m_neededToWin-1)
        || (Player2Score + moveValue.player2Min != m_neededToWin-1))
    {
      if (!searchAborted) 
      {
        ImplementationError("Nobody won!");
      };
    };
  };
//  if (myCount == 7)
//  {
//    int kkk=1;
//  };
  if (hashIndex < 0) swHashTable->EnterValue(hash, &moveValue);
  else 
  {
    swHashTable->UpdateValue(hashIndex, hash, &moveValue);
  };
  TRACERETURN(myCount, &moveValue)
  if (enableDump == myCount) 
  {
//    enableDump = 0;
  };
  return moveValue;
};


int CON::MeasureRope(void)
{
  int nn, nnA;
  int npm;
  PossibleMove pm[100];
  PossibleMove *cpm = pm-1;
  int ropeCount = 0;
#ifdef _CHECK
  CheckEverything(0, m_numNode, true);
#endif
//  int loopCount = 0, chainCount = 0; //Isolated loops and chains
//  LINKTYPE lt;
  {
  //There are generally no capturing moves.  
  //The previous mover would have removed them at 
  //less cost than having us search for them.
  //However, at startup the user may have presented
  //us with capturable coins.  If we find any
  //during our tour of the nodes then we quit right
  //there and quit trying any other moves.  Taking
  //them has to be the right thing to do.  If we
  //have any choice we will both try taking them all
  //and try the hard-hearted handout.
  // 
  //We go throungh the nodes and make list of all
  //possible positions after a single move.  We rate each
  //of the possibilities.  The rating is based on
  //two things:
  // 1 - The nimstring value - smaller is better.
  // 2 - The number of coins offered our opponent - smaller is better.
  // 3   provided both are short or both are long.
    for (nn=0; nn<m_numNode; nn++)
    {
      int en, ec, nnB;
      nnA = nd[nn].ix;
      if (IS_ISOLATED(sd[nnA][0].ns))
      {
        //ROPE if (nn < m_numNode-1)
        //ROPE {
        //ROPE   nnB = nd[nn+1].ix;
        //ROPE   if (sd[nnA][0].ns == sd[nnB][0].ns)
        //ROPE   {
        //ROPE     //The shorter one comes later.
        //ROPE     int longLen;
        //ROPE     if (sd[nnA][0].ns == GND_GND) longLen = 3;
        //ROPE     else if (sd[nnA][0].ns == LOOP) longLen = 4;
        //ROPE     else 
        //ROPE     {
        //ROPE       longLen = 0; //keep compiler quiet
        //ROPE       ImplementationError("Unknown isolated chain type");
        //ROPE     };
        //ROPE     if ((ed[nnA].e[0].el < longLen) || (ed[nnB].e[0].el >= longLen))
        //ROPE     {
        //ROPE       continue;
        //ROPE     };
        //ROPE   };
        //ROPE };
        if (sd[nnA][0].ns == GND_GND)
        {
          int len;
          len = ed[nnA].e[0].el;
          //ROPE if (len + hisScore >= m_neededToWin) 
          //ROPE {
          //ROPE   if (len < result.player2Min) result.player2Min = (ui16)len;
          //ROPE   continue;
          //ROPE };
          if (len > 2) 
          {
            continue;
          };
          COPYPOS
          cpm->offering = len;
          cpm->con.DeleteNode(nn);
          continue;
        }
        else if (sd[nnA][0].ns == LOOP)
        {
          continue;
        }
        else
        {
          NotImplemented("Unknown isolated chain");
        };
      };
      //ROPE if ((nn>0) && (ed[nnA] == ed[nd[nn-1].ix])) continue;
      ec = nd[nnA].ec;
      for (en=0; en<ec; en++)
      {
        int len;
        //ROPE if ((en>0) && (ed[nnA].e[en-1] == ed[nnA].e[en])) 
        //ROPE {
        //ROPE   if (sd[nnA][en-1].ns != nnA)
        //ROPE   {
        //ROPE     continue;
        //ROPE   };
        //ROPE };
        nnB = sd[nnA][en].ns;
        if (IS_A_NODE(nnB) && (nd[nnB].os < nn)) continue; //Only one direction, please
        if (nnB == DEADEND)
        {
          //Well, this is unexpected.;
          continue;
        };
        len = ed[nnA].e[en].el;
        if (nnB == nnA)
        {
          //A loop to ourselves.  A loony move at best.
          continue;
        };
        //ROPE if (len + hisScore >= m_neededToWin) 
        //ROPE {
        //ROPE   if (len < result.player2Min) result.player2Min = (ui16)len;
        //ROPE   continue;
        //ROPE };
        if (len > 2) continue;
        COPYPOS; //
        cpm->offering = len;
        cpm->con.DeleteEdge(nn, en);
      }; //for each link	hisScore	0x0000000d

    };
  };
  npm = cpm - pm + 1;
  //if (npm == 0) 
  //{
  //  result.player2Min = 0;
  //};
  for (int n=0; n<npm; n++)
  {
    NIMCON nimcon;
    if (pm[n].con.m_numNode != 0)
    {
      if (nimcon.NimVal(&pm[n].con) != 0) ropeCount++;
    };
  };
  return ropeCount;
}

