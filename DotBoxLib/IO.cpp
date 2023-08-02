#include "stdafx.h"
#include "..\DotBoxLib\DotsAndBoxes.h"
#include "..\DotBoxLib\NIMCON.h"
//#include "BoxDef.h"

arInput::arInput(CArchive *ar)
{
  m_ar = ar;
}

arInput::~arInput(void)
{
}


CString arInput::ReadLine(void)
{
  CString line;
  char ch;
  for (;;)
  {
    if (m_ar->Read(&ch, 1) < 1)
    {
      if (line.GetLength() == 0) return line;
      if (line[0] != '#') return line;
      line = "";
      continue;
    };
    if (ch == '\015')
    {
      if (m_ar->Read(&ch, 1) < 1)
      {
        if (ch == '#') return CString("");
        line += '\000';
        line += '\000';
        return line;
      };
      if (ch == '\012')
      {
        line += '\n';
        if (line[0] != '#') return line;
        line = "";
        continue;
      };
    }
    else
    {
      line += ch;
    };
  };
}

