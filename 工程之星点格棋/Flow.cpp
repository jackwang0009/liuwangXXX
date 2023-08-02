#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"


struct R
{
  int c[4];
  int *d;
  int *(s[4]);
};

 
void FLOW::ComputeFlow(const DOTS_AND_BOXES * pDB)
{
  int width, height;
  int row, col;
  int zero = 0;
  int w;
  int e;
  R r[MAX_WIDTH*MAX_HEIGHT*2];
  int numR;
  width  = pDB->Width();
  height = pDB->Height();
  numR = 0;
  for (col=0; col<width; col++)
  {
    for (row=0; row<height; row++)
    {
      m_v[col][row][0] = 1000*row/(height-1);
      m_v[col][row][1] = 1000*col/(width-1);
      if ((row>0)&&(row<height-1))
      {
        r[numR].d = &m_v[col][row][0];
          //
          //UP
        r[numR].s[0] = &m_v[col][row-1][0];
        if (pDB->VLineType(col,row-1) == LINE_NONE)
        {
          r[numR].c[0] = 1;
        }
        else
        {
          r[numR].c[0] = 5;
        };
          //
          //RIGHT
        if (col<width-1)
        {
          r[numR].s[1] = &m_v[col+1][row][0];
          if (pDB->HLineType(col,row) == LINE_NONE)
          {
            r[numR].c[1] = 1;
          }
          else
          {
            r[numR].c[1] = 5;
          };
        }
        else
        {
          r[numR].s[1] = &zero;
          r[numR].c[1] = 0;
        };
          //
          //DOWN
        r[numR].s[2] = &m_v[col][row+1][0];
        if (pDB->VLineType(col,row) == LINE_NONE)
        {
          r[numR].c[2] = 1;
        }
        else
        {
          r[numR].c[2] = 5;
        };
          //
          //LEFT
        if (col > 0)
        {
          r[numR].s[3] = &m_v[col-1][row][0];
          if (pDB->HLineType(col-1,row) == LINE_NONE)
          {
            r[numR].c[3] = 1;
          }
          else
          {
            r[numR].c[3] = 5;
          };
        }
        else
        {
          r[numR].s[3] = &zero;
          r[numR].c[3] = 0;
        };
        w = r[numR].c[0] + r[numR].c[1] + r[numR].c[2] + r[numR].c[3];
        r[numR].c[0] *= 1000000/w;
        r[numR].c[1] *= 1000000/w;
        r[numR].c[2] *= 1000000/w;
        r[numR].c[3] *= 1000000/w;
        numR++;
      };
      if ((col>0)&&(col<width-1))
      {
        r[numR].d = &m_v[col][row][1];
          //
          //UP
        if (row>0)
        {
          r[numR].s[0] = &m_v[col][row-1][1];
          if (pDB->VLineType(col,row-1) == LINE_NONE)
          {
            r[numR].c[0] = 1;
          }
          else
          {
            r[numR].c[0] = 5;
          };
        }
        else
        {
          r[numR].s[0] = &zero;
          r[numR].c[0] = 0;
        };
          //
          //RIGHT
        r[numR].s[1] = &m_v[col+1][row][1];
        if (pDB->HLineType(col,row) == LINE_NONE)
        {
          r[numR].c[1] = 1;
        }
        else
        {
          r[numR].c[1] = 5;
        };
          //
          //DOWN
        if (row < height-1)
        {
          r[numR].s[2] = &m_v[col][row+1][1];
          if (pDB->VLineType(col,row) == LINE_NONE)
          {
            r[numR].c[2] = 1;
          }
          else
          {
            r[numR].c[2] = 5;
          };
        }
        else
        {
          r[numR].s[2] = &zero;
          r[numR].c[2] = 0;
        };
          //
          //LEFT
        r[numR].s[3] = &m_v[col-1][row][1];
        if (pDB->HLineType(col-1,row) == LINE_NONE)
        {
          r[numR].c[3] = 1;
        }
        else
        {
          r[numR].c[3] = 5;
        };
        w = r[numR].c[0] + r[numR].c[1] + r[numR].c[2] + r[numR].c[3];
        r[numR].c[0] *= 1000000/w;
        r[numR].c[1] *= 1000000/w;
        r[numR].c[2] *= 1000000/w;
        r[numR].c[3] *= 1000000/w;
        numR++;
      };
    };
  };
  do
  {
    int i;
    e = 0;
    for (i=0; i<numR; i++)
    {
      int r0, c;
      r0 = *r[i].d;
      *r[i].d = (   *r[i].s[0] * r[i].c[0]
                  +  *r[i].s[1] * r[i].c[1]
                  +  *r[i].s[2] * r[i].c[2]
                  +  *r[i].s[3] * r[i].c[3]
                ) / 1000000;

      c = (r0 - *r[i].d);
      e += c * c;
    };
  } while (e > numR);
}