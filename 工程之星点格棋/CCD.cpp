#include "stdafx.h"
#include "NewBoxes.h"
#include "..\DotBoxLib\DotsAndBoxes.h" 
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"


extern RRCC rrcc;

static int enableDump = 1;
int srchDepth;
static FILE *f = NULL;
bool started = false;
int count;

void CCDn::LogUiUiUI64(char *format, ui i1, ui i2, ui64 i3)
{
  if (enableDump == 0) return;
//  FILE *f;
  char prefix[81];
  int numPrefix;
  numPrefix = srchDepth*5;
  if (numPrefix >80) numPrefix = 80;
  memset(prefix, ' ', numPrefix);
  prefix[numPrefix] = 0;
  fprintf(f, "%s", prefix);
  fprintf(f, format, i1, i2, i3);
  fprintf(f,"\n");
}


RRCC::RRCC(void)
{
  int i, j;
  for (i=0; i<RRCC_MaxWidth; i++)
  {
    for (j=0; j<RRCC_MaxHeight; j++)
    {
      rotTable[i][j] = NULL;
      refTable[i][j] = NULL;
      capturesTable[i][j] = NULL;
      cornersTable[i][j] = NULL;
      refCount[i][j] = 0;
    };
  };
}

RRCC::~RRCC(void)
{
  int i, j;
  for (i=0; i<RRCC_MaxWidth; i++)
  {
    for (j=0; j<RRCC_MaxHeight; j++)
    {
      if (rotTable[i][j] != NULL) free(rotTable[i][j]);
      rotTable[i][j] = NULL;
      if (refTable[i][j] != NULL) free(refTable[i][j]);
      refTable[i][j] = NULL;
      if (capturesTable[i][j] != NULL) free(capturesTable[i][j]);
      capturesTable[i][j] = NULL;
      if (cornersTable[i][j] != NULL) free(cornersTable[i][j]);
      cornersTable[i][j] = NULL;
      refCount[i][j] = 0;
    };
  };
}


void BuildRefTransp(int width, int height, int *transp)
{
  int lineNum, reflectedLineNum;
  int col, row;
  for (col=0; col<width; col++)
  {
    for (row=0; row<height; row++)
    {
      if (col < width-1) //If horizontal line attached
      {
        lineNum = row*(2*width-1) + col;
        reflectedLineNum = row*(2*width-1) + width - 2 - col;
        transp[lineNum] = reflectedLineNum;
      };
      if (row < height-1) //If vertical line attached
      {
        lineNum = row*(2*width-1) + width - 1 + col;
        reflectedLineNum = (row+1)*(2*width-1) - 1 - col;
        transp[lineNum] = reflectedLineNum;
      };
    };
  };
}

void BuildRotTransp(int width, int height, int *transp)
{
  int lineNum, rotatedLineNum;
  int col, row, x, y;
  for (col=0; col<width; col++)
  {
    for (row=0; row<height; row++)
    {
      if (col < width-1) //If horizontal line attached
      {
        lineNum = row*(2*width-1) + col;
        if (width == height) 
        { // rotate 90 degrees
          x=height-row-1;
          y=col;
          rotatedLineNum = y*(2*width-1)+width-1+x;
        }
        else
        { // rotate 180 degrees
          x = width-1-col;
          y = height-1-row;
          rotatedLineNum = y*(2*width-1)+x;
        };
        transp[lineNum] = rotatedLineNum;
      };
      if (row < height-1) //If vertical line attached
      {
        lineNum = row*(2*width-1) + width - 1 + col;
        if (width == height) 
        { // rotate 90 degrees
          x = width-row-2;
          y = col;
          rotatedLineNum = y*(2*width-1) + x;
        }
        else
        { // rotate 180 degrees
          x = width-1-col;
          y = height-1-row;
          rotatedLineNum = y*(2*width-1) + width-1 + x;
        };
        transp[lineNum] = rotatedLineNum;
      };
    };
  };
  //Fix up the corners
  if (width == height)
  {
    // 90-degree rotation
    transp[0] = width-2;
    // transp[width-2] is already OK
    transp[width-1] = 2*width-2;
    //transp[2*width-2] is already OK
    //transp[(height-2)*(2*width-1)+width-2] is already OK
    transp[(height-1)*(2*width-1)-1] = (height-2)*(2*width-1)+width-1;
    //transp[(height-1)*(2*width-1)] is already OK
    transp[(height-1)*(2*width-1)+width-2] = (height-1)*(2*width-1);
  }
  else
  {
    //180-degree rotation
    transp[0] = (height-1)*(2*width-1)-1;
    transp[width-2] = (height-2)*(2*width-1)+width-1;
    transp[width-1] = (height-1)*(2*width-1)+width-2;
    transp[2*width-2] = (height-1)*(2*width-1);
    transp[(height-2)*(2*width-2)+width-1] = width-2;
    transp[(height-1)*(2*width-1)-1] = 0;
    transp[(height-1)*(2*width-1)] = 2*width-2;
    transp[(height-1)*(2*width-1)+width-2] = width-1;
  };
}


