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
  inline operator =(const EDGEDATA& sd)
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


class CON //Collection Of Nodes
{
//  friend class STRING;
//  friend class NIMCON;
//  int m_numString;
public:
  int          m_numNode;
  unsigned int m_coinsRemaining;
  unsigned int m_neededToWin;
  EDGEDATA     ed[MAX_NODES];
  STOPDATA     sd[MAX_NODES][4];
  NODEDATA     nd[MAX_NODES];


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
  bool CombineNodes(int nn);
//  virtual void _FormChains(void)=0;
  void FormChains(void);
  void _FormChains(void);
  void Check4SplitString(ui stringNum);
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
  void SimpleWin(DOTS_AND_BOXES *pGame);
  int MaxCostOfNimZero(int direction, int depth) const;
  void SnapLongChains(int numNode);
  //void NimSimplify(void);
  bool StringsEquiv(int str1, int str2);
  bool LinksEquiv_n(int lna, int lny); //within a single string
  void CTLNGLIAS(int jointNum, int x, int y);
  //void CTLAS(ui jointIndex, ui x, ui y);
};

struct PossibleMove
{
  int  offering; //Number of coins offered by the move
  int  nimval;
  CON con;
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
#ifdef _CHECK
  void CheckEverything(int nnFirst, int nnLast) const;
#endif
};

