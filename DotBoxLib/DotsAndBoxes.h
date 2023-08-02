
const int MAX_WIDTH  = 12;
const int MAX_HEIGHT = 11;
const int MAX_NODES = (MAX_WIDTH-1)*(MAX_HEIGHT-1);
const int MAX_EDGES = (MAX_WIDTH-1)*MAX_HEIGHT+MAX_WIDTH*(MAX_HEIGHT-1);


#define DOT_CHAR '+'
#define HORIZONTAL_HUMAN_CHAR '-'
#define HORIZONTAL_COMPUTER_CHAR '='
#define VERTICAL_HUMAN_CHAR 'I'
#define VERTICAL_COMPUTER_CHAR '|'
#define OWNER_HUMAN_CHAR 'H'
#define OWNER_COMPUTER_CHAR 'C'




enum LINE_TYPE
{
  LINE_NONE,
  LINE_COMPUTER,
  LINE_HUMAN,
  //LINE_FLASHING
};

enum OWNER_TYPE
{
  OWNER_NONE,
  OWNER_HUMAN,
  OWNER_COMPUTER,
  //OWNER_FLASHING
};



class arInput
{
  CArchive *m_ar;
public:
  arInput(CArchive *ar);
  ~arInput(void);
  CString ReadLine(void);
  void Backspace(void);
};


enum AV_FLAGS
{
  AV_OCCUPIED  = 1, // A line is already there
  AV_SACRIFICE = 2, // A box would become capturable
  AV_PERIMETER = 4, // A line on the perimeter of the game
};


class DOTS_AND_BOXES
{
  int *m_divideAndConquerWeight;
  bool m_divideWeightsValid;
  int *m_breakUpLoopsWeight;
  bool m_loopWeightsValid;
  int m_width;
  int m_height;
  ui8 *m_hlines;
  ui8 *m_vlines;
  ui8 *m_owners;
  ui8 *m_avoidFlgs;
  ui8 *m_dotLoopThreat;
  ui8 *m_lineLoopThreat;
  bool ReadFromArchive(arInput& arIn);
  void Clear(void);
  void Setup(int width, int height);
  void BuildXitionTable(int xition[MAX_NODES][4][2]);
  bool GiveNothing_BreakLoops(int minWeight, int *loopRank) const;
  bool GiveNothing_BothLoopAndDivide(
             int searchDepth, 
             int *loopRank, 
             int *divideRank);
  bool GiveNothing_Divide(int *divideRank, int minWeight);
  INLINE void v(void){m_loopWeightsValid=false;m_divideWeightsValid=false;};
public:
  DOTS_AND_BOXES(int width, int height);
  ~DOTS_AND_BOXES(void);
  void operator >> (CArchive& ar) const;
  void operator << (arInput& arIn);
  DOTS_AND_BOXES& operator = (const DOTS_AND_BOXES& game);
  DOTS_AND_BOXES& operator = (ui64 bits);
  //void Serialize(CArchive& ar);
  //void SerializeOut(CArchive& ar);
  //void SerializeIn(CArchive& ar);

  INLINE int Width(void) const {return m_width;};
  INLINE int Height(void) const {return m_height;};
  INLINE int NumEdges(void) const {return m_width*(m_height-1)+m_height*(m_width-1);};
  INLINE POINT Size(void) const {POINT p; p.x=m_width; p.y=m_height; return p;};

  INLINE LINE_TYPE VLineType(int col, int row)const{return (LINE_TYPE)m_vlines[row*m_width + col];};
  INLINE LINE_TYPE VLineType(const POINT& dotNum)const{return VLineType(dotNum.x, dotNum.y);};
  INLINE void SetVLineType(int col, int row, LINE_TYPE lt){v();m_vlines[row*m_width+col]=(ui8)lt;};
  INLINE void SetVLineType(const POINT& dotNum, LINE_TYPE lt){SetVLineType(dotNum.x, dotNum.y, lt);};
  INLINE LINE_TYPE HLineType(int col, int row)const{return (LINE_TYPE)m_hlines[row*(m_width-1) + col];};
  INLINE LINE_TYPE HLineType(const POINT& dotNum)const{return HLineType(dotNum.x, dotNum.y);};
  INLINE void SetHLineType(int col, int row, LINE_TYPE lt){v();m_hlines[row*(m_width-1)+col]=(ui8)lt;};
  INLINE void SetHLineType(const POINT& dotNum, LINE_TYPE lt){SetHLineType(dotNum.x, dotNum.y, lt);};

