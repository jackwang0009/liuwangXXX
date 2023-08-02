#include "stdafx.h"
//#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"
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
int  cdExistsCalls;
int  cdExistsTrue;
int  totalcdCacheEntries;
int  cdHashCacheCrash;
int  cdCacheUpdate;

SWHASHTABLE::SWHASHTABLE(void)
{
#ifdef HASHSTATS
  pointsExistsCalls = 0;
  pointsExistsTrue = 0;
  totalPointsCacheEntries = 0;
  pointsHashCacheCrash = 0;
  pointsCacheUpdate = 0;
#endif
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
#ifdef HASHSTATS
  pointsExistsCalls++;
#endif
  while (hashId[index]!=0)
  {
    if (hashId[index] == hash)
    {
#ifdef HASHSTATS
      pointsExistsTrue++;
#endif
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
#ifdef HASHSTATS
      totalPointsCacheEntries--;
      pointsHashCacheCrash++;
#endif
      break;
    };
    if (i >= SWHASHSIZE) i -= SWHASHSIZE;
  };
#ifdef HASHSTATS
  totalPointsCacheEntries++;
#endif
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
#ifdef HASHSTATS
    pointsCacheUpdate++;
#endif
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



/*
CDHASHTABLE::CDHASHTABLE(void)
{
#ifdef HASHSTATS
  cdExistsCalls = 0;
  cdExistsTrue = 0;
  totalcdCacheEntries = 0;
  cdHashCacheCrash = 0;
  cdCacheUpdate = 0;
#endif
  hashId = (ui64 *)malloc(CDHASHSIZE * sizeof (hashId[0]));
  valuePairs = (CDHASHVALUE *)malloc(CDHASHSIZE * sizeof (valuePairs[0]));
  if ( (hashId == NULL) || (valuePairs == NULL) )
  {
    die("Cannot allocate hash table");
    return;
  };
  memset(hashId, 0, CDHASHSIZE*sizeof(hashId[0]));
  numEnt = 0;
}

CDHASHTABLE::~CDHASHTABLE(void)
{
  if (hashId != NULL) free(hashId);
  hashId = NULL;
  if (valuePairs != NULL) free(valuePairs);
  valuePairs = NULL;
}

void CDHASHTABLE::Value(int index, CDVALUE *value)
{
  value->player1Min = valuePairs[index].player1min;
  value->player2Min = valuePairs[index].player2min;
}

int CDHASHTABLE::Exists(ui64 hash)
{
  int j, index;
  index = (int)(hash%CDHASHSIZE); 
  j = 0;
#ifdef HASHSTATS
  cdExistsCalls++;
#endif
  while (hashId[index]!=0)
  {
    if (hashId[index] == hash)
    {
#ifdef HASHSTATS
      cdExistsTrue++;
#endif
      return index;
    };
    index += ((ui32)hash & 0xff) + 1;
    j++;
    if (j == CD_MAX_HASH_OFFSET) return -1;
    if (index >= CDHASHSIZE) index -= CDHASHSIZE;
  };
  return -1;
}
  
void CDHASHTABLE::EnterValue(ui64 hash, CDVALUE *value)
{
  int i, j, index;
  i = index = (int)(hash%CDHASHSIZE);
  j = 0;
  while (hashId[i]!=0)
  {
    if (hashId[i] == hash) break;
    i += ((ui32)hash & 0xff) + 1; 
    j++;
    if (j == CD_MAX_HASH_OFFSET) 
    {
      i=index;
#ifdef HASHSTATS
      totalcdCacheEntries--;
      cdHashCacheCrash++;
#endif
      break;
    };
    if (i >= CDHASHSIZE) i -= CDHASHSIZE;
  };
#ifdef HASHSTATS
  totalcdCacheEntries++;
#endif
  hashId[i] = hash;
  valuePairs[i].player1min = value->player1Min;
  valuePairs[i].player2min = value->player2Min;
}

void CDHASHTABLE::UpdateValue(ui32 index, ui64 hash, CDVALUE *value)
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
#ifdef HASHSTATS
    cdCacheUpdate++;
#endif
  };
}
*/
