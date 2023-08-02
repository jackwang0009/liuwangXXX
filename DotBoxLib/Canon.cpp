#include "stdafx.h"
//#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"
//#include "NewBoxesDoc.h"

extern ui32 ct[256];

//Compare two nodes by <edgeLengths> <edgeTypes> 
#define CT(x,e) ct[n[x].s[e]]

#define CompareTypes(x,y)                                         \
   (((((((CT(x,0)<<2)|(CT(x,1)))<<2)|(CT(x,2)))<<2)|(CT(x,3)))    \
     >((((((CT(y,0)<<2)|(CT(y,1)))<<2)|(CT(y,2)))<<2)|(CT(y,3))))

#define CompareNodeLT_GT(x,y) \
  (DBLW(j[x].l[0])>DBLW(j[y].l[0]))||((DBLW(j[x].l[0])==DBLW(j[y].l[0]))&&CompareTypes(x,y))
#define CompareType(x,y)                       \
  (IS_A_NODE(n[nn].s[x])?0:n[nn].s[x])         \
        -(IS_A_NODE(n[nn].s[y])?0:n[nn].s[y])

#define CompareLength(x,y) j[nn].l[x]-j[nn].l[y]

#define SWAPLINKS(x,y)                               \
{ /* Swap the two links */                           \
  STOPDATA tsd;                                        \
  EDGE te;                                        \
  if (IS_A_NODE(sd[nnA][y].ns))                         \
  {                                                  \
    sd[sd[nnA][y].ns][sd[nnA][y].ne].ne=x;                   \
  };                                                 \
  tsd=sd[nnA][x]; sd[nnA][x]=sd[nnA][y]; sd[nnA][y]=tsd; \
  te=ed[nnA].e[x]; ed[nnA].e[x]=ed[nnA].e[y]; ed[nnA].e[y]=te; \
  if (IS_A_NODE(sd[nnA][y].ns))                         \
  {                                                  \
    sd[sd[nnA][y].ns][sd[nnA][y].ne].ne=y;                   \
  };                                                 \
};
#define CTLGAS(x,y)\
{\
  if (ed[nnA].e[x]>ed[nnA].e[y]) SWAPLINKS(x,y)\
};

void CON::SortEdgesByGroup(int nnA)
{
  CTLGAS(0,2)
  CTLGAS(1,3)
  CTLGAS(2,3)
  CTLGAS(0,1)
  CTLGAS(1,2)
}

struct GROUPDESC
{
  ui16 firstJoint;
  ui16 numJoint;
};




#define _LT(ji) m_links[m_joints[ji]^1]
#define _GN(li) m_jointInfo[(unsigned)_LT(li)/JS].groupNum
#define GN(lt) m_jointInfo[(unsigned)lt/JS].groupNum


#define G1(jix,jiy) \
    ((NJL_i(jix)<4)||(ltx=_LT(jix),(ltx<0))||(lty=_LT(jiy),ngx=GN(ltx),ngy=GN(lty),ngx==ngy))?false:(ngx>ngy)
#define G2(jix,jiy) \
    ((ltx=_LT(jix),(ltx<0))||(lty=_LT(jiy),ngx=GN(ltx),ngy=GN(lty),ngx==ngy))?(G1(jix+1,jiy+1)):(ngx>ngy)
#define G3(jix,jiy) \
    ((ltx=_LT(jix),(ltx<0))||(lty=_LT(jiy),ngx=GN(ltx),ngy=GN(lty),ngx==ngy))?(G2(jix+1,jiy+1)):(ngx>ngy)
#define G4(jix,jiy) \
    ((ltx=_LT(jix),(ltx<0))||(lty=_LT(jiy),ngx=GN(ltx),ngy=GN(lty),ngx==ngy))?(G3(jix+1,jiy+1)):(ngx>ngy)
#define NGGT(jix,jiy) (G4(jix,jiy))



#define SORT_JOINT_GROUP(GTFUNC)                         \
{                                                        \
  int i, j, k, ns, f, l, jix, jiy;                       \
  f = groups[currentGroup].firstJoint;                   \
  l = f + groups[currentGroup].numJoint;                 \
  for (ns=groups[currentGroup].numJoint/2; ns>0; ns/=2)  \
  {                                                      \
    for (i=f; i<f+ns; i++)                               \
    {                                                    \
      for (j=i; j<l-ns; j+=ns)                           \
      {                                                  \
        k = j;                                           \
        do                                               \
        {                                                \
          jix = JS*k; jiy = JS*(k+ns);                   \
          if (!GTFUNC(jix,jiy)) break;                   \
          SWAPJOINT_N(k, k+ns);                          \
          k-=ns;                                         \
        } while (k>=f);                                  \
      };                                                 \
    };                                                   \
  };                                                     \
};

