#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"

/*
void COS::SwapJoint_n(int jn1, int jn2)
{
  int n, j;
  ui32 ji1, ji2;
  ui32 tempDWORD;
  ji1 = JS*jn1;
  ji2 = JS*jn2;
  SWAPDBL(m_joints[ji1],m_joints[ji2])
  SWAPDBL(m_joints[ji1+2], m_joints[ji2+2])
  SWAPDBL(m_jointInfo[jn1], m_jointInfo[jn2])
  n = NJL_n(jn1);
  for (j=0; j<n; j++, ji1++)
  {
    m_links[m_joints[ji1]] = (LINKTYPE)ji1;
  };
  n = NJL_n(jn2);
  for (j=0; j<n; j++, ji2++)
  {
    m_links[m_joints[ji2]] = (LINKTYPE)ji2;
  };
}
*/
/*
void COS::SwapJoint_n(int jn1, int jn2)
{
  int n, j;
  ui32 ji1, ji2;
  ui32 tempDWORD;
  ji1 = JS*jn1;
  ji2 = JS*jn2;
  SWAPDBL(m_joints[ji1],m_joints[ji2])
  SWAPDBL(m_joints[ji1+2], m_joints[ji2+2])
  SWAPDBL(m_jointInfo[jn1], m_jointInfo[jn2])
  n = NJL_n(jn1);
  for (j=0; j<n; j++, ji1++)
  {
    m_links[m_joints[ji1]] = (LINKTYPE)ji1;
  };
  n = NJL_n(jn2);
  for (j=0; j<n; j++, ji2++)
  {
    m_links[m_joints[ji2]] = (LINKTYPE)ji2;
  };
}
*/



/*
void COS::SwapLink_n(int ln1, int ln2)
{
  ui32 tempDWORD;
  int li1, li2;
  li1 = LS*ln1;
  li2 = LS*ln2;
  SWAPDBL (m_links[li1]  ,m_links[li2]);
  SWAPDBL (m_links[li1+2],m_links[li2+2]);
  //m_linkLengths[linkNum1] ^= m_linkLengths[linkNum2];
  //m_linkLengths[linkNum2] ^= m_linkLengths[linkNum1];
  //m_linkLengths[linkNum1] ^= m_linkLengths[linkNum2];
  if (m_links[li1] != GROUND)
  {
    m_joints[m_links[li1]] = (JOINTTYPE)li1;
  };
  li1++;
  if (m_links[li1] != GROUND)
  {
    m_joints[m_links[li1]] = (JOINTTYPE)li1;
  };
  if (m_links[li2] != GROUND)
  {
    m_joints[m_links[li2]] = (JOINTTYPE)li2;
  };
  li2++;
  if (m_links[li2] != GROUND)
  {
    m_joints[m_links[li2]] = (JOINTTYPE)li2;
  };
}
*/


void CON::Compress(void)
{
  //Discaard any unused nodes.
  int nn;
  static int count=0;
  count++;
#ifdef NS_DUMP
//  DUMP("About to Compress",count,0,0,0,m_numNode);
#endif
  for (nn=0; nn<m_numNode; nn++)
  {
    if (nd[nd[nn].ix].ec == 0)
    {
      DeleteNode(nn);
      nn--;
    };
  };
}