  INLINE OWNER_TYPE BoxOwner(int col, int row)const{return (OWNER_TYPE)m_owners[row*(m_width-1) + col];};
  INLINE OWNER_TYPE BoxOwner(const POINT& dotNum)const{return BoxOwner(dotNum.x, dotNum.y);};
  INLINE void SetBoxOwner(int col, int row, OWNER_TYPE ot){m_owners[row*(m_width-1)+col]=(ui8)ot;};
  INLINE void SetBoxOwner(const POINT& dotNum, OWNER_TYPE ot){SetBoxOwner(dotNum.x, dotNum.y, ot);};

  INLINE int Dot2Line(int x, int y, bool vertical) const 
         {
           return y * (2*m_width-1) + x + (vertical?(m_width-1):0);
         };
  INLINE int Dot2Line(POINT dotNum, bool vertical) const
  {
    return Dot2Line(dotNum.x, dotNum.y, vertical);
  };
  INLINE int AvoidFlag(int x, int y, bool vertical)const
  {
    return m_avoidFlgs[Dot2Line(x, y, vertical)];
  };
  void GiveNothing();
  int  NumberOfLines(void) const;
  int CountLines(POINT dotNum) const; //#lines around box below-right
  void RemoveCapturables(void);
  int  CountCapturables(void) const;
  bool IsCaptured(POINT dot);
  bool IsFinished(void) const;
  bool IsSpecial5x5(void){return (m_width==5)&&(m_height==5)&&(NumberOfLines()<10);};
//  void ComputeDivideAndConquerWeights(void);
  void DivideAndConquer(void);
  INLINE int VerticalLoopWeight(POINT dotNum){return m_breakUpLoopsWeight[Dot2Line(dotNum,true)];};
  INLINE int HorizontalLoopWeight(POINT dotNum){return m_breakUpLoopsWeight[Dot2Line(dotNum,false)];};
  INLINE int LoopWeight(POINT dotNum, bool vertical){return m_breakUpLoopsWeight[Dot2Line(dotNum,vertical)];};
  INLINE int VerticalDivideWeight(POINT dotNum){return m_divideAndConquerWeight[Dot2Line(dotNum,true)];};
  INLINE int HorizontalDivideWeight(POINT dotNum){return m_divideAndConquerWeight[Dot2Line(dotNum,false)];};
  void FindAvoids(void);
//  void ComputeDivideAndConquerWeights_2(void);
  void ComputeDivideAndConquerWeights_3(void);
  void ComputeBreakUpLoopWeights(int MINLOOPBREAK);
  POINT MinMaxLoopWeight(void) const;
  POINT MinMaxDivideWeight(void) const;
};

class DIVIDE_3
{
  const DOTS_AND_BOXES *m_pDB;
  int m_width;
  int m_height;
  int m_numNodes;
  int m_numPartners[MAX_NODES];
  int m_1stPartner[MAX_NODES];
  int m_numNeighbors[MAX_NODES];
  int m_1stNeighbor[MAX_NODES];
  int m_neighbors[MAX_NODES*4];
  int m_partners[MAX_NODES*MAX_NODES];
  int m_totalPartners;
  int m_totalNeighbors;
  void AddPartner(int index, int pointNum);
  void FindNeighbors(int index, int pointNum);
  void FindDistances(int *src, int *distances);
  int  LineNumber(int p1, int p2);
public:
  void Divide_3(int *src, int *dst);
  DIVIDE_3(const DOTS_AND_BOXES *pDB);
  void FindWeights(int *src, int *dst, int *weights);
};

//class TABLES
//{
//  TABLES::TABLES(int width, int height);
//};





