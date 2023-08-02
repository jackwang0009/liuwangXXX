#define WM_THREADCOMPLETION WM_USER+1

class arInput;


const int MAX_MOVE   = MAX_EDGES;
const int MAX_PROCESS = 10;

const int MOVE_STACK_SIZE = MAX_NODES*MAX_NODES/2;


typedef unsigned int    ui;
typedef          _int64 i64;
typedef unsigned _int64 ui64;
typedef          _int32 i32;
typedef unsigned _int32 ui32;
typedef          _int16 i16;
typedef unsigned _int16 ui16;
typedef          _int8  i8;
typedef unsigned _int8  ui8;



const int DEFAULT_WIDTH           = 6;   // #dots
const int DEFAULT_HEIGHT          = 6;   // #dots
const int DEFAULT_DOT_SPACE_X     = 75;  // #pixels
const int DEFAULT_DOT_SPACE_Y     = 75;  // #pixels
const int DEFAULT_DOT_SIZE_X      = 8;   // #pixels
const int DEFAULT_DOT_SIZE_Y      = 8;   // #pixels
const int DEFAULT_SCREEN_OFFSET_X = 30;  // #pixels
const int DEFAULT_SCREEN_OFFSET_Y = 64;  // #pixels
const int DEFAULT_HUMAN_SCORE_X   = 10;  // #pixels
const int DEFAULT_HUMAN_SCORE_Y   = 4;   // #pixels
const int DEFAULT_COMPUTER_SCORE_X= 10;  // #pixels
const int DEFAULT_COMPUTER_SCORE_Y= 18;  // #pixels
const int DEFAULT_STATUS_TEXT_X   = 100; // #pixels
const int DEFAULT_STATUS_TEXT_Y   = 10;  // #pixels
const int DEFAULT_MOVECOUNT_X     = 10;  // #pixels
const int DEFAULT_MOVECOUNT_Y     = 32;  // #pixels
const COLORREF DEFAULT_BACKGROUND_COLOR    = RGB(255,255,255);
const COLORREF DEFAULT_HUMAN_LINE_COLOR    = RGB(255,34,253);
const COLORREF DEFAULT_COMPUTER_LINE_COLOR = RGB(0,255,250);
const COLORREF DEFAULT_DOT_COLOR           = RGB(0,0,0);
const COLORREF DEFAULT_HUMAN_BOX_COLOR     = RGB(200,255,200);
const COLORREF DEFAULT_COMPUTER_BOX_COLOR  = RGB(255,200,200);
const COLORREF DEFAULT_HUMAN_TEXT_COLOR    = DEFAULT_HUMAN_LINE_COLOR;
const COLORREF DEFAULT_COMPUTER_TEXT_COLOR = DEFAULT_COMPUTER_LINE_COLOR;
const int DEFAULT_DOT_PEN_WIDTH  = 3;
const int DEFAULT_LINE_PEN_WIDTH = 3;
const int DEFAULT_MOVE_FLASH_COUNT = 150;
const int DEFAULT_RANDOM_SEED     = 500;
const bool DEFAULT_RANDOMIZE = false;
const int DEFAULT_TIMELIMIT = 20;

void InvalidateView(void);
#define INVALIDATE InvalidateView();

class NODE
{
  ui32 m_canonValue;
  i8   m_links[4];
  i8   m_numLink;
  i16  m_notUsed;
public:
  NODE(void);
  ~NODE(void);
  void WriteArchive(FILE *f, int nodeNum);
};

const int NOSCORE = 99999999;



class DOTS_AND_BOXES;
enum  MOVE_TYPE;

struct MOVE
{
  VECTOR    dotNum;
  bool      vertical;
  VECTOR    boxesCaptured[2];
  OWNER_TYPE oldOwnerType[2];
  OWNER_TYPE newOwnerType[2];
  LINE_TYPE  oldLineType;
  LINE_TYPE  newLineType;
  MOVE_TYPE  moveType;

  void FindCaptures(DOTS_AND_BOXES *pGame);
  void SetCaptures(DOTS_AND_BOXES *pGame);
  void ConstructFromLineNumber(int lineNum);
  CString TypeName(void)const;
};

class MOVE_HISTORY
{
  int m_totalMoves;
  int m_currentMoves;
  MOVE m_moves[MAX_MOVE];
  // bit 31 = computer move (versus human move)
  // The rest is the line number;
public:
  MOVE_HISTORY(void);
  MOVE StepForward(void);
  void Erase(void){m_totalMoves=0;m_currentMoves=0;};
  void AddMove(const MOVE& move);
  bool IsUndoPossible(void){return m_currentMoves>0;};
  bool IsRedoPossible(void){return m_currentMoves < m_totalMoves;};
  MOVE GetUndoMove(void);
  MOVE GetRedoMove(void);
};


class STRING;

//typedef (STRING::*kkk)(int i, int j);

#define LS 4  // LinkSize.  Number of LINKTYPE words per Link
              // Left, Right, Length, Original Line Number
#define INDEXLINKMASK (~3) // To compare two indices

#define JS 4  // JointSize. Number of JOINTTYPE words per Joint.
#define INDEXJOINTMASK (~3) // To compare two indices


#define LinkLength_i(linkIndex) m_links[((linkIndex)&(~3))+2]


//struct STRING_DESCRIPTOR
//{
//  i16 firstJoint;
//  i16 numJoint;
//  i16 firstLink;
//  i16 numLink;
//};


//struct JOINTINFO
//{
//  ui16 numLink;
//  ui16 groupNum;
//};




MOVE DeduceMove(DOTS_AND_BOXES *pGame, const CON *computerMove, MOVE_TYPE mt);
OWNER_TYPE OwnerType(LINE_TYPE lt);

#if (defined SW_DUMP)||(defined NS_DUMP)
void Break(void);
void CloseDump(void);
#define CLOSEDUMP CloseDump();
#else
#define CLOSEDUMP
#endif

