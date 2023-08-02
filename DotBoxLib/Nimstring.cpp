#include "stdafx.h"
#include <malloc.h>
//#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"
//#include "NewBoxesDoc.h"
//#include "Nimstring.h"

//void SetupLookupTables(void);
extern int enableDump;
//extern NIMHASHTABLE *nimHashTable;
extern i64 currentTime;
extern i64 timeLimit;
extern bool stopThinking;
extern bool searchAborted;


#ifdef NS_DUMP
#define BREAKIF(expr) if(expr) Break();
#define TRACEENTRY(title,n1,n2)        \
  searchDepth++;                       \
  BREAKIF(count==-1);                  \
  BREAKIF(count==-1);                  \
  if (count>-1) enableDump=true;       \
  this->DUMP(searchDepth, myCount, title,n1,n2);
#define TRACEEXIT(count,val)                               \
  LOGUIUI("{%05x} Exit - nimval = %d\n",count,val);\
  searchDepth--;
#define LOGUIUI(format,i1,i2) LogUiUi(format,i1,i2);
#define LOGUII64UI(format,i1,i64,i2) LogUiI64Ui(format,i1,i64,i2);
#else
#define BREAKIF(expr)
#define TRACEENTRY(title,n1,n2)
#define TRACEEXIT(count,val)
#define LOGUIUI(format,i1,i2)
#define LOGUII64UI(format,i1,i64,i2)
#endif


#ifdef _CHECK
void NIMCON::CheckEverything(int nnFirst, int nnLast) const
{
  int nn, nnA, en;
  for (nn=nnFirst; nn<nnLast; nn++)
  {
    nnA = nd[nn].ix;
    if (sd[nnA][0].ns == LOOP)
    {
      if (ed[nnA].e[0].el > 4)
      {
        ImplementationError("Nimstring Loop > 4 coins");
      };
    }
    else
    {
      for (en = 0; en < nd[nnA].ec; en++)
      {
        if (ed[nnA].e[en].el > 3)
        {
          ImplementationError("Nimstring Chain > 3 coins");
        };
      };
    };
  };
  CON::CheckEverything(nnFirst, nnLast, false);
}
#endif


ui64 NIMCON::NimHash(int nnFirst, int nnLast)
{
  int nn, nnA;
  ui32 h1, h2;
  h1 = h2 = 0;
  for (nn = nnFirst; nn<nnLast; nn++)
  {
    nnA = nd[nn].ix;
    h1 = h1*373733 + *((ui32 *)(&ed[nnA].e[0]));
    h2 = h2*434347 + *((ui32 *)(&ed[nnA].e[2]));
    h1 ^= h2 >> 21;
    h2 ^= h1 >> 23;
  };
  return ((ui64)h1<<32) + h2;
}



#ifdef NS_DUMP
class NIM_DUMP_FILE
{
public:
  FILE *f;
  NIM_DUMP_FILE(void){f=NULL;};
  ~NIM_DUMP_FILE(void);
  FILE *file(void);
  void Close(void);
};

NIM_DUMP_FILE::~NIM_DUMP_FILE(void)
{
  Close();
}

void NIM_DUMP_FILE::Close(void)
{
  if(f!=NULL)
  {
    fclose(f);
    f=NULL;
  };
}


FILE *NIM_DUMP_FILE::file(void)
{
  if (f != NULL) return f;
  f = fopen("NimDump.txt","w");
  return f;
}

NIM_DUMP_FILE df;




#endif


#define CreateNodes(neededNode)                 \
{                                               \
  for (; numNode<=(neededNode); numNode++)      \
{                                               \
    nodes[numNode].L[0]  = 0;                   \
    nodes[numNode].L[1]  = 0;                   \
    nodes[numNode].L[2]  = 0;                   \
    nodes[numNode].L[3]  = 0;                   \
    nodes[numNode].LT    = EMP0|EMP1|EMP2|EMP3; \
    nodes[numNode].len   = 0;                   \
    nodes[numNode].fei   = 0;                   \
    nodes[numNode].grp   = 0;                   \
  };                                            \
}

#define GETLENGTH(nn,ln)         \
    ((nodes[nn].len >> (6-2*ln)) & 3)


