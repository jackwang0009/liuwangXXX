#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"
#include "malloc.h"


extern int searchDepth;

int enableDump = 0;


void CheckEnable(void)
{
  enableDump = ((searchDepth>=0) && (searchDepth<6)) ?1:0;
  //enableDump = 0;
}


#ifdef SW_DUMP
#define COUNT static int count=0; count++;
#define BREAKIF(expr) if(expr) Break();
#define TRACEENTRY(title,n1,n2)Note(title,n1,n2);
#define LOGUIUI(format,i1,i2) LogUiUi(format,i1,i2);
#define LOGUII64UI(format,i1,i64,i2) LogUiI64Ui(format,i1,i64,i2);
#else
#define COUNT
#define BREAKIF(expr)
#define TRACEENTRY(title,n1,n2)
#define LOGUIUI(format,i1,i2)
#define LOGUII64UI(format,i1,i64,i2)
#endif


static FILE *f;
static bool started = false;


#if (defined SW_DUMP)||(defined NS_DUMP)
void CloseDump(void)
{
  if (f != NULL) 
  {
    fclose(f);
    f = NULL;
  };
  started = false;
}
#endif

CON::CON(void)
{
  m_numNode = 0;
  nd[GROUND].gn = GROUND;
  nd[DEADEND].gn = DEADEND;
}


CON::~CON(void)
{
//  NotImplemented("~CON");
//  CString msg;
//  msg.Format("Hash Stats = %d %d %d",hashTries, hashFinds, hashUses);
//  Note(msg);
}


CON& CON::operator =(const CON& con)
{
  int nn, nnA;
  m_numNode        = con.m_numNode;
  m_coinsRemaining = con.m_coinsRemaining;
  m_neededToWin    = con.m_neededToWin;
  nd[GROUND].gn = GROUND;
  nd[DEADEND].gn = DEADEND;
  for (nn=0; nn<m_numNode; nn++)
  {
    nnA = con.nd[nn].ix;
    nd[nn].ix = (STOPNUM)nnA;
    *((ui64 *)&sd[nnA][0]) = *((ui64 *)&con.sd[nnA][0]);
    //sd[nnA][1] = con.sd[nnA][1];
    //sd[nnA][2] = con.sd[nnA][2];
    //sd[nnA][3] = con.sd[nnA][3];
    ed[nnA] = con.ed[nnA];
    nd[nnA] = con.nd[nnA];
  };
  return *this;
}

bool CON::operator ==(const CON& con) const
{
  int nn, en, nnA1, nnA2;
  if (m_numNode != con.m_numNode) return false;
  for (nn=0; nn<m_numNode; nn++)
  {
    nnA1 = nd[nn].ix;
    nnA2 = con.nd[nn].ix;
    if (nd[nnA1].ec != con.nd[nnA2].ec) return false; 
    if (ed[nnA1].e[0].el != con.ed[nnA2].e[0].el) return false;
    if (ed[nnA1].e[0].ng != con.ed[nnA2].e[0].ng) return false;
    for (en=1; en<nd[nnA1].ec; en++)
    {
      if (ed[nnA1].e[en].el != con.ed[nnA2].e[en].el) return false;
      if (ed[nnA1].e[en].ng != con.ed[nnA2].e[en].ng) return false;
      //if (sd[nnA1][en].ne != con.sd[nnA2][en].ne) return false;
      //if (sd[nnA1][en].ns != con.sd[nnA2][en].ns) return false;
    };
  };
  return true;
}

#if defined(SW_DUMP) || defined(NS_DUMP)

