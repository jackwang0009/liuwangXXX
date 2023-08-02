// stdafx.cpp : source file that includes just the standard includes
//	NewBoxes.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void die(const char *msg)
{
  MessageBox(NULL,msg,"Fatal Error",MB_OK);PostQuitMessage(45);
  exit (0x23); 
}

void NotImplemented(char *msg)
{
  MessageBox(NULL, msg, "Not Implemented", MB_OK);
  exit(0x24);
}

void Note(const char *msg)
{
  MessageBox(NULL, msg, "Note", MB_OK);
}

void NoteUI(char *msg, unsigned int n)
{
  CString t;
  t.Format(msg, n);
  MessageBox(NULL, t, "Note", MB_OK);
}

void ImplementationError(const char *msg)
{
  MessageBox(NULL,msg,"Implementation Error",MB_OK);
  exit(0x25);
}


void Information(const char *msg)
{
  MessageBox(NULL,msg,"Information",MB_OK|MB_ICONEXCLAMATION);
}

