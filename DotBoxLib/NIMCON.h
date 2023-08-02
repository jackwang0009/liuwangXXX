typedef ui8 STOPNUM;
typedef ui8 EDGENUM;
typedef ui8 EDGELEN;
typedef ui8 GROUPNUM;
typedef ui8 EDGECOUNT;

#define EDGETYPE ui8

const EDGETYPE UNUSED  = 0xff;
const ui16     UNUSED2 = (UNUSED<<8)|UNUSED;
const ui32     UNUSED4 = (((ui32)UNUSED2<<16)|(ui32)UNUSED2);
const ui64     UNUSED8 = ((ui64)UNUSED4<<32)|(ui64)UNUSED4;
const EDGETYPE LOOP    = 0xfe; // A loop has no stops! Only a length.
                               //   stops; just a length.
const EDGETYPE GND_GND = 0xfd; // A chain from GROUND to GROUND has no nodes as
                               //   stops; just a length.
const EDGETYPE GND_DE  = 0xfc; // A chain from GROUND to DEADEND has no nodes as
                               //   stops; just a length.
const EDGETYPE DE_DE   = 0xfb; // A chain from DEADEND to DEADEND has no nodes as
const EDGETYPE GROUND  = 0xfa;
const EDGETYPE DEADEND = 0xf9; // A coin at the end has only one string.

//LOONY2 and LOONY 4 are not arbitrary.  They are the negative of the
//number of coins our opponent will refuse to retain control.
const int LOONY2 = -2; //Loony move opponent can refuse two coins.
const int LOONY4 = -4; //Loony move opponent can refuse four coins.

#define IS_A_NODE(n) ((n) < 0xf9)
#define IS_ISOLATED(n) ((n) > GROUND)

#define JOINTTYPE ui16
#define BYTW(x) *((ui8 *)(&(x)))
#define DBLW(x) *((ui32 *)(&(x)))
//#define XORDBL(x,y) DBLW(x)^=DBLW(y);
//#define SWAPDBL(x,y) XORDBL(x, y) XORDBL(y,x) XORDBL(x,y)

#define SWAPDBL(x,y) tempDWORD=DBLW(x);DBLW(x)=DBLW(y);DBLW(y)=tempDWORD;
#define SWAPBYTE(x,y) tempBYTE=BYTW(x);BYTW(x)=BYTW(y);BYTW(y)=tempBYTE;
#define SWAPJOINT_N(x,y) if((unsigned)(x)!=(unsigned)(y))SwapJoint_n(x,y)
#define SWAPLINK_I(x,y) if ((x)!=(y))SwapLink_i(x,y)
#define SWAPLINK_N(i,j) if((unsigned)(i)!=(unsigned)(j))SwapLink_i(LS*(i),LS*(j))


enum MOVE_TYPE
{
  MT_NONE,
  MT_RANDOM,
  MT_OBVIOUSCAPTURE,
  MT_GIVENOTHING,
  MT_BREAKLOOPS,
  MT_LOOPDIVIDE,
  MT_DIVIDE,
  MT_GIVELITTLE,
  MT_GRABANYTHING,
  MT_SIMPLEWIN,
  MT_SIMPLETIE,
  MT_SIMPLEWINCAPTURE,
  MT_SIMPLEWINDECLINE,
  MT_NIMVAL,
  MT_NONIMVAL,
  MT_EDIT,
  MT_VERIFYNIMSTRINGVALUE,
  MT_OPENINGWIN,
  MT_OPENINGTIE,
  MT_MAXDECLINE,
  MT_MAXCAPTURE,
  MT_NIMVALCAPTURE,
  MT_NIMVALDECLINE
};


struct SWVALUE
{
  ui16      player1Min;
  ui16      player2Min;
  i32       bestMove;
  MOVE_TYPE moveType;
  int       numPossibleMove;
  bool      allNimvalComputed;
};

//#define NJL_n(jn) m_jointInfo[jn].numLink
//#define NJL_i(ji) NJL_n((unsigned)(ji)/JS)


//const int SWHASHSIZE = 8973203;

struct SWHASHVALUE
{
  ui16 player1min;
  ui16 player2min;
};

const int SW_MAX_HASH_OFFSET = 10;

class SWHASHTABLE
{
  ui64 *hashId;
  SWHASHVALUE *valuePairs;
  int numEnt;
public:
  SWHASHTABLE(void);
  ~SWHASHTABLE(void);
  int Exists(ui64 hash);
  void Value(int index, SWVALUE *value);
  void EnterValue(ui64 hash, SWVALUE *value);
  void UpdateValue(ui32 hashIndex, ui64 hash, SWVALUE *value);
};


extern SWHASHTABLE *swHashTable;

//const int NIMHASHSIZE = 8973203;

typedef ui8 NIMVALUE;

typedef ui8 NIMHASHVALUE;

const int NIM_MAX_HASH_OFFSET = 10;

