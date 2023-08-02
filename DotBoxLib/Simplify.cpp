#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"


int sumOfLinks[9] =
{
  0, 1, 2, 3, 3, 3, 3, 3, 3
};


void CON::DeleteNode(int nn)
{
  // You better have arranged that nothing points to
  // the node being deleted.
  m_numNode--;
  for (int i=nn; i<m_numNode; i++)
  {
    nd[i].ix = nd[i+1].ix;
    nd[nd[i].ix].os = (STOPNUM)i;
  };
}

void CON::AddNode(void)
{
  //This is not something we do very often.
  //We have to search for a place to put the new node.
  int nnA;
  for (nnA=0; nnA<MAX_NODES; nnA++)
  {
    if (   (nd[nnA].os >= m_numNode)
        || (nd[nd[nnA].os].ix != nnA))
    {
      ed[nnA].Clear();
      sd[nnA][0].Clear();
      sd[nnA][1].Clear();
      sd[nnA][2].Clear();
      sd[nnA][3].Clear();
      nd[nnA].ec = UNUSED;
      nd[nnA].gn = UNUSED;
      nd[nnA].os = (STOPNUM)m_numNode;
      nd[m_numNode].ix = (STOPNUM)nnA;
      m_numNode++;
    };
  };
  ImplementationError("No Room for Additional Node");
}

void CON::DeleteEdge(int nn, int en)
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
        if (nd[nnA].ec == 3)
        {
          int e1, e2, enB;
          e1 = en;
          e2 = edgeB;
          if (e1 > e2)
          {
            e1 = e2;
            e2 = en;
          };
          sd[nnA][e2] = sd[nnA][2];
          sd[nnA][2].Clear();
          ed[nnA].e[e2] = ed[nnA].e[2];
          ed[nnA].e[2].Clear();
          sd[nnA][e1] = sd[nnA][1];
          sd[nnA][1].Clear();
          ed[nnA].e[e1] = ed[nnA].e[1];
          ed[nnA].e[1].Clear();
          nd[nnA].ec = 0;
          // only edge 0 remains.
          switch (sd[nnA][0].ns)
          {
          case DEADEND:
            sd[nnA][0].ns = DE_DE;
            sd[nnA][0].ne = UNUSED;
            ed[nnA].e[0].ng = DE_DE;
            nd[nnA].gn = UNUSED;
            break;
          case GROUND:
            sd[nnA][0].ns = GND_GND;
            sd[nnA][0].ne = UNUSED;
            ed[nnA].e[0].ng = GND_GND;
            nd[nnA].gn = UNUSED;
            break;
          default: //Must be a node out there.
            //Change far node to point to dead-end and delete us.
            nnB = sd[nnA][0].ns;
            enB = sd[nnA][0].ne;
            sd[nnB][enB].ns = DEADEND;
            sd[nnB][enB].ne = UNUSED;
            ed[nnB].e[0].ng = DEADEND;
            DeleteNode(nn);
          };
#ifdef _CHECK
          CheckEverything(0,m_numNode,false);
#endif
          return;
        };
        ImplementationError("Deleting looping edge");
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
      if (CombineEdges(nd[nnB].os))
      {
        DeleteNode(nd[nnB].os);
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
    if (CombineEdges(nd[nnA].os))
    {
      DeleteNode(nd[nnA].os);
    };
  };
}


