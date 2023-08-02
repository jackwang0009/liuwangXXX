class CNewBoxesDoc;

enum STATUS
{
  STATUS_STARTUP,
  STATUS_SHUTDOWN,
  STATUS_DONE,
  STATUS_OK,
  STATUS_IGNORED,
  STATUS_ERROR,
};


class PROCESS
{
protected:
  void PopProcess(void);
  void PushProcess(PROCESS *pNewProcess);
  void ReplaceProcess(PROCESS *pNewProcess);
public:
  PROCESS(void);
  ~PROCESS(void);

  CString m_title;
  virtual CString GetTitle(void){return m_title;};

  virtual STATUS  OnShutdown(void){return STATUS_ERROR;}; // All processes should handle shutdown!
  virtual STATUS  OnReturn(PROCESS *pProcess); //default deletes returning process. STATUS_DONE.
  virtual STATUS  OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/){return STATUS_IGNORED;};
  virtual STATUS  OnInitialize(void){m_title="Process Error";return STATUS_OK;}; // You must supply a title!
  virtual STATUS  OnChar(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/){return STATUS_OK;}; 
  virtual STATUS  OnTimer(UINT /*nIDEvent*/){return STATUS_OK;};
  virtual STATUS  OnThreadCompletion(void){return STATUS_OK;};
  virtual STATUS  OnComputerMove(void){return STATUS_IGNORED;};
  virtual STATUS  OnComputeNimval(void){return STATUS_IGNORED;};
  virtual STATUS  OnEnterPosition(void){return STATUS_IGNORED;};
  virtual STATUS  OnPlayGame(void){return STATUS_IGNORED;};
  virtual STATUS  OnUnDoTurn(void){return STATUS_IGNORED;};
  virtual STATUS  OnReDoTurn(void){return STATUS_IGNORED;};
  virtual bool    IsIdle(void)const{return false;};
  virtual bool    IsOK_ComputerMove(void)const{return false;};
  virtual bool    IsOK_ComputeNimval(void)const{return false;};
  virtual bool    IsOK_EnterPosition(void)const{return false;};
  virtual bool    IsOK_PlayGame(void)const{return false;};
  virtual bool    IsOK_UnDoTurn(void)const{return false;};
  virtual bool    IsOK_ReDoTurn(void)const{return false;};
  virtual bool    IsOK_TestCreation(void)const{return false;};
  virtual bool    IsOK_Options(void)const{return false;};
};


/*
class PROCESS_IDLE:public PROCESS
{
public:
  STATUS OnShutdown(void){return STATUS_DONE;};
  STATUS OnInitialize(void){m_title="Idle";return STATUS_OK;};
  STATUS OnEnterPosition(void);
  STATUS OnPlayGame(void);
  STATUS OnComputerMove(void);
  STATUS OnComputeNimval(void);
  STATUS OnUnDoTurn(void);
  STATUS OnReDoTurn(void);
  bool IsIdle(void)const{return true;};
  bool IsOK_ComputerMove(void)const{return true;};
  bool IsOK_ComputeNimval(void) const {return true;};
  bool IsOK_EnterPosition(void)const{return true;};
  bool IsOK_PlayGame(void)const{return true;};
  bool IsOK_UnDoTurn(void)const;
  bool IsOK_ReDoTurn(void)const;
  bool IsOK_TestCreation(void)const{return true;};
  bool IsOK_Options(void)const{return true;};
};
*/
/*
class PROCESS_ENTER_POSITION:public PROCESS
{
  STATUS  OnLButtonDown(UINT nFlags, CPoint point) ;
  STATUS  OnInitialize(void);
  STATUS  OnShutdown(void);
  STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
  STATUS  OnUnDoTurn(void);
  STATUS  OnReDoTurn(void);
  bool    IsOK_UnDoTurn(void)const;
  bool    IsOK_ReDoTurn(void)const;
};
*/
class PROCESS_PLAY_GAME:public PROCESS
{
  STATUS  OnLButtonDown(UINT nFlags, CPoint point) ;
  STATUS  AutoPlay(void);
  STATUS  OnInitialize(void);
  STATUS  OnShutdown(void);
  STATUS  OnComputeNimval(void);
  STATUS  OnComputerMove(void);
  STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
  STATUS  OnUnDoTurn(void);
  STATUS  OnReDoTurn(void);
  STATUS  OnReturn(PROCESS *pProcess);
  bool    IsOK_ComputerMove(void)const{return true;};
  bool    IsOK_ComputeNimval(void) const {return true;};
  bool    IsOK_UnDoTurn(void)const;
  bool    IsOK_ReDoTurn(void)const;
  bool    IsOK_Options(void)const{return true;};
  //bool    IsOK_EnterPosition(void
  VECTOR  m_score;
};