void CON::DUMP(char *title, int count, int p1, int p2, 
               int nnFirst, int nnLast) const
{
  int srchDepth;
  srchDepth = (searchDepth<0)?0:searchDepth;
  CheckEnable();
  if (enableDump == 0) return;
  //char *nullString = "";
  char prefix[81];
  int numPrefix;
  numPrefix = srchDepth*5;
  if (numPrefix >80) numPrefix = 80;
  memset(prefix, ' ', numPrefix);
  prefix[numPrefix] = 0;
  int line;
  int i, nn;
  if (!started)
  {
    f = fopen("SWDump.txt","w");
  };
  if (f == NULL)
  {
    int kkk=1;
  };
  started = true;
  fprintf(f,"\n\n%sSearch depth = %d\n", prefix, srchDepth);
  fprintf(f,"%s%d %s\n%s(%08x,%08x)\n",prefix,count, title, prefix, p1, p2);
//  for (stringNum=0; stringNum<m_numString; stringNum++)
//  {
//    fprintf(f, "\n%sString %d\n", prefix, stringNum);
  for (line=nnFirst; line<nnLast; line++)
  {
    fprintf(f, "%s", prefix);
    nn = line;
    fprintf(f, "%2x|%02x|", nn, nd[nn].ix);
    for (i=0; i<4; i++)
    {
      if (i!=0) fprintf(f," ");
      //if ((i < nd[nd[nn].ix].ec) || (i==0)&&(nd[nd[nn].ix].ec==0))
      //{
      if (sd[nd[nn].ix][i].ns==0xff)
      {
        fprintf(f,"  :");
      }
      else
      {
        fprintf(f, "%02x:", sd[nd[nn].ix][i].ns);
      };
      if (sd[nd[nn].ix][i].ne==0xff)
      {
        fprintf(f,"  ");
      }
      else
      {
        fprintf(f,"%02x",sd[nd[nn].ix][i].ne);
      };
      //}
      //else
      //{
      //  fprintf(f, "%2s",nullString);
      //};
    };
    fprintf(f,"|");
    for (i=0; i<4; i++)
    {
      if (i!=0) fprintf(f," ");
      if (ed[nd[nn].ix].e[i].ng==0xff)
      {
        fprintf(f,"  :");
      }
      else
      {
        fprintf(f, "%02x:", ed[nd[nn].ix].e[i].ng);
      };
      if (ed[nd[nn].ix].e[i].el==0xff)
      {
        fprintf(f,"  ");
      }
      else
      {
        fprintf(f,"%02x",ed[nd[nn].ix].e[i].el);
      };
    };
    fprintf(f,"|");
    fprintf(f," %02x",nd[nd[nn].ix].gn);
    fprintf(f,"\n");
  };
}

void CON::TraceReturn(int myCount, SWVALUE *pValue)
{
  CheckEnable();
  if (enableDump == 0) return;
//  FILE *f;
  char prefix[81];
  int numPrefix;
  numPrefix = searchDepth*5;
  if (numPrefix >80) numPrefix = 80;
  memset(prefix, ' ', numPrefix);
  prefix[numPrefix] = 0;
  if (!started)
  {
    f = fopen("SWDump.txt","w");
  };
  if (f == NULL)
  {
    int kkk=1;
  };
  started = true;
  fprintf(f,"%s%d Search Depth = %d value=(%d,%d)\n",prefix,myCount,searchDepth,pValue->player1Min,pValue->player2Min);
}


void CON::LogUiUi(char *format, ui i1, ui i2) const
{
  int srchDepth = searchDepth;
  if (srchDepth < 0) srchDepth = 0;
  CheckEnable();
  if (enableDump == 0) return;
//  FILE *f;
  char prefix[81];
  int numPrefix;
  numPrefix = srchDepth*5;
  if (numPrefix >80) numPrefix = 80;
  memset(prefix, ' ', numPrefix);
  prefix[numPrefix] = 0;
  if (!started)
  {
    f = fopen("SWDump.txt","w");
  };
  if (f == NULL)
  {
    int kkk=1;
  };
  started = true;
  fprintf(f, "%s", prefix);
  fprintf(f, format, i1, i2);
  fprintf(f,"\n");
}

void CON::LogUiI64Ui(char *format, ui i1, ui64 i2, ui i3)
{
  int srchDepth = searchDepth;
  if (srchDepth < 0) srchDepth = 0;
  CheckEnable();
  if (enableDump == 0) return;
//  FILE *f;
  char prefix[81];
  int numPrefix;
  numPrefix = srchDepth*5;
  if (numPrefix >80) numPrefix = 80;
  memset(prefix, ' ', numPrefix);
  prefix[numPrefix] = 0;
  if (!started)
  {
    f = fopen("SWDump.txt","w");
  };
  started = true;
  fprintf(f, "%s", prefix);
  fprintf(f, format, i1, i2, i3);
  fprintf(f,"\n");
}

#endif

ui64 CON::SWHash(void)
{
  int nn, nnA;
  ui32 h1, h2;
  h1 = h2 = 0;
  for (nn = 0; nn<m_numNode; nn++)
  {
    nnA = nd[nn].ix;
    h1 = h1*373733 + *((ui32 *)(&ed[nnA].e[0]));
    //h1 = h1*375733 + *((ui32 *)(&ed[nnA].e[0]));
    h2 = h2*434347 + *((ui32 *)(&ed[nnA].e[2]));
    h1 ^= h2 >> 21;
    h2 ^= h1 >> 23;
  };
  return ((ui64)h1<<32) + h2;
}



#ifdef _CHECK

void CERR(char *msg, int nn)
{
  CString err;
  err.Format("Position representation error\nNode number %d\n%s",nn,msg);
  ImplementationError((LPCTSTR)err);
}