bool CON::CombineEdges(int nn)
{
  //Return true if the node is deleted.
  int nodeLeftA, nodeRightA, edgeLeft, edgeRight;
  int nodeCenterA;
  nodeCenterA = nd[nn].ix;
  if (nd[nodeCenterA].ec == 1)
  {
    nodeLeftA = sd[nodeCenterA][0].ns;
    if (IS_A_NODE(nodeLeftA))
    {
      //Simply set the left node to a DEADEND length +1.
      edgeLeft  = sd[nodeCenterA][0].ne;
      ed[nodeLeftA].e[edgeLeft].el++;
      ed[nodeLeftA].e[edgeLeft].ng = DEADEND;
      sd[nodeLeftA][edgeLeft].ns = DEADEND;
      sd[nodeLeftA][edgeLeft].ne = UNUSED;
      //DeleteNode(nn);
      return true;
    }
    else
    {
      if (nodeLeftA == DEADEND)
      {
        if (ed[nodeCenterA].e[0].el < 1) ImplementationError("DE_DE length 1");
        ed[nodeCenterA].e[0].ng = DE_DE;
        ed[nodeCenterA].e[0].el++;
        sd[nodeCenterA][0].ns = DE_DE;
        sd[nodeCenterA][0].ne = UNUSED;
        nd[nodeCenterA].ec = 0;
        return false;
      }
      else if (nodeLeftA == GROUND)
      {
        ed[nodeCenterA].e[0].el++;
        ed[nodeCenterA].e[0].ng = GND_DE;
        sd[nodeCenterA][0].ns = GND_DE;
        sd[nodeCenterA][0].ne = UNUSED;
        nd[nodeCenterA].ec = 0;
        return false;
      }
      else
      {
        ImplementationError("Not DEADEND or GROUND");
        return false;
      };
    };
  };
  if (nd[nodeCenterA].ec != 2) ImplementationError("CombineNodes ec!=2");
  nodeLeftA  = sd[nodeCenterA][0].ns;
  nodeRightA = sd[nodeCenterA][1].ns;
  edgeLeft   = sd[nodeCenterA][0].ne;
  edgeRight  = sd[nodeCenterA][1].ne;
  if (!IS_A_NODE(nodeLeftA))
  {
    if (IS_A_NODE(nodeRightA))
    {
      //Left connected to ground or deadend.
      //Right side connected to a node.
      //Connect the node on the right side to the left stop
      //and add 1 to the sum of the lengths.
      //Discard this node.

      sd[nodeRightA][edgeRight].ns = (STOPNUM)nodeLeftA;
      ed[nodeRightA].e[edgeRight].ng = (STOPNUM)nodeLeftA;
      sd[nodeRightA][edgeRight].ne = UNUSED;
      ed[nodeRightA].e[edgeRight].el 
        = (EDGELEN)(ed[nodeRightA].e[edgeRight].el + ed[nodeCenterA].e[0].el + 1);
      //DeleteNode(nn);
      return true;
    }
    else
    {
      //Both Left and right are non-nodes.  So this is an isolated
      //chain from GROUND-GROUND, DEADEND-GROUND, or DEADEND-DEADEND.
      if ((nodeLeftA==GROUND)&&(nodeRightA==GROUND))
      {
        int length;
        ed[nodeCenterA].e[0].ng = GND_GND;
        length = ed[nodeCenterA].e[0].el + ed[nodeCenterA].e[1].el + 1;
        ed[nodeCenterA].e[0].el = (EDGELEN)length;
        ed[nodeCenterA].e[1].Clear();
        sd[nodeCenterA][0].ne = UNUSED;
        sd[nodeCenterA][0].ns = GND_GND;
        sd[nodeCenterA][1].Clear();
        nd[nodeCenterA].ec = 0;
        return false;
      }
      else if(   (nodeLeftA==DEADEND)&&(nodeRightA==GROUND)
              || (nodeLeftA==GROUND)&&(nodeRightA==DEADEND) )
      {
        // A Node between GROUND and DEADEND
        int length;
        ed[nodeCenterA].e[0].ng = GND_DE;
        length = ed[nodeCenterA].e[0].el + ed[nodeCenterA].e[1].el + 1;
        ed[nodeCenterA].e[0].el = (EDGELEN)length;
        sd[nodeCenterA][0].ne = UNUSED;
        sd[nodeCenterA][0].ns = GND_DE;
        nd[nodeCenterA].ec = 0;
        ed[nodeCenterA].e[1].Clear();
        sd[nodeCenterA][1].Clear();
        return false;
      }
      else
      {
        int length;
        ed[nodeCenterA].e[0].ng = DE_DE;
        length = ed[nodeCenterA].e[0].el + ed[nodeCenterA].e[1].el + 1;
        ed[nodeCenterA].e[0].el = (EDGELEN)length;
        ed[nodeCenterA].e[1].Clear();
        sd[nodeCenterA][0].ne = UNUSED;
        sd[nodeCenterA][0].ns = DE_DE;
        sd[nodeCenterA][1].Clear();
        nd[nodeCenterA].ec = 0;
        return false;
      };
    };
  }
  else
  {
    if (  (nodeRightA == GROUND)
        ||(nodeRightA == DEADEND) )
    {
      //Left connected to another node, right to ground.
      //Use the Left link to connect the left joint to ground.
      //Then we can delete ourselves.
      sd[nodeLeftA][edgeLeft].ns = (STOPNUM)nodeRightA;
      sd[nodeLeftA][edgeLeft].ne = UNUSED;
      ed[nodeLeftA].e[edgeLeft].el 
         = (EDGELEN)(ed[nodeLeftA].e[edgeLeft].el + ed[nodeCenterA].e[1].el + 1);
      ed[nodeLeftA].e[edgeLeft].ng = (STOPNUM)nodeRightA;
      //DeleteNode(nn);
      return true;
    }
    else
    {
      //Both sides connected to another node.
      //Connect the node on the right side to the node on the left.
      //Discard this node and the left link.
      if (nodeRightA != nodeCenterA)
      {
        int length;
        //The two edges go to another node (not ourselves)..
        //m_links[indexLinkRight] = (LINKTYPE)indexJointLeft;
        sd[nodeRightA][edgeRight].ns = (STOPNUM)nodeLeftA;
        ed[nodeRightA].e[edgeRight].ng = (STOPNUM)nodeLeftA;
        sd[nodeRightA][edgeRight].ne = (EDGENUM)edgeLeft;
        sd[nodeLeftA][edgeLeft].ns   = (STOPNUM)nodeRightA;
        ed[nodeLeftA].e[edgeLeft].ng = (STOPNUM)nodeRightA;
        sd[nodeLeftA][edgeLeft].ne   = (EDGENUM)edgeRight;
        length = ed[nodeCenterA].e[0].el + ed[nodeCenterA].e[1].el + 1;
        ed[nodeLeftA].e[edgeLeft].el   = (EDGELEN)length;
        ed[nodeRightA].e[edgeRight].el = (EDGELEN)length;
        //DeleteNode(nn);
        return true;
      }
      else
      { // What we have here is a loop.  And we are the only node on the loop.
        // An isolated loop
        nd[nodeCenterA].ec = UNUSED;
        sd[nodeCenterA][0].ns = LOOP;
        sd[nodeCenterA][0].ne = UNUSED;
        sd[nodeCenterA][1].Clear();
        ed[nodeCenterA].e[0].ng = LOOP;
        ed[nodeCenterA].e[0].el++;
        ed[nodeCenterA].e[1].Clear();
        nd[nodeCenterA].ec = 0;
        return false;
      };
    };
  };
  return true;
}