class NIMHASHTABLE
{
  ui64 *hashId;
  NIMHASHVALUE *values;
  int numEnt;
public:
  NIMHASHTABLE(void);
  ~NIMHASHTABLE(void);
  int Exists(ui64 hash);
  inline unsigned int GetValue(int hashIndex)const
  {
    return values[hashIndex];
  };
  void EnterValue(ui64 hash, NIMVALUE value);
  void UpdateValue(ui32 hashIndex, ui64 hash, NIMVALUE value);
};

extern NIMHASHTABLE *nimHashTable;

struct EDGE
{
  GROUPNUM ng;
  EDGELEN  el;
  inline bool operator >(const EDGE& e) const
  {
    return *((ui16 *)(this)) > *((ui16 *)(&e));
  };
  inline bool operator ==(const EDGE& e) const
  {
    return *((ui16 *)(this)) == *((ui16 *)(&e));
  };
  inline void operator =(const EDGE& e)
  {
    *((ui16 *)(this)) = *((ui16 *)(&e));
  };
  inline void Clear(void){*((ui16 *)this)=UNUSED2;};
};


struct EDGEDATA
{
  EDGE e[4];
  inline bool operator <(const EDGEDATA& ed) const
  {
    return *((ui64 *)(this)) < *((ui64 *)(&ed));
  };
  inline bool operator ==(const EDGEDATA& ed) const
  {
    return *((ui64 *)(this)) == *((ui64 *)(&ed));
  };
  inline void operator =(const EDGEDATA& ed)
  {
    *((ui64 *)this) = *((ui64 *)(&ed));
  };
  inline void Clear(void){*((ui64 *)this)=UNUSED8;};
};

struct STOPDATA
{
  STOPNUM   ns;
  EDGENUM   ne;
  inline int operator =(const EDGEDATA& sd)
  {
    *((ui16 *)(this)) = *((ui16 *)(&sd));
  };
  inline void Clear(void){*((ui16 *)this)=UNUSED2;};
};

struct NODEDATA
{
  EDGECOUNT ec;
  GROUPNUM  gn;
  STOPNUM   os;
  STOPNUM   ix;
  inline void Clear(void){*((ui32 *)this)=UNUSED4;ec=0;};
};

#define SwapNode(x,y)                          \
{                                              \
  STOPNUM ts;                                  \
  ts=nd[x].ix; nd[x].ix=nd[y].ix; nd[y].ix=ts; \
  nd[nd[x].ix].os = (STOPNUM)(x);              \
  nd[nd[y].ix].os = (STOPNUM)(y);              \
};


struct MOVE;

class CON //Collection Of Nodes
{
//  friend class STRING;
//  friend class NIMCON;
//  int m_numString;
public:
  int          m_numNode;
  unsigned int m_coinsRemaining;
  unsigned int m_neededToWin;
  unsigned int m_neededToTie;
  EDGEDATA     ed[256];
  STOPDATA     sd[256][4];
  NODEDATA     nd[256];


//  inline void SetupString(int stringNum)
//  {
//    m_firstLink  = m_stringDesc[stringNum].firstLink;
//    m_numLink    = m_stringDesc[stringNum].numLink;
//    m_firstJoint = m_stringDesc[stringNum].firstJoint;
//    m_numJoint   = m_stringDesc[stringNum].numJoint;
//  };
//  inline void SaveStringInfo(int stringNum)
//  {
//    m_stringDesc[stringNum].numLink = (ui16)m_numLink;
//    m_stringDesc[stringNum].numJoint = (ui16)m_numJoint;
//  };
//*************************************************************

