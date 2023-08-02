#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"

extern PossibleMove computerMove;
extern MOVE_TYPE moveType;

void CON::AddGround(int boxNum)
{
  int en;
  ASSERT(nd[boxNum].ec < 4);
  en = nd[boxNum].ec++;
  sd[boxNum][en].ns = GROUND;
  sd[boxNum][en].ne = UNUSED;
  ed[boxNum].e[en].el = 0;
  ed[boxNum].e[en].ng = GROUND;
}
  


void CON::AddLink(int boxNum1, int boxNum2)
{ // 
  int en1, en2;
  ASSERT(nd[boxNum1].ec < 4);
  ASSERT(nd[boxNum2].ec < 4);
  en1 = nd[boxNum1].ec++;
  en2 = nd[boxNum2].ec++;
  sd[boxNum1][en1].ns = (STOPNUM)boxNum2;
  ed[boxNum1].e[en1].ng = (STOPNUM)boxNum2;
  sd[boxNum1][en1].ne = (EDGENUM)en2;
  ed[boxNum1].e[en1].el = 0;
  sd[boxNum2][en2].ns = (STOPNUM)boxNum1;
  ed[boxNum2].e[en2].ng = (STOPNUM)boxNum1;
  sd[boxNum2][en2].ne = (EDGENUM)en1;
  ed[boxNum2].e[en2].el = 0;
}

void CON::GrabAnything(void)
{
  int nn, nnA;
  // Grab any available coin.
  for (nn=0; nn<m_numNode; nn++)
  {
    //First we look for a chain with DEADEND on both ends.
    nnA = nd[nn].ix;
    if (sd[nnA][0].ns == DE_DE)
    {
      if (ed[nnA].e[0].el == 2)
      {
        computerMove.con = *this;
        computerMove.con.DeleteNode(nn);
      }
      else if (ed[nnA].e[0].el >= 3)
      {
        computerMove.con = *this;
        computerMove.con.ed[nnA].e[0].el--;
      }
      else
      {
        ImplementationError("de-de length < 2");
      };
      computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
      computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
      moveType = MT_GRABANYTHING;
      return;
    };
  };
  for (nn=0; nn<m_numNode; nn++)
  {
    //Now look for a chain from DEADEND to GROUND
    nnA = nd[nn].ix;
    if (sd[nnA][0].ns == GND_DE)
    {
      computerMove.con = *this;
      if (ed[nnA].e[0].el == 1)
      {
        computerMove.con.DeleteNode(nn);
      }
      else
      {
        computerMove.con.ed[nnA].e[0].el--;
      };
      moveType = MT_GRABANYTHING;
      computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
      computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
      return;
    };
  };
  for (nn=0; nn<m_numNode; nn++)
  {
    //Now look for a chain from DEADEND to NODE
    nnA = nd[nn].ix;
    for (int en=0; en<nd[nnA].ec; en++)
    {
      if (sd[nnA][en].ns == DEADEND)
      {
        if (ed[nnA].e[en].el > 1)
        {
          computerMove.con = *this;
          computerMove.con.ed[nnA].e[en].el--;
        }
        else
        {
          computerMove.con = *this;
          computerMove.con.DeleteEdge(nn,en);
        };
        computerMove.con.Canon(0,computerMove.con.m_numNode);
#ifdef _CHECK
        computerMove.con.CheckEverything(0,computerMove.con.m_numNode,false);
#endif
        moveType = MT_GRABANYTHING;
        return;
      };
    };
  };
}


CON& CON::operator =(const DOTS_AND_BOXES& db)
{
  int row, col, width, height, rowStart, boxNum;
  int linksPerRow, linkStart;
  m_numNode = 0;
  nd[GROUND].gn = GROUND;
  nd[DEADEND].gn = DEADEND;
  width = db.Width()-1;    // #boxes
  height = db.Height()-1;  // #boxes
  m_neededToWin = width * height/2 + 1;
  //m_numJoint = width*height;
  linksPerRow = 2*width+1;
  //nd[0].ec = 0;
  //nd[0].ix = 0;
  //nd[0].os = 0;
  Clear(0);
  //nd[1].ec = 0;
  //nd[1].ix = 1;
  //nd[1].os = 1;

  
  // Central rows *********************************************
  for (linkStart=width, rowStart=0, row=0; 
       row<height; 
       row++, rowStart+=width, linkStart+=linksPerRow)
  {
    for (boxNum=rowStart,col=0; col<width; col++,boxNum++)
    {

      if (row == 0)
      {
        if (col < width-1) 
        {
          Clear(boxNum+1);
        };
        if (db.HLineType(col, 0) == LINE_NONE) AddGround(col);
      };

      
      if (row < height-1)
      {
      
        Clear(boxNum+width);
        if (db.HLineType(col, row+1) == LINE_NONE) 
                         AddLink(boxNum, boxNum+width);
        if (col == 0)
        {
          if (db.VLineType(col, row) == LINE_NONE) AddGround(boxNum);
        };
        if (col < width-1)
        {
          if (db.VLineType(col+1, row) == LINE_NONE) AddLink(boxNum, boxNum+1);
        }
        else
        {
          if (db.VLineType(col+1, row) == LINE_NONE) AddGround(boxNum);
        };
      }
      else
      {

        if (db.HLineType(col, row+1) == LINE_NONE) AddGround(boxNum);
        if (col == 0)
        {
          if (db.VLineType(col, row) == LINE_NONE) AddGround(boxNum);
        };
        if (col < width-1)
        {
          if (row == 0)
          {
            nd[boxNum+1].ec = 0;
          };
          if (db.VLineType(col+1, row) == LINE_NONE) AddLink(boxNum, boxNum+1);
        }
        else
        {
          if (db.VLineType(col+1, row) == LINE_NONE) AddGround(boxNum);
        };
      };
    };
  };
  m_numNode = width*height; 
  Compress();//Remove unused nodes.
#ifdef NS_DUMP
//  DUMP("About to form chains",0,0,0,0,m_numNode);
#endif
  _FormChains();
#ifdef NS_DUMP
//  DUMP("Finished forming chains",0,0,0,0,m_numNode);
#endif
  Canon(0,m_numNode);
#ifdef _CHECK
  CheckEverything(0,m_numNode,false);
#endif
  {
    ui count = 0;
    int nn, nnA, nnB, en;
    for (nn=0; nn<m_numNode; nn++)
    {
      nnA = nd[nn].ix;
      if (IS_ISOLATED(sd[nnA][0].ns))
      {
        count += 2*ed[nnA].e[0].el;
      }
      else
      {
        int ec;
        count += 2;
        ec = nd[nnA].ec;
        for (en=0; en<ec; en++)
        {
          nnB = sd[nnA][en].ns;
          if (!IS_A_NODE(nnB))
          {
            count += ed[nnA].e[en].el;
          };
          count += ed[nnA].e[en].el;
        };
      };
    };
    m_coinsRemaining = count/2;
  };
#ifdef _CHECK
  CheckEverything(0, m_numNode, false);
#endif
  return *this;
}
