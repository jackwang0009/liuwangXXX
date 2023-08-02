// Enumerate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\DotBoxLib\Combinations.h"



COMBINATIONS::COMBINATIONS(void)
{
  int r, c;
  for (r=0; r<=10; r++)
  {
    table[r][0] = 0;
  };
  for (c=0; c<=40; c++)
  {
    table[0][c] = 1;
  };
  for (r=1; r<=10; r++)
  {
    for (c=1; c<=40; c++)
    {
      table[r][c] = table[r][c-1] + table[r-1][c-1];
    };
  };
}


unsigned int COMBINATIONS::Index(ui64 p, int nOneBits, int nTotalBits)
{
  unsigned int result = 0;
  ui64 mask = 1I64 << nTotalBits;
  while (nOneBits > 0)
  {
    mask >>= 1;
    nTotalBits--;
    if ((p & mask) != 0)
    {
      result += table[nOneBits][nTotalBits];
      nOneBits--;
    };
  };
  return result;
}

ui64 COMBINATIONS::Combination(unsigned int index, int nOneBits, int nTotalBits)
{
  ui64 result = 0;
  int i, j=nTotalBits;
  for (i=0; i<nTotalBits; i++)
  {
    j--;
    result <<= 1;
    if (index >= table[nOneBits][j])
    {
      index -= table[nOneBits][j];
      result |= 1;
      nOneBits--;
    };
  };
  return result;
}



