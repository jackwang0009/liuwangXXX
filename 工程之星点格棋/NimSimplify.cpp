#include "stdafx.h"
#include <malloc.h>
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
//#include "NewBoxesDoc.h"
//#include "Nimstring.h"



#define SNAPLONG(ln)                                  \
{                                                     \
  if ((nodes[nn].LT & MSK##ln) == 0)                  \
  {                                                   \
    if ((nodes[nn].len & MSK##ln) == MSK##ln)         \
    {                                                 \
      nodes[nn].L[ln] = 0;                            \
      nodes[nn].LT |= GND##ln;                        \
    };                                                \
  };                                                  \
}

/*
void NIMSTRING::SnapLongChains(int numNode)
{
  int nn;
  for (nn=0; nn<numNode; nn++)
  {
    SNAPLONG(0);
    SNAPLONG(1);
    SNAPLONG(2);
    SNAPLONG(3);
  };
}
*/
#define LIST(ln)                               \
{                                              \
  if ((nodes[nn].LT & (EMP##ln|GND##ln)) == 0) \
  {                                            \
    if (listed[nodes[nn].L[ln]] == 0)          \
    {                                          \
      list[toVisit++] = nodes[nn].L[ln];       \
      listed[nodes[nn].L[ln]] = 1;             \
    };                                         \
  };                                           \
};

#define FIXLINK(e,n)                                           \
{                                                              \
  if ((nodes[x].LT & MSK##e) == 0)                             \
{                                                              \
    nodes[nodes[x].L[e]].L[GETFIELD(e,nodes[x].fei)] = (ui8)n; \
  };                                                           \
}
/*
void NIMSTRING::SwapNode(int x, int y)
{
  ui32 t;
  FIXLINK(0,y);
  FIXLINK(1,y);
  FIXLINK(2,y);
  FIXLINK(3,y);
  t=*(ui32 *)(&nodes[x].L[0]);
  *(ui32 *)(&nodes[x].L[0]) = *(ui32 *)(&nodes[y].L[0]);
  *(ui32 *)(&nodes[y].L[0]) = t;
  t=*(ui32 *)(&nodes[x].L[0]+4);
  *(ui32 *)(&nodes[x].L[0]+4) = *(ui32 *)(&nodes[y].L[0]+4);
  *(ui32 *)(&nodes[y].L[0]+4) = t;
//ui8 t;
//#define SwapNode(x,y) \
//    t=nodes[x].idx; nodes[x].idx=nodes[y].idx; nodes[y].idx=t;
  FIXLINK(0,x);
  FIXLINK(1,x);
  FIXLINK(2,x);
  FIXLINK(3,x);
}
*/
/*
int NIMSTRING::Divide(int numNode)
{
  int nn, toVisit, visited, i, j;
  ui8 listed[256];
  if (numNode < 2) return numNode;
  ui8 list[256];
  for (nn=0; nn<((numNode+7)&(~7)); nn+=8)
  {
    *((ui32 *)(listed + nn)) = 0;
    *((ui32 *)(listed + nn + 4)) = 0;
  };
  toVisit=1;
  visited = 0;
  list[0] = 0;
  listed[0] = 1;
  while (visited < toVisit)
  {
    nn = list[visited++];
    LIST(0)
    LIST(1)
    LIST(2)
    LIST(3)
  };
  if (toVisit == numNode) return numNode;
  //We have to go through the list and bring all the marked
  //nodes to the front.
  for (j=numNode-1,i=0; i<toVisit; i++)
  {
    if (listed[i] != 0) continue;
    for (; listed[j]==0; j--){};
    SwapNode(i, j--);
  };
  //Now we need to relocate the remaining nodes down to zero.
  for (nn=toVisit; nn<numNode; nn++)
  {
    if ((nodes[nn].LT & MSK0)==0)
    {
      nodes[nn].L[0] = (ui8)(nodes[nn].L[0] - toVisit);
    };
    if ((nodes[nn].LT & MSK1)==0)
    {
      nodes[nn].L[1] = (ui8)(nodes[nn].L[1] - toVisit);
    };
    if ((nodes[nn].LT & MSK2)==0)
    {
      nodes[nn].L[2] = (ui8)(nodes[nn].L[2] - toVisit);
    };
    if ((nodes[nn].LT & MSK3)==0)
    {
      nodes[nn].L[3] = (ui8)(nodes[nn].L[3] - toVisit);
    };
  };
  return toVisit;
}
*/
/*
#define CTLAS(x,y)                                                \
{                                                                 \
  ui8 t;                                                          \
  if ((CompareFields##x##y[nodes[n].LT]<0)                        \
        ||(   (CompareFields##x##y[nodes[n].LT]==0)               \
           && (CompareFields##x##y[nodes[n].len]<0)               \
          )                                                       \
     )                                                            \
  {                                       \
    t=nodes[n].L[x];nodes[n].L[x]=nodes[n].L[y];nodes[n].L[y]=t;  \
    nodes[n].len = SWAPFIELDS(x,y,nodes[n].len);                  \
    nodes[n].LT  = SWAPFIELDS(x,y,nodes[n].LT);                   \
    nodes[n].fei = SWAPFIELDS(x,y,nodes[n].fei);                  \
    if (GETFIELD(x,nodes[n].LT) == 0)                             \
    SETFIELD(GETFIELD(x,nodes[n].fei),x,nodes[nodes[n].L[x]].fei);\
    if (GETFIELD(y,nodes[n].LT) == 0)                             \
    SETFIELD(GETFIELD(y,nodes[n].fei),y,nodes[nodes[n].L[y]].fei);\
  };                                                              \
};
*/
/*
//Compare nodes by their neighbor distances then types.
#define CompareNeighbors(n)              \
{                                        \
  int lenx, leny, etx, ety;              \
  lenx = nodes[NS*x+n] & LENGTHMASK;     \
  leny = nodes[NS*y+n] & LENGTHMASK;     \
  if (lenx > leny) return true;          \
  if (lenx < leny) return false;         \
  etx = nodes[NS*x+n] & NEIGHBORMASK;    \
  ety = nodes[NS*y+n] & NEIGHBORMASK;    \
  if ((etx!=0)&&(ety==0)) return true;   \
  if ((etx==0)&&(ety!=0)) return false;  \
};
*/

/*
bool NIMSTRING::CompareNodeGroupGT(int x, int y)
{
  //We already know that the edge count, the edge type, and
  //edge lengths are identical.  They are also sorted
  //so that links to nodes come before grounds and empties.
  //We need only compare the group number of the neighbor nodes
  //until we find a non-node edge.

  if ((nodes[x].LT & MSK0) != 0) return false;
  if (nodes[nodes[x].L[0]].grp != nodes[nodes[y].L[0]].grp)
    return nodes[nodes[x].L[0]].grp > nodes[nodes[y].L[0]].grp;

  if ((nodes[x].LT & MSK1) != 0) return false;
  if (nodes[nodes[x].L[1]].grp != nodes[nodes[y].L[1]].grp) 
    return nodes[nodes[x].L[1]].grp > nodes[nodes[y].L[1]].grp;

  if ((nodes[x].LT & MSK2) != 0) return false;
  if (nodes[nodes[x].L[2]].grp != nodes[nodes[y].L[2]].grp) 
    return nodes[nodes[x].L[2]].grp > nodes[nodes[y].L[2]].grp;

  if ((nodes[x].LT & MSK3) != 0) return false;
  return nodes[nodes[x].L[3]].grp > nodes[nodes[y].L[3]].grp;
}
*/
/*

//void NSNODE::CTLAS01(void)
//{
//  ui8 t;
//  if ((CompareFields01[LT]>0)
//        ||(   (CompareFields01[LT]==0)
//           && (CompareFields01[len]>0)
//          )
//     )
//  {
//    t=L[0];L[0]=L[1];L[1]=t;
//    len = SWAPFIELDS(0,1,len);
//    LT  = SWAPFIELDS(0,1,LT);
//  };
//}
*/
/*
#define CEGAS(x,y)                                                \
{                                                                 \
  if (nodes[nodes[n].L[x]].grp>nodes[nodes[n].L[y]].grp)          \
  {                                      \
    t=nodes[n].L[x];nodes[n].L[x]=nodes[n].L[y];nodes[n].L[y]=t;  \
    nodes[n].len = SWAPFIELDS(x,y,nodes[n].len);                  \
    nodes[n].LT  = SWAPFIELDS(x,y,nodes[n].LT);                   \
    nodes[n].fei = SWAPFIELDS(x,y,nodes[n].fei);                  \
    SETFIELD(GETFIELD(x,nodes[n].fei),x,nodes[nodes[n].L[x]].fei);\
    SETFIELD(GETFIELD(y,nodes[n].fei),y,nodes[nodes[n].L[y]].fei);\
  };                                                              \
}
*/
/*
void NIMSTRING::SortEdgesByGroup(int n)
{
  ui equality;
  ui8 t;
  equality = FieldsBothZero[nodes[n].LT] & FieldsEqual[nodes[n].len];
  switch (equality & 7)
  {
  case 0: //abcd
      break;
  case 1: //aabc
      CEGAS(0,1)
      break;
  case 2: //abbc
      CEGAS(1,2)
      break;
  case 3: //aaab
      CEGAS(0,2)
      CEGAS(0,1)
      CEGAS(1,2)
      break;
  case 4: //abcc
      CEGAS(2,3)
      break;
  case 5: //aabb
      CEGAS(0,1)
      CEGAS(2,3)
      break;
  case 6: //abbb
      CEGAS(1,3)
      CEGAS(2,3)
      CEGAS(1,2)
      break;
  case 7: //aaaa
      CEGAS(0,2)
      CEGAS(1,3)
      CEGAS(0,1)
      CEGAS(2,3)
      CEGAS(1,2)
      break;
  };
}
*/