#define BREAK_JOINT_GROUP(GTFUNC)                        \
{                                                        \
  int i, j, f, l,jix,jiy;                                \
  f = groups[currentGroup].firstJoint;                   \
  l = f + groups[currentGroup].numJoint - 1;             \
  for (i=f+1; i<=l; i++)                                 \
  { /*Find first greater joint   */                      \
    jix=JS*i; jiy=jix-JS;                                \
    if (!GTFUNC(jix,jiy)) continue;                      \
    groupBroken = true;                                  \
    groups[currentGroup].numJoint = (ui16)(i-f);         \
    do                                                   \
    {                                                    \
      m_jointInfo[i].groupNum = (ui16)numGroup;          \
      /* Find extent of this value */                    \
      jiy = i*JS;                                        \
      for (j=i+1; j<=l; j++)                             \
      {                                                  \
        jix = j*JS;                                      \
        if (GTFUNC(jix,jiy)) break;                      \
        m_jointInfo[j].groupNum = (ui16)numGroup;        \
      };                                                 \
      groups[numGroup].firstJoint = (ui16)i;             \
      groups[numGroup].numJoint = (ui16)(j-i);           \
      numGroup++;                                        \
      i = j;                                             \
    } while (i<=l);                                      \
  };                                                     \
}


#define FIX_NEIGHBOR(nnA,en,gn)                            \
{                                                          \
  if (IS_A_NODE(sd[nnA][en].ns))                           \
  {                                                        \
    ed[sd[nnA][en].ns].e[sd[nnA][en].ne].ng = (STOPNUM)gn; \
    SortEdgesByGroup(sd[nnA][en].ns);                      \
  };                                                       \
}

void CON::FixNeighborGroups(int nnA, int gn)
{
  FIX_NEIGHBOR(nnA,0,gn);
  FIX_NEIGHBOR(nnA,1,gn);
  FIX_NEIGHBOR(nnA,2,gn);
  FIX_NEIGHBOR(nnA,3,gn);
}

  //First sort the neighbors in each node by their distance and type.
  //We need to initialize the group numbers to zero.  They may
  //contain total giberish from their recent history.