#define SETGROUND(nn,ln,length)                                  \
{                                                                \
  nodes[nn].L[ln] = 0;                                           \
  SETFIELD(ln,length,nodes[nn].len);                             \
  nodes[nn].LT = (ui8)(nodes[nn].LT & (~MSK##ln) | GND##ln); \
}



bool NIMCON::CombineEdges(int nnA)
{
//  int len;
  ASSERT(nd[nnA].ec == 2);
  if (sd[nnA][0].ns == GROUND)
  {
    if (sd[nnA][1].ns == GROUND)
    {
      ed[nnA].e[0].ng = GND_GND;
      ed[nnA].e[0].el = (EDGECOUNT)(ed[nnA].e[0].el + ed[nnA].e[1].el + 1);
      ed[nnA].e[1].Clear();
      sd[nnA][0].ns = GND_GND;
      sd[nnA][1].Clear();
      sd[nnA][0].ne = UNUSED;
      nd[nnA].ec = 0;
      return false;
    }
    else
    {//GROUND to non-GROUND
      //Set neighbor[1] edge to GROUND.  Tell Caller to delete this node.
      int len, nnB, edgeB;
      len = ed[nnA].e[1].el + ed[nnA].e[0].el + 1;
      if (len > 3) len = 3;
      nnB = sd[nnA][1].ns;
      edgeB = sd[nnA][1].ne;
      ed[nnB].e[edgeB].ng = GROUND;
      if (len == 1)
      {
        NotImplemented("Check to set length to zero");
      }
      else
      {
        ed[nnB].e[edgeB].el = (EDGELEN)len;
      };
      sd[nnB][edgeB].ns = GROUND;
      sd[nnB][edgeB].ne = UNUSED;
      return true;
    };
  }
  else
  {
    ASSERT(IS_A_NODE(sd[nnA][0].ns));
    if (sd[nnA][1].ns == GROUND)
    {//non-GROUND to GROUND
      //Set neighbor[0] edge to GROUND.  Tell Caller to delete this node.
      int len, nnB, edgeB;
      len = ed[nnA].e[1].el + ed[nnA].e[0].el + 1;
      if (len > 3) len = 3;
      nnB = sd[nnA][0].ns;
      edgeB = sd[nnA][0].ne;
      ed[nnB].e[edgeB].ng = GROUND;
      if (len == 1)
      {
        int en;
        len = 0;
        for (en=0; en<nd[nnB].ec; en++)
        {
          if (ed[nnB].e[en].el >= 2) continue;
          if (IS_A_NODE(sd[nnB][en].ns))
          {
            len = 1;
            break;;
          };
          ASSERT(sd[nnB][en].ns == GROUND);
          if (en == edgeB) continue;
          if (ed[nnB].e[en].el == 0) continue;
          len = 1;
          break;
        };
          ed[nnB].e[edgeB].el = (EDGELEN)len;
      }
      else
      {
        ed[nnB].e[edgeB].el = (EDGELEN)len;
      };
      sd[nnB][edgeB].ns = GROUND;
      sd[nnB][edgeB].ne = UNUSED;
      return true;
    }
    else
    {//node-to-node
      int len, nnB, nnC, edgeB, edgeC;
      len = ed[nnA].e[1].el + ed[nnA].e[0].el + 1;
      nnB = sd[nnA][0].ns;
      edgeB = sd[nnA][0].ne;
      nnC = sd[nnA][1].ns;
      edgeC = sd[nnA][1].ne;
      if (len >= 3)
      {//We can snap a long chain right now
        sd[nnB][edgeB].ns = GROUND;
        sd[nnC][edgeC].ns = GROUND;
        sd[nnB][edgeB].ne = UNUSED;
        sd[nnC][edgeC].ne = UNUSED;
        ed[nnB].e[edgeB].ng = GROUND;
        ed[nnC].e[edgeC].ng = GROUND;
        ed[nnB].e[edgeB].el = 3;
        ed[nnC].e[edgeC].el = 3;
      }
      else
      {
        sd[nnB][edgeB].ns = (STOPNUM)nnC;
        ed[nnB].e[edgeB].ng = (STOPNUM)nnC;
        sd[nnC][edgeC].ns = (STOPNUM)nnB;
        ed[nnC].e[edgeC].ng = (STOPNUM)nnB;
        sd[nnB][edgeB].ne = (EDGENUM)edgeC;
        sd[nnC][edgeC].ne = (EDGENUM)edgeB;
        ed[nnB].e[edgeB].el = (EDGELEN)len;
        ed[nnC].e[edgeC].el = (EDGELEN)len;
      };
      return true;
    };
  };
}


void NIMCON::DeleteNode(int nn, int *nnLast)
{
  // You better have arranged that nothing points to
  // the node being deleted.
  (*nnLast)--;
  for (int i=nn; i<*nnLast; i++)
  {
    nd[i].ix = nd[i+1].ix;
    nd[nd[i].ix].os = (STOPNUM)i;
  };
}



void NIMCON::DeleteEdge(int nn, int en, int *nnLast)
{
  //The edge originates at node nn and may terminate
  //anywhere: GROUND, DEADEND, another node, or even this node.
  int nnA, nnB, edgeB, ecA, ecB;
  nnA = nd[nn].ix;
  nnB = sd[nnA][en].ns;
  if (IS_A_NODE(nnB))
  {
    //We will delete it from the far node first.
    edgeB = sd[nnA][en].ne;
    if (nnB == nnA)
    {
      if (nd[nnA].ec < 4)
      {
        NotImplemented("Deleting looping edge");
      };
      if (edgeB < en)
      {
        //swap so the larger edge is deleted first
        edgeB = en;
        en = sd[nnA][en].ne;
      };
    };
    ecB = --nd[nnB].ec;
    while (edgeB < ecB) 
    {
      MoveEdge(nnB,edgeB+1,edgeB);
      edgeB++;
    };
    ed[nnB].e[ecB].Clear();
    sd[nnB][ecB].Clear();
    if (ecB < 3)
    {
      if (CombineEdges(nnB))
      {
        DeleteNode(nd[nnB].os, nnLast);
      };
    };
  };
  //Now delete the edge from the near node.
  ecA = --nd[nnA].ec;
  while (en < ecA) 
  {
    MoveEdge(nnA,en+1,en);
    en++;
  };
  ed[nnA].e[ecA].Clear();
  sd[nnA][ecA].Clear();
  if (ecA < 3)
  {
    if (CombineEdges(nnA))
    {
      DeleteNode(nd[nnA].os, nnLast);
    };
  };
}



int searchDepth = 2;
static int count = 10;

int NIMCON::NimVal(const int nnFirst, const int nnLast)
{
  //Method:
  //1) Divide into separate strings.
  //   For each string:
  //   a) Simplify
  //   b) Canon
  //   c) Check cached values
  //   d) For each possible move
  //      1) Copy to free area.
  //      2) Make the move.
  //      3) Get its nim value.
  int firstNode, i, n;
  ui64 *pVisited;
  ui8 visited[MAX_NODES+7];
  i32 hashIndex;
  NIMCON nimcon;
  ui32 mex;
  ui32 result=0;
  ASSERT(nnLast > nnFirst);
  ui64 hash;
  searchDepth++;
#ifdef _DEBUG
  count++;
  i32 myCount;
  myCount = count;
#endif
#ifdef NS_DUMP
  LOGUIUI("%d Enter NimVal", myCount, 0);
  DUMP("Entering NimVal",myCount,0,0,nnFirst,nnLast);
#endif
  ASSERT(!Simplify(nnFirst, nnLast));
  n = (unsigned)(nnLast+7)/8;
  pVisited = (ui64 *)visited;
  for (i=0; i<n; i++)
  {
    *(pVisited++) = 0;
  };
  firstNode = nnFirst;
  {
    int nn, nnA, en, numToVisit, numVisited, neighbor, neighborA;;   int nnCurrent;
    int toVisit[MAX_NODES];
    int j;
    while (firstNode < nnLast)
    {
      int lastNode;
      numToVisit = 1;
      numVisited = 0;
      toVisit[0] = firstNode;
      visited[firstNode] = 1;
      while (numVisited < numToVisit)
      {
        nnCurrent = toVisit[numVisited++];
        //if (visited[nnCurrent] != 0) continue;
        //visited[nnCurrent] = 1;
        nnA = nd[nnCurrent].ix;
        if (IS_ISOLATED(sd[nnA][0].ns))
        {
          ASSERT(numVisited == 1);
        }
        else
        {
          for (en=0; en<nd[nnA].ec; en++)
          {
            neighborA = sd[nnA][en].ns;
            if (IS_A_NODE(neighborA))
            {
              neighbor = nd[neighborA].os;
              if (visited[neighbor] == 0)
              {
                visited[neighbor] = 1;
                toVisit[numToVisit++] = neighbor;
              };
            };
          };
        };
      };
      //Now collect all the visited nodes together
      j = firstNode;
      nn = firstNode;
      lastNode = firstNode + numVisited;
      for (i=0; i<numVisited; i++)
      {
        if (toVisit[i] < lastNode) continue;
        //We have to move toVisit[i] into the collection
        //of visited nodes.
        //Find an unvisited node to swap with.
        while (visited[j] != 0) j++;
        SwapNode(j, toVisit[i]);
        visited[toVisit[i]] = 0;
        visited[j] = 1;
      };
      if (IS_ISOLATED(sd[nd[firstNode].ix][0].ns))
      {
        //This is an isolated string.  easy to compute the nimstring value
        nnA = nd[firstNode].ix;
        if (sd[nnA][0].ns == GND_GND)
        {
          if (ed[nnA].e[0].el < 3)
          {
            result ^= 1;
#ifdef NS_DUMP
            LOGUIUI(" ",0,0);
            LOGUIUI("%dIsolated sub-String value = 1",myCount,0);
#endif
          };
          //else LOONY
        }
        else if (sd[nnA][0].ns == LOOP) 
        {
          result ^= 0;
        }
        else
        {
          NotImplemented("Nim of non-GND_GND isolated string");
        };
      }
      else
      {
        if (lastNode-firstNode <= NIM_MAX_CANON) Canon(firstNode, lastNode);
        hash = NimHash(firstNode, lastNode);
#ifdef NS_DUMP
        DUMP("NimVal Canonized sub-string",myCount,
          (ui32)(hash>>32),(ui32)hash,
          firstNode,lastNode);
#endif
#ifdef _CHECK
        CheckEverything(firstNode, lastNode);
#endif
        hashIndex = nimHashTable->Exists(hash);
        if (hashIndex >= 0)
        {
          result ^= nimHashTable->GetValue(hashIndex);
          LOGUIUI(" ",0,0);
          LOGUII64UI("%d Sub-string Hash=%016I64x Value = %d",myCount,hash,nimHashTable->GetValue(hashIndex));
        }
        else
        {
          //Now we need to make all the possible moves and select the
          //minimum excluded value;
          if (stopThinking || (currentTime > timeLimit)) 
          {
            searchAborted = true;
            return 99;
          };
          mex = 0;
          for (nn=firstNode; nn<lastNode; nn++)
          {
            nnA = nd[nn].ix;
            const int ec = nd[nnA].ec;
            if ((nn>firstNode) && ((nd[nnA].gn == nd[nd[nn-1].ix].gn))) continue;
            for (en=0; en<ec; en++)
            {
              ASSERT(sd[nnA][en].ns != UNUSED);
              if (    (en!=0) 
                   && (ed[nnA].e[en-1].el == ed[nnA].e[en].el)
                   && (nd[sd[nnA][en-1].ns].gn == nd[sd[nnA][en].ns].gn)) continue;
              if (ed[nnA].e[en].el < 3)
              {
                if (sd[nnA][en].ns == GROUND)
                {
                  int last = lastNode;
  


                  for (i=firstNode; i<lastNode; i++)
                  { //Copy the string.
                    int nnA;
                    nimcon.nd[i] = nd[i];
                    nnA = nd[i].ix;
                    nimcon.sd[nnA][0] = sd[nnA][0];
                    nimcon.sd[nnA][1] = sd[nnA][1];
                    nimcon.sd[nnA][2] = sd[nnA][2];
                    nimcon.sd[nnA][3] = sd[nnA][3];
                    nimcon.ed[nnA] = ed[nnA];
                    nimcon.nd[nnA] = nd[nnA];
                    nimcon.nd[nnA].gn = (STOPNUM)i;
                  };
                  nimcon.DeleteEdge(nn,en,&last);
                  //{ //MoveEdge
                  //  while (en < ec-1)
                  //  {
                  //    con.sd[nnA][en] = con.sd[nnA][en+1];
                  //    con.ed[nnA].e[en] = con.ed[nnA].e[en+1];
                  //    if (IS_A_NODE(con.sd[nnA][en].ns))
                  //    {
                  //      con.sd[con.sd[nnA][en].ns][con.sd[nnA][en].ne].ne=(EDGENUM)en;
                  //    };
                  //    en++;
                  //  };
                  //  con.sd[nnA][ec-1].Clear();
                  //  con.ed[nnA].e[ec-1].Clear();
                  //};
                  //con.nd[nnA].ec--;
                  //if (ec == 3)
                  //{
                  //  if (con.CombineEdges(nnA))
                  //  {
                  //    con.DeleteNode(nn, &last);
                  //    //con.nd[nn].ix = con.nd[--last].ix;
                  //    //con.nd[con.nd[nn].ix].os = (STOPNUM)nn;
                  //  };
                  //};
                  mex |= 1 << nimcon.NimVal(firstNode, last);



#ifdef Searching0
                  if ((searchDepth == 1) && (mex & 1)) return 1;
#endif
                  




                }
                else
                {
                  int nnB;
                  ASSERT(IS_A_NODE(sd[nnA][en].ns));
                  nnB = sd[nnA][en].ns;
                  if (nd[nnB].os > nn)
                  {
                    int last = lastNode;
                    for (i=firstNode; i<lastNode; i++)
                    {
                      int nnA;
                      nimcon.nd[i] = nd[i];
                      nnA = nd[i].ix;
                      nimcon.sd[nnA][0] = sd[nnA][0];
                      nimcon.sd[nnA][1] = sd[nnA][1];
                      nimcon.sd[nnA][2] = sd[nnA][2];
                      nimcon.sd[nnA][3] = sd[nnA][3];
                      nimcon.ed[nnA] = ed[nnA];
                      nimcon.nd[nnA] = nd[nnA];
                      nimcon.nd[nnA].gn = (STOPNUM)i;
                    };
  
                
                
                
                
                
                
                    {
                      // We need to remove the edge from the other node. (pna,one)
                      //onn = nodes[nn].L[en];
                      //oei = GETFIELD(en,nodes[nn].fei);
                      //int enB, ecB;
                      //ecB = con.nd[nnB].ec;
                      //enB = con.sd[nnA][en].ne;
                      nimcon.DeleteEdge(nn, en, &last);
                      //{ //MoveEdge
                      //  while (enB < ecB-1)
                      //  {
                      //    con.sd[nnB][enB] = con.sd[nnB][enB+1];
                      //    con.ed[nnB].e[enB] = con.ed[nnB].e[enB+1];
                      //    if (IS_A_NODE(con.sd[nnB][enB].ns))
                      //    {
                      //      con.sd[con.sd[nnB][enB].ns][con.sd[nnB][enB].ne].ne=(EDGENUM)enB;
                      //    };
                      //    enB++;
                      //  };
                      //  con.sd[nnB][ecB-1].Clear();
                      //  con.ed[nnB].e[ecB-1].Clear();
                      //};
                      //con.nd[nnB].ec--;
                      //if (ecB == 3) //It has been reduced to 2.
                      //{
                      //  //We know that this node number is the larger
                      //  //of the two (ie:  nnB > nnA).
                      //  //So, if we need to delete it we will not disturb
                      //  //the node at nnA.
                      //  if (con.CombineEdges(nnB))
                      //  {
                      //    int nB = con.nd[nnB].os;
                      //    con.DeleteNode(nB, &last);
                      //    //con.nd[nB].ix = con.nd[--last].ix;
                      //    //con.nd[con.nd[nB].ix].os = (STOPNUM)nB;
                      //  };
                      //  //Remove the edge from this node
                      // //  nodes[sn].len[en] = 0;
                      // //  SETFIELD(en,EMP,nodes[sn].len);
                      //};
                    };
                    //{
                    //  // Now we need to remove the edge from this node. (nnA, en)
                    //  { //MoveEdge
                    //    while (en < ec-1)
                    //    {
                    //      con.sd[nnA][en] = con.sd[nnA][en+1];
                    //      con.ed[nnA].e[en] = con.ed[nnA].e[en+1];
                    //      if (IS_A_NODE(con.sd[nnA][en].ns))
                    //      {
                    //        con.sd[con.sd[nnA][en].ns][con.sd[nnA][en].ne].ne=(EDGENUM)en;
                    //      };
                    //      en++;
                    //    };
                    //    con.sd[nnA][ec-1].Clear();
                    //    con.ed[nnA].e[ec-1].Clear();
                    //  };
                    //  con.nd[nnA].ec--;
                    //  if (ec == 3) //It has been reduced to 2.
                    //  {
                    //    //We know that this node number is the larger
                    //    //of the two (ie:  nnB > nnA).
                    //    //So, if we need to delete it we will not disturb
                    //    //the node at nnA.
                    //    if (con.CombineEdges(nnA))
                    //    {
                    //      int mynn = con.nd[nnA].os;
                    //      con.DeleteNode(mynn, &last);
                    //      //con.nd[mynn].ix = con.nd[--last].ix;
                    //      //con.nd[con.nd[mynn].ix].os = (STOPNUM)mynn;
                    //    };
                    //    //Remove the edge from this node
                    ////  nodes[sn].len[en] = 0;
                    ////  SETFIELD(en,EMP,nodes[sn].len);
                    //  };
                    //};
                    {
                      int val;
                      val = nimcon.NimVal(firstNode, last);
                      if (val == 99) return 99;
                      mex |= 1 << val;

                    
                    
#ifdef Searching0
                      if ((searchDepth == 1) && (mex & 1)) return 1;
#endif
                    
                    
                    
                    };
              
              
              
              
              
              
              
              
                  };
                };
              };
            };
          };//for each node
		  int r;
          for ( r=0; mex&1; r++,mex>>=1);
          LOGUIUI(" ",0,0)
          LOGUII64UI("%d Computed Substring Hash=%016I64x Set value = %d", myCount, hash, r);
          result ^= r;
          nimHashTable->EnterValue(hash, (NIMVALUE)r);
        };
      };
      firstNode += numVisited;
    };
  };
  LOGUIUI("%d Exit NimVal result=%d", myCount, result);
  searchDepth--;
  return result;
};


NIMCON& NIMCON::operator =(const CON *con)
{
  int nn, nnA;
  m_numNode        = con->m_numNode;
  for (nn=0; nn<m_numNode; nn++)
  {
    nnA = con->nd[nn].ix;
    nd[nn].ix = (STOPNUM)nnA;
    sd[nnA][0] = con->sd[nnA][0];
    sd[nnA][1] = con->sd[nnA][1];
    sd[nnA][2] = con->sd[nnA][2];
    sd[nnA][3] = con->sd[nnA][3];
    ed[nnA] = con->ed[nnA];
    nd[nnA] = con->nd[nnA];
  };
  return *this;
}

int NIMCON::NimVal(const CON *con)
{
  ui result;
  int nn;
  bool modified;
//  SetupLookupTables();
  *this = con;
  Simplify(0, m_numNode);
  //
  //
  //
  // We need to deal with capturable coins.
  // There are two cases.
  // 1) taking the coins does not change the nim-value.
  // 2) the nim-value is LOONY
  do
  {
    modified = false;
    for (nn=0; !modified && (nn<m_numNode); nn++)
    {
      int nnA, en, ec;
      nnA = nd[nn].ix;
      if (IS_ISOLATED(sd[nnA][0].ns))
      {
        if (     (sd[nnA][0].ns == GND_GND)
              || (sd[nnA][0].ns == LOOP) ) continue;
        if (sd[nnA][0].ns == DE_DE)
        {
          int el;
          el = ed[nnA].e[0].el;
          if (el < 4)
          {
            DeleteNode(nn, &m_numNode);
            modified = true;
          }
          else
          {
            return LOONY4;
          };
        }
        else if (sd[nnA][0].ns == GND_DE)
        {
          int el;
          el = ed[nnA].e[0].el;
          if (el < 2)
          {
            DeleteNode(nn, &m_numNode);
            modified = true;
          }
          else
          {
            return LOONY2;
          };
        }
        else
        {
          ImplementationError("Illegal Capturable configuration");
        };
      }
      else
      {
        ec = nd[nnA].ec;
        for (en=0; !modified && (en<ec); en++)
        {
          if (sd[nnA][en].ns == DEADEND)
          {
            int el;
            el = ed[nnA].e[en].el;
            if (el == 1)
            {
              DeleteEdge(nn, en, &m_numNode);
              modified = true;
            }
            else
            {
              return LOONY2;
            };
          };
        };
      };
    };
  } while (modified);



  result = NimVal(0,m_numNode);

  return result;
}