class PROCESS_COMPUTER_MOVE:public PROCESS
{
  MOVE m_move;
  DOTS_AND_BOXES *m_pGame;
  void StartThread(void);

  STATUS  OnInitialize(void);
  STATUS  OnShutdown(void);
  STATUS  OnReturn(PROCESS *pProcess);
  STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
  STATUS  OnTimer(UINT nIDEvent);
  STATUS  OnThreadCompletion(void);
public:
  PROCESS_COMPUTER_MOVE(DOTS_AND_BOXES *pGame);
};

class PROCESS_NIMVALUE:public PROCESS
{
  //MOVE m_move;
  DOTS_AND_BOXES *m_pGame;
  void StartThread(void);

  STATUS  OnInitialize(void);
  STATUS  OnShutdown(void);
  STATUS  OnReturn(PROCESS *pProcess);
  STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
  //STATUS  OnTimer(UINT nIDEvent);
  STATUS  OnThreadCompletion(void);
public:
  PROCESS_NIMVALUE(DOTS_AND_BOXES *pGame);
};

class PROCESS_FLASH:public PROCESS
{
  MOVE m_move;
//  VECTOR m_boxesCaptured[2];
  int m_flashCount;
  LINE_TYPE m_lineType;
  OWNER_TYPE m_ownerType;
  STATUS  OnInitialize(void);
  STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
  STATUS  OnTimer(UINT nIDEvent);
  STATUS  OnShutdown(void);
public:
  PROCESS_FLASH(const MOVE& move);
};

//class PROCESS_FLASH_VERTICAL:public PROCESS
//{
//  VECTOR m_dotNum;
////  VECTOR m_boxesCaptured[2];
//  int m_flashCount;
//  LINE_TYPE m_lineType;
//  OWNER_TYPE m_ownerType;
//public:
//  PROCESS_FLASH_VERTICAL(const MOVE& move);
//  STATUS  OnInitialize(void);
//  STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
//  STATUS  OnTimer(UINT nIDEvent);
//  STATUS  OnShutdown(void);
//};

class PROCESS_UNDO_TURN:public PROCESS
{
  MOVE m_move;

  //STATUS  OnLButtonDown(UINT nFlags, CPoint point) ;
  STATUS  OnInitialize(void);
  STATUS  OnStartup(void);
  STATUS  OnReturn(PROCESS *pProcess);
  //STATUS  OnShutdown(void){return STATUS_DONE;};
  //STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
};

class PROCESS_REDO_TURN:public PROCESS
{
  MOVE m_move;

  //STATUS  OnLButtonDown(UINT nFlags, CPoint point) ;
  STATUS  OnInitialize(void);
  STATUS  OnStartup(void);
  STATUS  OnReturn(PROCESS *pProcess);
  //STATUS  OnShutdown(void){return STATUS_DONE;};
  //STATUS  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
};


//extern PROCESS *pCurrentProcess;
void ProcessManager(STATUS status);
void ReplaceCurrentProcess(PROCESS *newProcess);
PROCESS *CurrentProcess();