  void Clear(int nn)
  {
    nd[nn].Clear();
    *((ui64 *)sd[nn]) = UNUSED8;
    ed[nn].Clear();
    nd[nn].ix = (STOPNUM)nn;
    nd[nn].os = (STOPNUM)nn;
  };
  void AddLink(int boxNum1, int boxNum2);
//  virtual void DeleteLink_i(unsigned int /*li*/){ImplementationError("dli");};
  void DeleteEdge(int nn, int en);
  inline void MoveEdge(int nnA, int edgeFrom, int edgeTo)
  {
    sd[nnA][edgeTo]=sd[nnA][edgeFrom];
    ed[nnA].e[edgeTo]=ed[nnA].e[edgeFrom];
    if (IS_A_NODE(sd[nnA][edgeTo].ns))
    {
      sd[sd[nnA][edgeTo].ns][sd[nnA][edgeTo].ne].ne=(EDGENUM)edgeTo;
    };
  };
//  virtual void DeleteJoint_n(unsigned int /*jn*/){ImplementationError("djn");};
  void DeleteNode(int nodeNum);
//  void DeleteString(int stringNum);
  void AddNode(void);
  void SwapJoint_n(int jointNum1, int jointNum2);
  void SwapLink_n(int linkNum1, int linkNum2);
  void SwapLink_i(int linkIndex1, int linkIndex2);
//  void SwapNode(int x, int y);
  void SortEdgesByGroup(int nn);
  void FixNeighborGroups(int nnA, int gn);
  void AddGround(int boxNum);
  void Compress(void);
//  bool StringGT(int stringNum1, int stringNum2);
  bool LinkClass_GE_i(int linkIndex1, int linkIndex2);
  bool JTLGT(unsigned int x, unsigned int y);
  void _SavePos(ui32 *saveArea);
  void _RestorePos(ui32 *saveArea);
  void DoubleCheck(void);
#ifdef _CHECK
  void CheckEverything(int nnFirst, int nnLast, bool checkRemainder) const;
//  virtual int _CheckEverything(void){ImplementationError("_ce");return 0;};
//  int _CheckEverything(void);
#endif
#if defined(SW_DUMP) || defined(NS_DUMP)
  void DUMP(char *title, int count, int p1, int p2,int nnFirst, int nnLast)const;
  void TraceReturn(int myCount, SWVALUE *pValue);
#endif
  CON(void);
  ~CON(void);
  CON& operator =(const DOTS_AND_BOXES& db);
  CON& operator =(const CON& con);
  bool operator ==(const CON& con) const;
  void LogUiUi(char *format, ui i1, ui i2) const;
  void LogUiI64Ui(char *format, ui i1, ui64 i2, ui i3);
//  unsigned int  NumString(void)const{return m_numString;};
  void Canon(int nnFirst, int num);
//  void SortStrings(void);
//  virtual bool CombineNodes(int /*jointCenter*/){ImplementationError("CN");return false;};
  bool CombineEdges(int nn);
//  virtual void _FormChains(void)=0;
  void FormChains(void);
  void _FormChains(void);
  void Check4SplitString(ui stringNum);
  void Capture(int nnA, int edge);
  void ObviousCapture(void);
//  virtual bool _GrabAnything(void)const=0;
  bool _GrabAnything(void) const;
  void GrabAnything(void);
//  virtual void _GiveNothing(ui16 *pPossibleMoves, int *pNumPossible)const=0;
//  void _GiveNothing(ui16 *possibleMoves, int *pNumPossible)const;
//  void GiveNothing(void);
//  virtual void _GiveLittle(ui16 *pPossibleMoves, int * pNumPossible, int * pShortest)const = 0;
  void GiveLittle(void)const;
  //void GiveLittle(void);
  ui64 SWHash(void);
  bool HISCHOICE(ui ourScore, ui hisScore, 
                 ui total, ui decline, 
                 SWVALUE& result, struct PossibleMove *pm);
  SWVALUE MYCHOICE(ui ourScore, ui hisScore, 
                     ui total, ui decline,
                     struct PossibleMove *pm);
  SWVALUE SWEvaluateMove(ui player1Score, ui player2Score, struct PossibleMove *pm);
  SWVALUE SWEvaluateAllMoves(ui ourScore, ui hisScore, struct PossibleMove *pm) const;
  SWVALUE CheckSWCapture(ui computerScore, ui humanScore, struct PossibleMove *pm) const;
  void TheGeneralCase(ui width, ui height, ui computerScore, ui humanScore, PossibleMove *pm);
  void TheCCD55Case(DOTS_AND_BOXES *pGame);
  void SimpleWin(DOTS_AND_BOXES *pGame);
  int MaxCostOfNimZero(int direction, int depth) const;
  void SnapLongChains(int numNode);
  //void NimSimplify(void);
  bool StringsEquiv(int str1, int str2);
  bool LinksEquiv_n(int lna, int lny); //within a single string
  void CTLNGLIAS(int jointNum, int x, int y);
  LONG_CHAIN_INFO ComputeLongChainInfo(void);
  LONG_CHAIN_INFO ComputeLongChainInfo(DOTS_AND_BOXES *pDB, MOVE *pMove);
  int  MeasureRope(void); //# ways NOT to reduce nimval to zero.
  //void CTLAS(ui jointIndex, ui x, ui y);
};

struct PossibleMove
{
  int  offering; //Number of coins offered by the move
  int  nimval;
  CON con;
  int  lineNum;
  inline bool operator >(const PossibleMove& pm) const
  {
    if (nimval==0)
    {
      if (pm.nimval==0) return offering>pm.offering;
      return false;
    }
    else
    {
      if (pm.nimval==0) return true;
      if (offering==pm.offering) return nimval<pm.nimval;
      return offering>pm.offering;
    };
  };
};


class NIMCON:public CON
{
public:
  int NimVal(const CON *con);
  int NimVal(const int nnFirst, const int nnLast);
  NIMCON& operator =(const CON *con);
  bool Simplify(int nnFirst, int nnLast);
  ui64 NimHash(int nnFirst, int nnLast);
  bool CombineEdges(int nnA);
  void DeleteEdge(int nn, int en, int *last);
  void DeleteNode(int nn, int *nnLast);
#ifdef _CHECK
  void CheckEverything(int nnFirst, int nnLast) const;
#endif
};