void CON::CheckEverything(int nnFirst,
                          int nnLast,
                          bool checkRemainder) const
{
  //We want to make sure the strings are properly formatted, sorted,
  //and in their canonical state.
  int nn, nnA, nnB, en;
  int neighborIndexA, neighborEdge;
  if (nd[GROUND].gn != GROUND)
    CERR("GROUND has wrong Group Number",-1);
  if (nd[DEADEND].gn != DEADEND)
    CERR("DEADEND has wrong Group Number",-1);
  for (nn=nnFirst; nn<nnLast; nn++)
  {
    //Check the index for consistency.
    if (!IS_A_NODE(nd[nn].ix)) CERR("Index value is not a node value",nn);
    nnA = nd[nn].ix;
    if (nd[nnA].os != nn) CERR("Inconsistent index values",nn);
    //Check unused edge data is UNUSED.
    if (   (ed[nnA].e[0].ng == GND_DE)
         ||(ed[nnA].e[0].ng == GND_GND)
         ||(ed[nnA].e[0].ng == DE_DE)
         ||(ed[nnA].e[0].ng == LOOP) )
    {
      if (nd[nnA].ec != 0) CERR("non-existent edge-count non-zero",nn);
      if (ed[nnA].e[0].ng != sd[nnA][0].ns)
        CERR("Inconsistent ng-ns",nn);
      for (en=0; en<4; en++)
      {
        if (en != 0)
        {
          if (ed[nnA].e[en].ng != UNUSED) CERR("non-existent ng not UNUSED",nn);
          if (ed[nnA].e[en].el != UNUSED) CERR("non-existent el not UNUSED",nn);
          if (sd[nnA][en].ns != UNUSED) CERR("non-existent ns not UNUSED",nn);
        };
        if (sd[nnA][en].ne != UNUSED) CERR("non-existent ne not UNUSED",nn);
      };
    }
    else
    {
      // Must be a node
      for (en=nd[nnA].ec; en<4; en++)
      {
        if (sd[nnA][en].ne != UNUSED) CERR("Unused ne not UNUSED",nn);
        if (sd[nnA][en].ns != UNUSED) CERR("Unused ns not UNUSED",nn);
        if (ed[nnA].e[en].el != UNUSED) CERR("Unused el not UNUSED",nn);
        if (ed[nnA].e[en].ng != UNUSED) CERR("Unused ng not UNUSED",nn);
      }
      //Check link consistencies.
      for (en=0; en<nd[nnA].ec; en++)
      {
        if (IS_A_NODE(sd[nnA][en].ns))
        {
          neighborIndexA = sd[nnA][en].ns;
          neighborEdge   = sd[nnA][en].ne;
          if (sd[neighborIndexA][neighborEdge].ns != nnA)
            CERR("Inconsistent stop pointers",nn);
          if (sd[neighborIndexA][neighborEdge].ne != en)
            CERR("Inconsistent edge pointers",nn);
          //if (ed[neighborIndexA].e[neighborEdge].ng != nd[nnA].gn)
          //  CERR("Inconsistent group numbers",nn);
          if (ed[neighborIndexA].e[neighborEdge].el != ed[nnA].e[en].el)
            CERR("Inconsistent link lengths",nn);
        }
        else if (   (sd[nnA][en].ns == GROUND)
                || (sd[nnA][en].ns == DEADEND) )
        {
          if (ed[nnA].e[en].ng != sd[nnA][en].ns)
            CERR("Non-node ng != ns",nn);
          if (sd[nnA][en].ne != UNUSED)
            CERR("Non-node edge not UNUSED",nn);
        }
        else
        {
          if (en != 0) CERR("Non-node stop in other than edge[0]",nn);
        };
      };
    };
    //Now we need to check that the edges and nodes are sorted.
    //The edges within each node are ordered by neighbor type,
    //and then by length.
    for (en=0; en<nd[nnA].ec; en++)
    {
      if (sd[nnA][en].ns == ed[nnA].e[en].ng) continue;
      if (en == nd[nnA].ec-1) continue;
      if (ed[nnA].e[en] > ed[nnA].e[en+1])
        CERR("Edges not sorted and Neighbor Group != Neighbor Stop",nn);
    };
  };
//  for (nn=nnFirst; nn<nnLast-1; nn++)
//  {
//    nnA = nd[nn].ix;
//    nnB = nd[nn+1].ix;
//    if (    (nd[nnA].gn > nd[nnB].gn)
//         || (nd[nnA].gn == nd[nnB].gn) && (ed[nnA] < ed[nnB]))
//      CERR("nodes not sorted",nn);
//  };
  if (checkRemainder)
  {
    ui count = 0;
    for (nn=nnFirst; nn<nnLast; nn++)
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
    if (count != 2*m_coinsRemaining)
    {
      ImplementationError("Wrong coinsRemaining");
    };
  };
};
#endif
