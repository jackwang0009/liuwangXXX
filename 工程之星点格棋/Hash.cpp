#include "stdafx.h" 
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
//#include "NewBoxesDoc.h"
//#include "Process.h"
//#include "NewBoxesView.h"
//#include "Utility.h"

int nimExistsCalls;
int nimExistsTrue;
int totalNimHashEntries;
int nimHashCacheCrash;
int  pointsExistsCalls;
int  pointsExistsTrue;
int  totalPointsCacheEntries;
int  pointsHashCacheCrash;
int  pointsCacheUpdate;

SWHASHTABLE::SWHASHTABLE(void)
{
  pointsExistsCalls = 0;
  pointsExistsTrue = 0;
  totalPointsCacheEntries = 0;
  pointsHashCacheCrash = 0;
  pointsCacheUpdate = 0;
  hashId = (ui64 *)malloc(SWHASHSIZE * sizeof (hashId[0]));
  valuePairs = (SWHASHVALUE *)malloc(SWHASHSIZE * sizeof (valuePairs[0]));
  if ( (hashId == NULL) || (valuePairs == NULL) )
  {
    die("Cannot allocate hash table");
    return;
  };
  memset(hashId, 0, SWHASHSIZE*sizeof(hashId[0]));
  numEnt = 0;
}

SWHASHTABLE::~SWHASHTABLE(void)
{
  if (hashId != NULL) free(hashId);
  hashId = NULL;
  if (valuePairs != NULL) free(valuePairs);
  valuePairs = NULL;
}

void SWHASHTABLE::Value(int index, SWVALUE *value)
{
  value->player1Min = valuePairs[index].player1min;
  value->player2Min = valuePairs[index].player2min;
}

int SWHASHTABLE::Exists(ui64 hash)
{
  int j, index;
  index = (int)(hash%SWHASHSIZE); 
  j = 0;
  pointsExistsCalls++;
  while (hashId[index]!=0)
  {
    if (hashId[index] == hash)
    {
      pointsExistsTrue++;
      return index;
    };
    index += ((ui32)hash & 0xff) + 1;
    j++;
    if (j == SW_MAX_HASH_OFFSET) return -1;
    if (index >= SWHASHSIZE) index -= SWHASHSIZE;
  };
  return -1;
}
  
void SWHASHTABLE::EnterValue(ui64 hash, SWVALUE *value)
{
  int i, j, index;
  i = index = (int)(hash%SWHASHSIZE);
  j = 0;
  while (hashId[i]!=0)
  {
    if (hashId[i] == hash) break;
    i += ((ui32)hash & 0xff) + 1; 
    j++;
    if (j == SW_MAX_HASH_OFFSET) 
    {
      i=index;
      totalPointsCacheEntries--;
      pointsHashCacheCrash++;
      break;
    };
    if (i >= SWHASHSIZE) i -= SWHASHSIZE;
  };
  totalPointsCacheEntries++;
  hashId[i] = hash;
  valuePairs[i].player1min = value->player1Min;
  valuePairs[i].player2min = value->player2Min;
}

void SWHASHTABLE::UpdateValue(ui32 index, ui64 hash, SWVALUE *value)
{
  if (hashId[index] != hash)
  {
    hashId[index] = hash;
    valuePairs[index].player1min = value->player1Min;
    valuePairs[index].player2min = value->player2Min;
    return;
  };
  if (value->player1Min > valuePairs[index].player1min)
  {
    valuePairs[index].player1min = value->player1Min;
    if (value->player2Min > valuePairs[index].player2min)
    {
      valuePairs[index].player2min = value->player2Min;
    };
  }
  else
  {
    if (value->player2Min > valuePairs[index].player2min)
    {
      valuePairs[index].player2min = value->player2Min;
    };
    pointsCacheUpdate++;
  };
}



NIMHASHTABLE::NIMHASHTABLE(void)
{
#ifdef HASHSTATS
  totalNimHashEntries = 0;
  nimExistsCalls = 0;
  nimExistsTrue = 0;
  nimHashCacheCrash = 0;
#endif
  hashId = (ui64 *)malloc(NIMHASHSIZE * sizeof (hashId[0]));
  values = (NIMHASHVALUE *)malloc(NIMHASHSIZE * sizeof (values[0]));
  if ( (hashId == NULL) || (values == NULL) )
  {
    die("Cannot allocate nim hash table");
    return;
  };
  memset(hashId, 0, NIMHASHSIZE*sizeof(hashId[0]));
  numEnt = 0;
}

NIMHASHTABLE::~NIMHASHTABLE(void)
{
  if (hashId != NULL) free(hashId);
  hashId = NULL;
  if (values != NULL) free(values);
  values = NULL;
}


int NIMHASHTABLE::Exists(ui64 hash)
{
  int j, index;
  index = (int)(hash%NIMHASHSIZE); 
  j = 0;
  while (hashId[index]!=0)
  {
    if (hashId[index] == hash)
    {
#ifdef HASHSTATS
      nimExistsCalls++;
      nimExistsTrue++;
#endif
      return index;
    };
    index += ((int)hash & 0xff) + 1;
    j++;
    if (j == NIM_MAX_HASH_OFFSET)
    {
#ifdef HASHSTATS
      nimExistsCalls++;
#endif
      return -1;
    };
    if (index >= NIMHASHSIZE) index -= NIMHASHSIZE;
  };
#ifdef HASHSTATS
      nimExistsCalls++;
#endif
  return -1;
}
  
void NIMHASHTABLE::EnterValue(ui64 hash, NIMVALUE value)
{
  int i, j, index;
  i = index = (int)(hash%NIMHASHSIZE);
  j = 0;
  while (hashId[i]!=0)
  {
    if (hashId[i] == hash) 
    {
#ifdef HASHSTATS
      totalNimHashEntries--;
#endif
      break;
    };
    i += ((int)hash & 0xff) + 1; 
    j++;
    if (j == NIM_MAX_HASH_OFFSET) 
    {
      i=index;
#ifdef HASHSTATS
      totalNimHashEntries--;
      nimHashCacheCrash++;
#endif
      break;
    };
    if (i >= NIMHASHSIZE) i -= NIMHASHSIZE;
  };
#ifdef HASHSTATS
      totalNimHashEntries++;
#endif
  hashId[i] = hash;
  values[i] = value;
}