// <--n--O--m-->   becomes   <--n+m--O-->

void CON::_FormChains(void)
{
  // Get rid of any joints with two links.
  int nodeCenterA, nn;
  for (nn=0; nn<m_numNode; nn++)
  {
    nodeCenterA = nd[nn].ix;
    if (nd[nodeCenterA].ec < 3)
    {
      if (CombineEdges(nn)) 
      {
        DeleteNode(nn);
        nn--;
      };
    };
  };
}

bool NIMCON::Simplify(int nnFirst, int nnLast)
{
  bool result = false;
  int nn, nnA, nnB, edgeB, ec, en;
  for (nn=nnFirst; nn<nnLast; nn++)
  {
    nnA = nd[nn].ix;
    if (IS_ISOLATED(sd[nnA][0].ns)) continue;
    ec = nd[nnA].ec;
    for (en=0; en<ec; en++)
    {
      if (ed[nnA].e[en].el >= 3)
      {
        if (IS_A_NODE(sd[nnA][en].ns))
        {
          //A long chain that we can snap.
          result = true;
          nnB = sd[nnA][en].ns;
          edgeB = sd[nnA][en].ne;
          sd[nnB][edgeB].ne = UNUSED;
          sd[nnB][edgeB].ns = GROUND;
          ed[nnB].e[edgeB].ng = GROUND;
          ed[nnB].e[edgeB].el = 3;
          sd[nnA][en].ne = UNUSED;
          sd[nnA][en].ns = GROUND;
          ed[nnA].e[en].ng = GROUND;
          ed[nnA].e[en].el = 3;
        }
        else
        {
          if (sd[nnA][en].ns != GROUND)
          {
            ImplementationError("Expected connection to GROUND");
          }
          else
          {
            if (ed[nnA].e[en].el > 3)
            {
              result = true;
              ed[nnA].e[en].el = 3;
            };
          };
        };
      };
    };
  };
  return result;
}