/*

class RRCCn
{
protected:
//Capture tables
//For each line there are two boxes that might be captured.
//The entries in this table are masks of the 4 lines that
//capture each of the two boxes.
public:
  RRCCn(void);
  ~RRCCn(void);
  virtual ui64 Rotate(ui64 v)=0;
  virtual ui64 Reflect(ui64 v)=0;
  bool IsCapture(ui64 pos, int line);
};

*/
class RRCCp
{
  int m_width;
  int m_height;
  ui64 *m_rotTable;      //[4][65536]
  ui64 *m_refTable;      //[4][65536]
  ui64 *m_capturesTable; //[64][2]
  ui64 *m_cornersTable;  //[4][2]; [i][0]=the higher line; [i][1]=both lines;
public:
  RRCCp(int width, int height);
  ~RRCCp(void);
  ui64 Rotate(ui64 v);
  ui64 Reflect(ui64 v);
  bool IsCapture(ui64 pos, int line);
  // *************************************************
  // Note that the canonical corner configuration consists
  // of the lower line number when only one line exists.
  ui64 CornerCanon(ui64 pos);
  //**************************************************
  // Note that rotations and reflections are not strictly
  // geometric.  They maintain the canonical corner configurations.
  ui64 NextRotRef(ui64 pos, int n);
  //**************************************************
};

class OPENINGp
{
// The four entries for each number of lines are
// [0] Island wins
// [1] Island ties
// [2] Strait wins
// [3] Strait ties
  int  numPositions[4];
  ui64 *positions[4];
  int m_numLine;
public:
  OPENINGp(int width, int height, int numLine);
  ~OPENINGp(void);
  void Play(DOTS_AND_BOXES *pGame);
  inline ui64 Position(int type, int positionNumber)
  {
    return positions[type][positionNumber];
  };
  inline int NumPositions(int type)
  {
    return numPositions[type];
  };
};


class CCDn
{
protected:
  RRCCp *m_prar;
public:
  ui64 m_pos;
  //
  CCDn(void);
  ~CCDn(void);
  virtual CCDn& operator =(const DOTS_AND_BOXES *db)=0;
  //int Nimval(void);
  void Canon(void);
  void LogUiUiUI64(char *format, ui i1, ui i2, ui64 i3);
  virtual int NumRotRef(void)=0;
};

class CCDp:public CCDn
{
  int m_width;
  int m_height;
public:
  CCDp(int width, int height);
  ~CCDp(void);
  CCDn& operator =(const DOTS_AND_BOXES *db);
  bool IsCapture(int lineNum);
  inline int NumRotRef(void){return (m_width==m_height)?8:4;};
  bool GoodOpening(ui64 openingPosition, ui64 move);
};

const int RRCC_MaxWidth = 6;
const int RRCC_MaxHeight = 6;
class RRCC
{
  ui64 *rotTable[RRCC_MaxWidth][RRCC_MaxHeight];
  ui64 *refTable[RRCC_MaxWidth][RRCC_MaxHeight];
  ui64 *capturesTable[RRCC_MaxWidth][RRCC_MaxHeight];
  ui64 *cornersTable[RRCC_MaxWidth][RRCC_MaxHeight];
  int  refCount[RRCC_MaxWidth][RRCC_MaxHeight];
  void CaptureEntry(int width, int height, int dir, int *line);
  void BuildCapturesTable(int width, int height);
  void BuildRefRotTables(int width, int height);
  void AddCorner(int width, int height, int corner, int line1, int line2);
  void BuildCornersTable(int width, int height);
public:
  RRCC();
  ~RRCC(void);
  void IncrementReferenceCount(int width, int height);
  void DecrementReferenceCount(int width, int height);
  inline ui64 *GetRotTable(int width, int height)     {return rotTable[width][height];};
  inline ui64 *GetRefTable(int width, int height)     {return refTable[width][height];};
  inline ui64 *GetCapturesTable(int width, int height){return capturesTable[width][height];};
  inline ui64 *GetCornersTable(int width, int height) {return cornersTable[width][height];};
};

class FLOW
{
public:
  int m_v[MAX_WIDTH][MAX_HEIGHT][2];
  void ComputeFlow(const DOTS_AND_BOXES *pDB);
};

struct LONG_CHAIN_INFO
{
  int numLongChain;
  int longestChain;
  int sumOfLongChains;
  int gain;
};