void CON::Canon(int nnFirst, int nnLast)
{
  int nn, nnA;
//  DUMP("Enter Canon",0,0,0,nnFirst,nnLast);
  for (nn=nnFirst; nn<nnLast; nn++) 
  {
    nnA = nd[nn].ix;
    nd[nnA].gn = 0;
    if (IS_A_NODE(sd[nnA][0].ns)) ed[nnA].e[0].ng = 0; 
    if (IS_A_NODE(sd[nnA][1].ns)) ed[nnA].e[1].ng = 0; 
    if (IS_A_NODE(sd[nnA][2].ns)) ed[nnA].e[2].ng = 0; 
    if (IS_A_NODE(sd[nnA][3].ns)) ed[nnA].e[3].ng = 0; 
    SortEdgesByGroup(nnA);
  };
//  DUMP("After Sorting Edges",0,0,0,nnFirst,nnLast);
  {
    bool brokenGroup, arbitrary;
    int arbitraryGn;
    int numGroup;
    unsigned int firstNodeInGroup[256];
    unsigned int lastNodeInGroup[256];
    int groupNumbers[256];
    firstNodeInGroup[0] = nnFirst;
    lastNodeInGroup[0] = nnLast-1;
    numGroup = 1;
    //There.  Everything initialized.  Now repeat for 
    //each group in an attempt to break the groups into
    //smaller groups.
    for (arbitrary=false, arbitraryGn=-1; numGroup>0; arbitrary=true)
    {
      do
      {
        brokenGroup = false;
        for (int gn=0; gn<numGroup; gn++)
        {
          // Process one group.  See if we can break it into
          // two or more parts.
          //
          int f, l, l1, nn, currentGroupNum;
          f = firstNodeInGroup[gn];
          l = lastNodeInGroup[gn];
          if (f == l)
          {
            numGroup--;
            firstNodeInGroup[gn] = firstNodeInGroup[numGroup];
            lastNodeInGroup[gn] = lastNodeInGroup[numGroup];
            gn--;
            arbitraryGn = -1;
            continue;
          };
          l1 = l+1;
          //First we need to sort the nodes in this group. 
          {
            int i, j, k, n, ns;
            n = l + 1 - f;
            for (ns = n/2; ns>0; ns/=2)
            {
              for (i=f; i<f+ns; i++)
              {
                for (j=i; j<=l-ns; j+=ns)
                {
                  if (ed[nd[j].ix] < ed[nd[j+ns].ix])
                  {
                    k=j;
                    do
                    {
                      SwapNode(k, k+ns);
                      k-=ns;
                    } while ((k>=(signed)i) && (ed[nd[k].ix] < ed[nd[k+ns].ix]));
                  };
                };
              };
            };
          };

//          DUMP("Canon process sorted group",gn,0,0,firstNodeInGroup[gn],lastNodeInGroup[gn]+1);
          // Assign new group numbers.
          currentGroupNum = f-nnFirst;
          for (nn=f+1; nn<=l; nn++)
          {
            if (ed[nd[nn].ix] < ed[nd[currentGroupNum+nnFirst].ix]) 
            {
              currentGroupNum = nn-nnFirst; 
              //brokenGroup = true;
            };
            groupNumbers[nn] = currentGroupNum;
            if (!arbitrary) nd[nd[nn].ix].gn = (GROUPNUM)currentGroupNum;
          };
          // Now we update our neighbors and create new groups
          currentGroupNum = f-nnFirst;
          for (nn=f+1; nn<=l; nn++)
          {
            if (groupNumbers[nn] != currentGroupNum)
            {
              //Move the previous group to a new group unless it
              //consists of a single node;
              if (currentGroupNum != nn-nnFirst-1)
              {
                firstNodeInGroup[numGroup] = firstNodeInGroup[gn];
                lastNodeInGroup[numGroup] = nn-1;
                numGroup++;
              };
              firstNodeInGroup[gn] = nn;
              currentGroupNum = nn-nnFirst;
              brokenGroup = true;
              arbitraryGn = -1;
            };
            if (groupNumbers[nn] != f-nnFirst) FixNeighborGroups(nd[nn].ix,groupNumbers[nn]);
          };
          if (firstNodeInGroup[gn] == lastNodeInGroup[gn])
          {
            numGroup--;
            firstNodeInGroup[gn] = firstNodeInGroup[numGroup];
            lastNodeInGroup[gn] = lastNodeInGroup[numGroup];
            arbitraryGn = -1;
            gn--;
          };
        };
      } while (brokenGroup);
      if (arbitraryGn >= 0)
      {
        numGroup--;
        firstNodeInGroup[arbitraryGn] = firstNodeInGroup[numGroup];
        lastNodeInGroup[arbitraryGn] = lastNodeInGroup[numGroup];
        arbitraryGn = -1;
      };
      if (numGroup > 0)
      { // We have determined which nodes are indistinguishable.
        // Now we will assign break one node from the
        // first such group.
        int largestGroup;
        int gn, gn0, f, l;
        largestGroup = firstNodeInGroup[0];
        gn0 = 0;
        for (gn=1; gn<numGroup; gn++)
        {
          //Search for the largest group number.
          f = firstNodeInGroup[gn];
          if (nd[nd[f].ix].gn > largestGroup)
          {
            largestGroup = nd[nd[f].ix].gn;
            gn0 = gn;
          };
        };
        //Now assign new group number to the last member
        //of the group.
//        DUMP("Apply Arbitrary numbers",gn0,0,0,firstNodeInGroup[gn0],lastNodeInGroup[gn0]+1);
        f = firstNodeInGroup[gn0];
        l = lastNodeInGroup[gn0];
//        for (nn=f; nn<=l; nn++)
        {
          FixNeighborGroups(nd[l].ix, l-nnFirst);
        };
        lastNodeInGroup[gn0]--;
        arbitraryGn = gn0;
      };
    };// while (numGroup > 0);
 //   for (nn=nnFirst; nn<nnLast; nn++)
 //   {
 //     nd[nd[nn].ix].gn = (STOPNUM)groupNumbers[nn];
 //   };
  };
#if defined(SW_DUMP) || defined(NS_DUMP)
//  DUMP("Leaving Canon",0,0,0,nnFirst,nnLast);
#endif
//Don't check here because we don't know whether to call
// CON::CheckEverything or NIMCON::CheckEverything.
//#ifdef _CHECK
//  CheckEverything(nnFirst, nnLast, false);
//#endif
}

void CON::DoubleCheck(void)
{
}