void RRCC::BuildRefRotTables(int width, int height)
{
  int rotateTransp[64]; //rotate transpositions
  int reflectTransp[64]; //reflection transpositions
  int numLine = (width-1)*height + (height-1)*width;
  int i, j;
  BuildRefTransp(width, height, reflectTransp);
  BuildRotTransp(width, height, rotateTransp);
  rotTable[width][height] = (ui64 *)malloc(4*65536*sizeof(ui64));
  refTable[width][height] = (ui64 *)malloc(4*65536*sizeof(ui64));
  for (i=0; i<4; i++)
  { 
    for (j=0; j<65536; j++)
    {
      ui64 rot = 0;
      ui64 ref = 0;
      int j1 = j;
      for (int k=0; k<16; k++, j1>>=1)
      {
        if (16*i+k >= numLine) break;
        if (j1 & 1)
        {
          rot |= 1I64<<rotateTransp[16*i+k];
          ref |= 1I64<<reflectTransp[16*i+k];
        };
      };
      rotTable[width][height][4*j+i] = rot;
      refTable[width][height][4*j+i] = ref;
    };
  };
}


int dot2HLine(int col, int row)
{
  if (row < 0) return -1;
  if (col < 0) return -1;
  if (row > 4) return -1;
  if (col > 3) return -1;
  return row*9 + col;
}

int dot2VLine(int col, int row)
{
  if (row < 0) return -1;
  if (col < 0) return -1;
  if (row > 3) return -1;
  if (col > 4) return -1;
  return row*9 + col + 4;
}

/*
RRCCn::RRCCn(void)
{
}

RRCCn::~RRCCn(void)
{
}
*/
void RRCC::CaptureEntry(int width, int height, int dir, int *line)
{
  ui64 m;
  if (line[0] < 0) return;
  if (line[1] < 0) return;
  if (line[2] < 0) return;
  if (line[3] < 0) return;
  m =   (1I64 << line[0])
      | (1I64 << line[1])
      | (1I64 << line[2])
      | (1I64 << line[3]);
  capturesTable[width][height][line[0]*2 + dir] = m;
}



void RRCC::BuildCapturesTable(int width, int height)
{
  int i, j, row, col, line[4], numLine;
  numLine = (width-1)*height + (height-1)*width;
  capturesTable[width][height] = (ui64 *)malloc(2*numLine*sizeof(ui64));
  for (i=0; i<numLine; i++)
  {
    for (j=0; j<2; j++)
    {
      capturesTable[width][height][i*2 + j] = 0xffffffffffffffff;
    };
  };
  for (row=0; row<height; row++)
  {
    for (col=0; col<width; col++)
    {
      //The two boxes associate with the horizontal line at (col, row);
      line[0] = dot2HLine(col, row);
      line[1] = dot2VLine(col, row-1);
      line[2] = dot2HLine(col, row-1);
      line[3] = dot2VLine(col+1, row-1);
      CaptureEntry(width, height, 0, line);
      line[1] = dot2VLine(col, row);
      line[2] = dot2HLine(col, row+1);
      line[3] = dot2VLine(col+1, row);
      CaptureEntry(width, height, 1, line);
      //The two boxes associate with the vertical line at (col, row);
      line[0] = dot2VLine(col, row);
      line[1] = dot2HLine(col-1, row);
      line[2] = dot2VLine(col-1, row);
      line[3] = dot2HLine(col-1, row+1);
      CaptureEntry(width, height, 0, line);
      line[1] = dot2HLine(col, row);
      line[2] = dot2VLine(col+1, row);
      line[3] = dot2HLine(col, row+1);
      CaptureEntry(width, height, 1, line);
    };
  };
}


void RRCC::AddCorner(int width, int height, int corner, int line1, int line2)
{
  cornersTable[width][height][2*corner+0] = 1I64 << line2;
  cornersTable[width][height][2*corner+1] = (1I64 << line1) | (1I64 << line2);
}

