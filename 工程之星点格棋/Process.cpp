#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
#include "BoxDef.h"
#include "NewBoxesDoc.h"
#include "Process.h"

extern CString currentProcessTitle;


PROCESS *(pCurrentProcess[20]); 
int numProcess = 0;
PROCESS *replacementProcess = NULL;
PROCESS *pushedProcess = NULL;
bool popProcess = false;


PROCESS::PROCESS(void)
{
}

PROCESS::~PROCESS(void)
{
}

void PROCESS::PopProcess(void)
{
  if (popProcess)
  {
    die("Doubly popped process");
    return;
  };
  popProcess = true;
}


void PROCESS::PushProcess(PROCESS *pNewProcess)
{
  if (pushedProcess != NULL)
  {
    die("Doubly pushed process");
    return;
  };
  pushedProcess = pNewProcess;
}

STATUS PROCESS::OnReturn(PROCESS *pProcess)
{
  delete pProcess;
  return STATUS_OK;
}
  

void PROCESS::ReplaceProcess(PROCESS *newProcess) 
{
  if (replacementProcess != NULL)
  {
    die ("Doubly replaced process");
    return;
  };
  replacementProcess = newProcess;
}

PROCESS *CurrentProcess(void)
{
  return pCurrentProcess[numProcess-1];
}

void ProcessManager(STATUS status) 
{
  for (;;)
  {
    switch(status)
    {
    case STATUS_STARTUP:
      if (numProcess != 0)
      {
        die("Opening Window twice???");
        return;
      };
      pCurrentProcess[0] = new PROCESS_PLAY_GAME;
      numProcess = 1;
      status = CurrentProcess()->OnInitialize();
      INVALIDATE
      break;
    case STATUS_SHUTDOWN:
      {
        int i;
        for (i=0; i<numProcess; i++)
        {
          delete pCurrentProcess[i];
          pCurrentProcess[i] = NULL;
        };
        numProcess = 0;
      };
      return;
    case STATUS_OK:
    case STATUS_IGNORED:
      break;
    case STATUS_DONE:
      numProcess--;
      popProcess = false;
      status = pCurrentProcess[numProcess-1]->OnReturn(pCurrentProcess[numProcess]);
      INVALIDATE
      continue;
    default:
      die("Unknown ProcessMaintenance message");
      return;
    };
    if (replacementProcess != NULL)
    {
      delete pCurrentProcess[numProcess-1];
      pCurrentProcess[numProcess-1] = replacementProcess;
      replacementProcess = NULL;
      status = pCurrentProcess[numProcess-1]->OnInitialize();
      INVALIDATE
      continue;
    };
    if (popProcess)
    {
      numProcess--;
      popProcess = false;
      status = pCurrentProcess[numProcess-1]->OnReturn(pCurrentProcess[numProcess]);
      INVALIDATE
      continue;
    };
    if (pushedProcess != NULL)
    {
      pCurrentProcess[numProcess++] = pushedProcess;
      pushedProcess = NULL;
      status = pCurrentProcess[numProcess-1]->OnInitialize();
      INVALIDATE
      continue;
    };
    break;
  };
}
/*
bool PROCESS_IDLE::IsOK_UnDoTurn(void) const
{
  return pDoc->IsUndoPossible();
}

bool PROCESS_IDLE::IsOK_ReDoTurn(void) const
{
  return pDoc->IsRedoPossible();
}

STATUS PROCESS_IDLE::OnComputerMove(void)
{
  PushProcess(new PROCESS_COMPUTER_MOVE(&pDoc->Game()));
  return STATUS_OK;
}

STATUS PROCESS_IDLE::OnComputeNimval(void)
{
  PushProcess(new PROCESS_NIMVALUE(&pDoc->Game()));
  return STATUS_OK;
}

STATUS PROCESS_IDLE::OnEnterPosition(void)
{
  PushProcess(new PROCESS_ENTER_POSITION);
  return STATUS_OK;
}

STATUS PROCESS_IDLE::OnPlayGame(void)
{
  PushProcess(new PROCESS_PLAY_GAME);
  return STATUS_OK;
}


STATUS PROCESS_IDLE::OnUnDoTurn(void)
{
  PushProcess(new PROCESS_UNDO_TURN);
  return STATUS_OK;
}

STATUS PROCESS_IDLE::OnReDoTurn(void)
{
  PushProcess(new PROCESS_REDO_TURN);
  return STATUS_OK;
}
*/
