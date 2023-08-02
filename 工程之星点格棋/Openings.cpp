#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\Combinations.h"


extern unsigned int numOpenings5x5[11][4];
extern unsigned char *Openings5x5[11][4]; 

 
void Decode(ui64* &positions, 
            unsigned char *src, 
            int numPosition, 
            int nOneBits, 
            int nTotalBits) 
{
  int i, n, delta, index;
  int prevIndex;
  int byteIndex;
  COMBINATIONS combinations;
  positions = (ui64 *)malloc(numPosition * sizeof (ui64));
  if (positions == NULL)
  {
    die("Cannot allocate opening book space");
  };
  prevIndex = 0;
  byteIndex = 0;
  for (i=0; i<numPosition; i++)
  {
    n = (src[byteIndex] >> 6) & 3;
    delta = src[byteIndex] & 0x3f;
    byteIndex++;
    while (n > 0)
    {
      delta <<= 8;
      delta += src[byteIndex++];
      n--;
    };
    index = prevIndex + delta;
    prevIndex = index;
    positions[i] = combinations.Combination(index, nOneBits, nTotalBits);
  };
}

OPENINGp::OPENINGp(int width, int height, int numLine)
{
  int i;
  m_numLine = numLine;
  for (i=0; i<4; i++)
  {
    Decode(positions[i], 
           Openings5x5[numLine+1][i],
           numOpenings5x5[numLine+1][i],
           numLine+1,
           (width-1)*height + (height-1)*width);
    numPositions[i] = numOpenings5x5[numLine+1][i];
  };
}


OPENINGp::~OPENINGp(void)
{
  int i;
  for (i=0; i<4; i++)
  {
    if (positions[i] != NULL)
    {
      free(positions[i]);
      positions[i] = NULL;
    };
  };
}