void RRCC::BuildCornersTable(int width, int height)
{
  ASSERT(cornersTable[width][height] == NULL);
  cornersTable[width][height] = (ui64 *)malloc(8 * sizeof (ui64));
  AddCorner(width, height, 0, 0,                              width-1); // Line numbers
  AddCorner(width, height, 1, width-2,                        2*width-2);
  AddCorner(width, height, 2, (height-2)*(2*width-1)+width-1, (height-1)*(2*width-1));
  AddCorner(width, height, 3, (height-1)*(2*width-1)-1,       (height-1)*(2*width-1)+width-2);
}

void RRCC::IncrementReferenceCount(int width, int height)
{
  if (   (width > RRCC_MaxWidth)
      || (height > RRCC_MaxHeight))
  {
    die("Too Large");
  };
  refCount[width][height]++;
  if (refCount[width][height] != 1) return;;
  BuildCapturesTable(width, height);
  BuildCornersTable(width, height);
  BuildRefRotTables(width, height);
  return;
}

void RRCC::DecrementReferenceCount(int width, int height)
{
  ASSERT(refCount[width][height] > 0);
  refCount[width][height]--;
  if (refCount[width][height] == 0)
  {
    free(rotTable[width][height]);      rotTable[width][height] = NULL;
    free(refTable[width][height]);      refTable[width][height] = NULL;
    free(capturesTable[width][height]); capturesTable[width][height] = NULL;
    free(cornersTable[width][height]);  cornersTable[width][height] = NULL;
  };
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


RRCCp::RRCCp(int width, int height)
{
  m_width = width;
  m_height = height;
  rrcc.IncrementReferenceCount(width, height);
  m_rotTable = rrcc.GetRotTable(width, height);
  m_refTable = rrcc.GetRefTable(width, height);
  m_capturesTable = rrcc.GetCapturesTable(width, height);
  m_cornersTable = rrcc.GetCornersTable(width, height);
}

RRCCp::~RRCCp(void)
{
  rrcc.DecrementReferenceCount(m_width, m_height);
}

ui64 RRCCp::Rotate(ui64 v)
{
  ui16 *p;
  p = (ui16 *)&v;
  return   m_rotTable[4*p[0]+0]
         | m_rotTable[4*p[1]+1]
         | m_rotTable[4*p[2]+2]
         | m_rotTable[4*p[3]+3];
}

ui64 RRCCp::Reflect(ui64 v)
{
  ui16 *p;
  p = (ui16 *)&v;
  return   m_refTable[4*p[0]+0]
         | m_refTable[4*p[1]+1]
         | m_refTable[4*p[2]+2]
         | m_refTable[4*p[3]+3];
}

ui64 RRCCp::NextRotRef(ui64 pos, int n)
{
  if (m_width == m_height)
  {
    switch(n)
    {
    case 0:
    case 4:
      return Reflect(pos);
    default:
      return Rotate(pos);
    };
  };
  ImplementationError("Illegal rotation/reflection");
  return 0;
}

ui64 RRCCp::CornerCanon(ui64 pos)
{
  int i;
  ui64 v = pos;
  for (i=0; i<4; i++)
  {
    if ((v & m_cornersTable[2*i+1]) == m_cornersTable[2*i])
    {
      v ^= m_cornersTable[2*i+1];
    };
  };
  return v;
}


bool RRCCp::IsCapture(ui64 pos, int line)
{
  ui64 newPos;
  newPos = pos | (1I64 << line);
  if ((m_capturesTable[2*line+0] & newPos) == m_capturesTable[2*line+0]) return true;
  if ((m_capturesTable[2*line+1] & newPos) == m_capturesTable[2*line+1]) return true;
  return false;
}


void CCDn::Canon(void)
{
  int i, n;
    ui64 v;
  m_pos = m_prar->CornerCanon(m_pos);
  n = NumRotRef();
  v = m_pos;
  for (i=1; i<n; i++)
  {
    v = m_prar->NextRotRef(v, i);
    if (v < m_pos) m_pos = v;
  };
}

CCDn::CCDn(void)
{
}

CCDn::~CCDn(void)
{
}


CCDp::CCDp(int width, int height)
{
  m_width = width;
  m_height = height;
  m_prar = new RRCCp(width, height);
}

CCDp::~CCDp(void)
{
  delete m_prar;
  m_prar = NULL;
}

CCDn& CCDp::operator =(const DOTS_AND_BOXES *db)
{
  POINT dot;
  int width, height, dotnum;
  height = db->Height();
  width = db->Width();
  dotnum = 0;
  m_pos = 0;
  for (dot.y=0; dot.y<height; dot.y++)
  {
    for (dot.x=0; dot.x<width; dot.x++)
    {
      if ((dot.x < width-1) && (db->HLineType(dot) != LINE_NONE))
      {
        m_pos ^= 1I64 << dotnum;
      };
      if ((dot.y < height-1) && (db->VLineType(dot) != LINE_NONE))
      {
        m_pos ^= 1I64 << (dotnum + width - 1);
      };
      dotnum++;
    };
    dotnum += width - 1;
  };
  return *this;
}

bool CCDp::IsCapture(int lineNum)
{
  return m_prar->IsCapture(m_pos, lineNum);
}

bool CCDp::GoodOpening(ui64 openingPosition, ui64 move)
{
  int i, n = NumRotRef();
  ui64 newPos = m_pos | move;
  if (newPos == openingPosition) 
  {
    return true;
  };
  for (i=1; i<n; i++)
  {
    newPos = m_prar->NextRotRef(newPos, i);
    if (newPos == openingPosition) 
    {
      return true;
    };
  };
  return false;
}

/*
CDVALUE CCD55::SimpleWin(void)
{
  CDVALUE cdvalue;
  cdvalue.player1Min = 0;
  return cdvalue;
}
*/



#ifdef CCD_DUMP
#define TRACEENTRY(myCount, pos)\
LogUiUiUI64("%d Enter CCD::Value depth=%d  pos=%I64x\n",myCount,srchDepth,pos);

#define TRACEEXIT(myCount, val)\
LogUiUiUI64("%d Exit CCD::Value=(%d,%d)\n",myCount, val.player1Min, val.player2Min);
#endif


/*
CDVALUE CCD55::Value(int myScore,
                     int hisScore)
{
  ui64 mask = 1I64;
  int line;
  int hashIndex;
  CDVALUE val, result;
#ifdef CCD_DUMP
  int myCount;
  srchDepth++;
  count++;
  myCount = count;
  if (myCount == 421)
  {
    int kkk=1;
  };
  TRACEENTRY(myCount, pos)
#endif
  if (pos == (1I64 << 40)) 
  {
    result.player1Min = result.player2Min = 0;
#ifdef CCD_DUMP
    TRACEEXIT(myCount, result)
    srchDepth--;
#endif
    return result;
  };
  result.player1Min = 0;
  result.player2Min = 17;
  Canon();
  //Time to look in the hash table.
  hashIndex = cdHashTable->Exists(pos);
  if (hashIndex >= 0)
  {
    cdHashTable->Value(hashIndex, &val);
    if (    (val.player1Min + myScore > 8)
         || (val.player2Min + hisScore > 8)
         || (val.player1Min + val.player2Min + myScore + hisScore == 16))
    {
#ifdef CCD_DUMP
      TRACEEXIT(myCount, val);
      srchDepth--;
#endif
      return val;
    };
  };
  for (line=0; (line<40) && (myScore+result.player1Min < 9); line++, mask<<=1)
  {
    if (line==0)continue;
    if (line==8)continue;
    if (line==39)continue;
    if (line==31)continue;
    if ((pos & mask) != 0)
    {
      CCD55 newpos(pos^mask);
      if ((capturesTable[0][line] & newpos.pos) == 0)
      {
        if ((capturesTable[1][line] & newpos.pos) == 0)
        {
          val = newpos.Value(myScore+2, hisScore);
          val.player1Min += 2;
        }
        else
        {
          val = newpos.Value(myScore+1, hisScore);
          val.player1Min += 1;
        };
      }
      else
      {
        if ((capturesTable[1][line] & newpos.pos) == 0)
        {
          val = newpos.Value(myScore+1, hisScore);
          val.player1Min += 1;
        }
        else
        {
          CDVALUE t;
          t = newpos.Value(hisScore, myScore);
          val.player1Min = t.player2Min;
          val.player2Min = t.player1Min;
        };
      };
      if (val.player1Min > result.player1Min) result.player1Min = val.player1Min;
      if (val.player2Min < result.player2Min) result.player2Min = val.player2Min;
      //if (CausesCapture(line)
    };
  };
  if (line != 40)
  {
    //We did not try all moves so we don't know the 
    result.player2Min = 0;
  };
  if (hashIndex >= 0)
  {
    cdHashTable->UpdateValue(hashIndex, pos, &result);
  }
  else
  {
    cdHashTable->EnterValue(pos, &result);
  };
#ifdef CCD_DUMP
  TRACEEXIT(myCount, result)
  srchDepth--;
#endif
  return result;
}
*/
