#include "stdafx.h"

#include "DotsAndBoxes.h"
//#include "Utility.h"

ui32 MyRandom(ui32 n);


void DOTS_AND_BOXES::Setup(int width, int height)
{
  int i;
  m_vlines = new ui8[(width) * (height-1)];
  for (i=0; i<(width)*(height-1); i++)
  {
    m_vlines[i] = LINE_NONE;
  };
  m_hlines = new ui8[(width-1) * (height)];
  for (i=0; i<(width-1)*(height); i++)
  {
    m_hlines[i] = LINE_NONE;
  };
  m_owners = new ui8[(width-1)*(height-1)];
  for (i=0; i<(width-1)*(height-1); i++)
  {
    m_owners[i] = OWNER_NONE;
  };
  m_dotLoopThreat = new ui8[width*height];
  for (i=0; i<width*height; i++)
  {
    m_dotLoopThreat[i] = 0;
  };
  m_lineLoopThreat = new ui8[(width-1)*height + (height-1)*width];
  for (i=0; i<(width-1)*height+(height-1)*width; i++)
  {
    m_lineLoopThreat[i] = 0;
  };
  m_divideAndConquerWeight = new int[(width-1)*height + (height-1)*width];
  for (i=0; i<(width-1)*height+(height-1)*width; i++)
  {
    m_divideAndConquerWeight[i] = -1;
  };
  m_breakUpLoopsWeight = new int[(width-1)*height + (height-1)*width];
  m_loopWeightsValid = false;
  for (i=0; i<(width-1)*height+(height-1)*width; i++)
  {
    m_divideAndConquerWeight[i] = -1;
  };
  m_divideWeightsValid = false;
  m_avoidFlgs = new ui8[(width-1)*height + (height-1)*width];
  m_width = width;
  m_height = height;
}

void DOTS_AND_BOXES::Clear(void)
{
  delete m_vlines;
  delete m_hlines;
  delete m_owners;
  delete m_dotLoopThreat;
  delete m_lineLoopThreat;
  delete m_divideAndConquerWeight;
  delete m_breakUpLoopsWeight;
  delete m_avoidFlgs;
  m_loopWeightsValid = false;
  m_divideWeightsValid = false;
}

DOTS_AND_BOXES::DOTS_AND_BOXES(int width, int height)
{
  Setup(width, height);
}

DOTS_AND_BOXES::~DOTS_AND_BOXES(void)
{
  Clear();
}


void Write(CArchive& ar, char c)
{
  ar.Write(&c, 1);
}

void Write(CArchive& ar, const char *line)
{
  int i, len = strlen(line);
  for (i=0; i<len; i++)
  {
    if (line[i] == '\n')
    {
      Write(ar, '\015');
      Write(ar, '\012');
    } else Write(ar, line[i]);
  };
}



void DOTS_AND_BOXES::operator >> (CArchive& ar) const
{
  int row, col;
  char c;
  Write(ar,"#Comments are welcome anywhere\n");
  Write(ar,"#Human vertical = "); Write(ar, VERTICAL_HUMAN_CHAR); Write(ar,"\n");
  Write(ar,"#Computer vertical = "); Write(ar, VERTICAL_COMPUTER_CHAR); Write(ar,"\n");
  Write(ar,"#Human horizontal = "); Write(ar, HORIZONTAL_HUMAN_CHAR); Write(ar, "\n");
  Write(ar,"#Computer horizontal = "); Write(ar, HORIZONTAL_COMPUTER_CHAR); Write(ar, "\n");
  Write(ar,"#Human box ownership = "); Write(ar, OWNER_HUMAN_CHAR); Write(ar,"\n");
  Write(ar,"#Computer box ownership = "); Write(ar, OWNER_COMPUTER_CHAR); Write(ar,"\n");
  Write(ar,"#\n");
  for (row=0; row<m_height; row++)
  {
    for (col=0; col<m_width; col++) // The Horizontal bars
    {
      Write(ar, DOT_CHAR);
      if (col < m_width-1)
      {
        switch (HLineType(col, row))
        {
        case LINE_HUMAN: c = HORIZONTAL_HUMAN_CHAR; break;
        case LINE_COMPUTER: c = HORIZONTAL_COMPUTER_CHAR; break;
        default: c = ' ';
        };
        Write(ar, c);
      };
    };
    if (row == 0) Write(ar,"#The first row determines width");
    Write(ar, "\n");
    if (row < m_height-1)
    {
      // Now draw the vertical bars
      for (col=0; col<m_width; col++)
      {
        switch (VLineType(col, row))
        {
        case LINE_HUMAN: c = VERTICAL_HUMAN_CHAR; break;
        case LINE_COMPUTER: c = VERTICAL_COMPUTER_CHAR; break;
        default: c = ' ';
        };
        Write(ar, c);
        if (col < m_width-1)
        {
          switch(BoxOwner(col, row))
          {
          case OWNER_HUMAN: c = OWNER_HUMAN_CHAR; break;
          case OWNER_COMPUTER: c = OWNER_COMPUTER_CHAR; break;
          default: c = ' ';
          };
          Write(ar, c);
        };
      };
      Write(ar,"\n");
    };
  };
  Write(ar,"Anything illegal in column one determines height.\n");
}


void BadFormat(char *message)
{
  CString m;
  m.Format("Improperly formatted Dots-and-Boxes File\n%s",message);
  MessageBox(NULL, m, "Error loading", MB_OK);
}

bool DOTS_AND_BOXES::ReadFromArchive (arInput& arIn)
{
  int col, row, m;
  CString line;
  bool error;
  LINE_TYPE lineType = LINE_NONE; // to avoid compiler warning
  OWNER_TYPE ownerType;
  m_loopWeightsValid = false;
  m_divideWeightsValid = false;
  for(row=0; row<MAX_HEIGHT; row++)
  {
    // First dots and horizontal lines
    if ((line = arIn.ReadLine()).GetLength() == 0) break;
    m = (row==0) ? (strlen(line)+1)/2 : m_width;
    for (col=0; col<m; col++)
    {
      if (line[2*col] != DOT_CHAR)
      {
        if (row == 0)
        {
          m_width = col;
          break;
        }
        else
        {
          m_height = row;
          return true;
        };
      };
      if (col < m-1)
      {
        error = false;
        switch (line[2*col+1])
        {
        case HORIZONTAL_HUMAN_CHAR: lineType = LINE_HUMAN; break;
        case HORIZONTAL_COMPUTER_CHAR: lineType = LINE_COMPUTER; break;
        case ' ': lineType = LINE_NONE; break;
        default: error = true;
        };
        if (error)
        {
          if (row == 0) 
          {
            m_width = col + 1;
            break;
          }
          else
          {
            BadFormat("Expected Horizontal line character");
            return false;
          };
        };
        SetHLineType(col, row, lineType);
      };
    };
    // Now vertical lines and box owners
    if ((line = arIn.ReadLine()).GetLength() == 0) 
    {
      BadFormat("Expected vertical lines and owners.");
      return false;
    };
    for (col=0; col<m_width; col++)
    {
      switch ((line.GetLength()>2*col)?line[2*col]:' ')
      {
      case VERTICAL_HUMAN_CHAR: lineType = LINE_HUMAN; break;
      case VERTICAL_COMPUTER_CHAR: lineType = LINE_COMPUTER; break;
      case ' ': lineType = LINE_NONE; break;
      case 0:    
      case '\n': lineType = LINE_NONE; break;
      default:
        m_height = row + 1;
        return true;
      };
      SetVLineType(col, row, lineType);
      if (col < m_width-1)
      {
        switch (line.GetLength()>(2*col+1)?line[2*col+1]:' ')
        {
        case OWNER_HUMAN_CHAR: ownerType = OWNER_HUMAN; break;
        case OWNER_COMPUTER_CHAR: ownerType = OWNER_COMPUTER; break;
        case ' ': ownerType = OWNER_NONE; break;
        case 0:    
        case '\n': ownerType = OWNER_NONE; break;
        default:
          BadFormat("Expected blank or Owner Type");
          return false;
        };
        SetBoxOwner(col, row, ownerType);
      };
    };
  };
  BadFormat("No game found in file");
  return false;
}

void DOTS_AND_BOXES::operator <<(arInput& arIn)
{
  bool result;
  DOTS_AND_BOXES newGame(MAX_WIDTH, MAX_HEIGHT);
  result = newGame.ReadFromArchive(arIn);
  if (result) *this = newGame;
}

DOTS_AND_BOXES& DOTS_AND_BOXES::operator =(const DOTS_AND_BOXES& game)
{
  Clear();
  Setup(game.m_width, game.m_height);
  memcpy(m_vlines,game.m_vlines,m_width*(m_height-1));
  memcpy(m_hlines,game.m_hlines,(m_width-1)*m_height);
  memcpy(m_owners,game.m_owners,(m_width-1)*(m_height-1));
  m_loopWeightsValid = false;
  m_divideWeightsValid = false;
  return *this;
}

DOTS_AND_BOXES& DOTS_AND_BOXES::operator =(ui64 p)
{
  POINT dot;
  Clear();
  Setup(m_width, m_height);
  for (dot.y = 0; dot.y < m_height-1; dot.y++)
  {
    for (dot.x=0; dot.x<m_width-1; dot.x++)
    {
      if (p & 1) SetHLineType(dot,LINE_COMPUTER);
      p >>= 1;
    };
    for (dot.x=0; dot.x<m_width; dot.x++)
    {
      if (p & 1) SetVLineType(dot,LINE_COMPUTER);
      p >>= 1;
    };
  };
  for (dot.x=0; dot.x<m_width-1; dot.x++)
  {
    if (p & 1) SetHLineType(dot,LINE_COMPUTER);
    p >>= 1;
  };
  m_loopWeightsValid = false;
  m_divideWeightsValid = false;
  return *this;
}

  
int DOTS_AND_BOXES::NumberOfLines(void) const
{
  POINT dot;
  int count = 0;
  for (dot.y = 0; dot.y < m_height-1; dot.y++)
  {
    for (dot.x=0; dot.x<m_width; dot.x++)
    {
      if (VLineType(dot) != LINE_NONE) count++;
    };
  };
  for (dot.x=0; dot.x<m_width-1; dot.x++)
  {
    for (dot.y=0; dot.y<m_height; dot.y++)
    {
      if (HLineType(dot) != LINE_NONE) count++;
    };
  };
  return count;
}


  
void DOTS_AND_BOXES::RemoveCapturables(void)
{
  VECTOR dot;
  bool capture;
  do
  {
    capture = false;
    for (dot.y=0; dot.y<m_height-1; dot.y++)
    {
      for (dot.x=0; dot.x<m_width-1; dot.x++)
      {
        if (CountLines(dot) == 3)
        {
          SetVLineType(dot.x, dot.y, LINE_COMPUTER);
          SetHLineType(dot.x, dot.y, LINE_COMPUTER);
          SetHLineType(dot.x, dot.y+1, LINE_COMPUTER);
          SetVLineType(dot.x+1, dot.y, LINE_COMPUTER);
          m_loopWeightsValid = false;
          m_divideWeightsValid = false;
          capture = true;
        };
      };
    };
  } while (capture);
}

int DOTS_AND_BOXES::CountCapturables(void) const
{
  VECTOR dot;
  int capture;
  capture = 0;
  for (dot.y=0; dot.y<m_height-1; dot.y++)
  {
    for (dot.x=0; dot.x<m_width-1; dot.x++)
    {
      if (CountLines(dot) == 3)
      {
        capture++;
      };
    };
  };
  return capture;
}


int DOTS_AND_BOXES::CountLines(POINT dotNum) const
{ // Count the lines around the box at dotNum (below and right)
  int result = 0;
  if (dotNum.x >= Size().x - 1) return 0;
  if (dotNum.y >= Size().y - 1) return 0;
  if (HLineType(dotNum) != LINE_NONE) result++;
  if (VLineType(dotNum)   != LINE_NONE) result++;
  dotNum.x++;
  if (VLineType(dotNum)   != LINE_NONE) result++;
  dotNum.x--;
  dotNum.y++;
  if (HLineType(dotNum) != LINE_NONE) result++;
  return result;
}




bool DOTS_AND_BOXES::IsFinished(void) const
{
  VECTOR dot;
  for (dot.y=0; dot.y<m_height; dot.y++)
  {
    for (dot.x=0; dot.x<m_width; dot.x++)
    {
      if (dot.x < m_width-1)
      {
        if (HLineType(dot) == LINE_NONE) return false;
      };
      if (dot.y < m_height-1)
      {
        if (VLineType(dot) == LINE_NONE) return false;
      };
    };
  };
  return true;
}


/*
void DOTS_AND_BOXES::ComputeDivideAndConquerWeights(void)
{
  int width, height;
  POINT dotNum;
  FLOW flow;
  width = Width();
  height = Height();
  flow.ComputeFlow(this);
  for (dotNum.x=0; dotNum.x<width; dotNum.x++)
  {
    for (dotNum.y=0; dotNum.y<height; dotNum.y++)
    {
      int weight;
      if (dotNum.x < width-1)
      {
        //Check the horizontal
        if ( (HLineType(dotNum) == LINE_NONE) && (m_avoid[Dot2Line(dotNum,false)] == 0))
        {
          POINT boxNum1, boxNum2;
          boxNum1 = boxNum2 = dotNum;
          boxNum1.y--;
          if (    ((dotNum.y <= 0) || (CountLines(boxNum1) < 2)) 
              && ((dotNum.y >= height-1) || (CountLines(boxNum2) < 2))
            )
          {
            if ((dotNum.y == 0) || (dotNum.y == height-1))
            {
              weight = 0;
            }
            else
            {
              int c0, c1;
              c0 = flow.m_v[dotNum.x][dotNum.y][0] - flow.m_v[dotNum.x+1][dotNum.y][0];
              c1 = flow.m_v[dotNum.x][dotNum.y][1] - flow.m_v[dotNum.x+1][dotNum.y][1];
              weight = c0*c0 + c1*c1;
            }
          }
          else
          {
            weight = 0;
          };
        }
        else
        {
          weight = 0;
        };
      }
      else
      {
        weight = 0;
      };
      m_divideAndConquerWeight[Dot2Line(dotNum, false)] = weight;
      if (dotNum.y < height-1)
      {//Check the Vertical
        if ((VLineType(dotNum) == LINE_NONE)&&(m_avoid[Dot2Line(dotNum,true)]==0))
        {
          POINT boxNum1, boxNum2;
          boxNum1 = boxNum2 = dotNum;
          boxNum1.x--;
          if (    ((dotNum.x <= 0) || (CountLines(boxNum1) < 2)) 
              && ((dotNum.x >= width-1) || (CountLines(boxNum2) < 2))
            )
          {
            if ((dotNum.x == 0) || (dotNum.x == width-1))
            {
              weight = 0;
            }
            else
            {
              int c0, c1;
              c0 = flow.m_v[dotNum.x][dotNum.y][0] - flow.m_v[dotNum.x][dotNum.y+1][0];
              c1 = flow.m_v[dotNum.x][dotNum.y][1] - flow.m_v[dotNum.x][dotNum.y+1][1];
              weight = c0*c0 + c1*c1;
            };
          }
          else
          {
            weight = 0;
          };
        }
        else
        {
          weight = 0;
        };
      }
      else
      {
        weight = 0;
      };
      m_divideAndConquerWeight[Dot2Line(dotNum, true)] = weight;
    };
  };
}
*/


void DOTS_AND_BOXES::ComputeBreakUpLoopWeights(int MINLOOPBREAK)
{
  POINT dotNum;
  int width, height, numEdge;
  char loopWeightMin[MAX_EDGES];
  if (!m_loopWeightsValid)
  {
    width = Width();
    height = Height();
    numEdge = width*(height-1) + height*(width-1);
    memset(m_breakUpLoopsWeight,0,numEdge*sizeof(m_breakUpLoopsWeight[0]));
    memset(loopWeightMin, 0, numEdge);
    for (dotNum.x=1; dotNum.x<width-1; dotNum.x++)
    {
      for (dotNum.y=1; dotNum.y<height-1; dotNum.y++)
      {
        int hLine, vLine, count;
        if (HLineType(dotNum.x, dotNum.y) != LINE_NONE) continue;
        if (VLineType(dotNum.x, dotNum.y) != LINE_NONE) continue;
        if (HLineType(dotNum.x-1, dotNum.y) != LINE_NONE) continue;
        if (VLineType(dotNum.x, dotNum.y-1) != LINE_NONE) continue;
        hLine = dotNum.y*(2*width-1) + dotNum.x;
        vLine = hLine + width - 1;
        //Count Lines around this point
        count = 1;
        if (HLineType(dotNum.x-1, dotNum.y-1) != LINE_NONE) count++;
        if (HLineType(dotNum.x  , dotNum.y-1) != LINE_NONE) count++;
        if (HLineType(dotNum.x-1, dotNum.y+1) != LINE_NONE) count++;
        if (HLineType(dotNum.x  , dotNum.y+1) != LINE_NONE) count++;
        if (VLineType(dotNum.x-1, dotNum.y-1) != LINE_NONE) count++;
        if (VLineType(dotNum.x-1, dotNum.y  ) != LINE_NONE) count++;
        if (VLineType(dotNum.x+1, dotNum.y-1) != LINE_NONE) count++;
        if (VLineType(dotNum.x+1, dotNum.y  ) != LINE_NONE) count++;
        if (count < 8)
        {
          int weight = 9*count*count;
          if (count >= MINLOOPBREAK)
          {
            loopWeightMin[hLine] = 1;
            loopWeightMin[vLine] = 1;
            loopWeightMin[hLine-1] =1;
            loopWeightMin[vLine - (2*width-1)] =1;
          };
          m_breakUpLoopsWeight[hLine] += weight;
          m_breakUpLoopsWeight[vLine] += weight;
          m_breakUpLoopsWeight[hLine-1] += weight;
          m_breakUpLoopsWeight[vLine - (2*width-1)] += weight;
        };
      };
    };
    for (dotNum.x=1; dotNum.x<width-1; dotNum.x++)
    {
      for (dotNum.y=1; dotNum.y<height-1; dotNum.y++)
      {
        int hLine, vLine;
        hLine = dotNum.y*(2*width-1) + dotNum.x;
        vLine = hLine + width - 1;
        //Horizontal 6-loops
        if (
                (dotNum.x < width-2)
            && (HLineType(dotNum.x  , dotNum.y  ) != LINE_NONE)
            && (VLineType(dotNum.x  , dotNum.y-1) == LINE_NONE)
            && (VLineType(dotNum.x+1, dotNum.y-1) == LINE_NONE)
            && (HLineType(dotNum.x+1, dotNum.y  ) == LINE_NONE)
            && (VLineType(dotNum.x+1, dotNum.y  ) == LINE_NONE)
            && (VLineType(dotNum.x  , dotNum.y  ) == LINE_NONE)
            && (HLineType(dotNum.x-1, dotNum.y  ) == LINE_NONE)
          )
        {
          //Count Lines around this point
          int count = 0;
          if (HLineType(dotNum.x-1, dotNum.y-1) != LINE_NONE) count++;
          if (HLineType(dotNum.x  , dotNum.y-1) != LINE_NONE) count++;
          if (HLineType(dotNum.x+1, dotNum.y-1) != LINE_NONE) count++;
          if (HLineType(dotNum.x-1, dotNum.y+1) != LINE_NONE) count++;
          if (HLineType(dotNum.x  , dotNum.y+1) != LINE_NONE) count++;
          if (HLineType(dotNum.x+1, dotNum.y+1) != LINE_NONE) count++;
          if (VLineType(dotNum.x-1, dotNum.y-1) != LINE_NONE) count++;
          if (VLineType(dotNum.x-1, dotNum.y  ) != LINE_NONE) count++;
          if (VLineType(dotNum.x+2, dotNum.y-1) != LINE_NONE) count++;
          if (VLineType(dotNum.x+2, dotNum.y  ) != LINE_NONE) count++;
          if (count < 10)
          {
            int weight = 4*count*count;
            if (count >= MINLOOPBREAK)
            {            
              loopWeightMin[hLine+1] = 1;
              loopWeightMin[vLine] = 1;
              loopWeightMin[vLine+1] = 1;
              loopWeightMin[hLine-1] = 1;
              loopWeightMin[vLine - (2*width-1)] = 1;
              loopWeightMin[vLine+1 - (2*width-1)] = 1;
            };
            m_breakUpLoopsWeight[hLine+1] += weight;
            m_breakUpLoopsWeight[vLine] += weight;
            m_breakUpLoopsWeight[vLine+1] += weight;
            m_breakUpLoopsWeight[hLine-1] += weight;
            m_breakUpLoopsWeight[vLine - (2*width-1)] += weight;
            m_breakUpLoopsWeight[vLine+1 - (2*width-1)] += weight;
          };
        };
        //Vertical 6-loops
        if (
                (dotNum.y < height-2)
            && (VLineType(dotNum.x  , dotNum.y  ) != LINE_NONE)
            && (HLineType(dotNum.x  , dotNum.y  ) == LINE_NONE)
            && (HLineType(dotNum.x  , dotNum.y+1) == LINE_NONE)
            && (VLineType(dotNum.x  , dotNum.y+1) == LINE_NONE)
            && (HLineType(dotNum.x-1, dotNum.y+1) == LINE_NONE)
            && (HLineType(dotNum.x-1, dotNum.y  ) == LINE_NONE)
            && (VLineType(dotNum.x  , dotNum.y-1) == LINE_NONE)
          )
        {
          //Count Lines around this point
          int count = 0;
          if (HLineType(dotNum.x-1, dotNum.y-1) != LINE_NONE) count++;
          if (HLineType(dotNum.x  , dotNum.y-1) != LINE_NONE) count++;
          if (HLineType(dotNum.x-1, dotNum.y+2) != LINE_NONE) count++;
          if (HLineType(dotNum.x  , dotNum.y+2) != LINE_NONE) count++;
          if (VLineType(dotNum.x-1, dotNum.y-1) != LINE_NONE) count++;
          if (VLineType(dotNum.x-1, dotNum.y  ) != LINE_NONE) count++;
          if (VLineType(dotNum.x-1, dotNum.y+1) != LINE_NONE) count++;
          if (VLineType(dotNum.x+1, dotNum.y-1) != LINE_NONE) count++;
          if (VLineType(dotNum.x+1, dotNum.y  ) != LINE_NONE) count++;
          if (VLineType(dotNum.x+1, dotNum.y+1) != LINE_NONE) count++;
          if (count < 10)
          {
            int weight = 4*count*count;
            if (count >= MINLOOPBREAK)
            {
              loopWeightMin[hLine] = 1;
              loopWeightMin[hLine + (2*width-1)] = 1;
              loopWeightMin[vLine + (2*width-1)] = 1;
              loopWeightMin[hLine-1 + (2*width-1)] = 1;
              loopWeightMin[hLine-1] = 1;
              loopWeightMin[vLine - (2*width-1)] = 1;
            };
            m_breakUpLoopsWeight[hLine] += weight;
            m_breakUpLoopsWeight[hLine + (2*width-1)] += weight;
            m_breakUpLoopsWeight[vLine + (2*width-1)] += weight;
            m_breakUpLoopsWeight[hLine-1 + (2*width-1)] += weight;
            m_breakUpLoopsWeight[hLine-1] += weight;
            m_breakUpLoopsWeight[vLine - (2*width-1)] += weight;
          };
        };
      };
    };
    for (int i=0; i<numEdge; i++)
    {
      if ((m_avoidFlgs[i]&(AV_OCCUPIED|AV_SACRIFICE)) != 0)
      {
        m_breakUpLoopsWeight[i] = 0;
      };
      if (loopWeightMin[i] == 0)
      {
        m_breakUpLoopsWeight[i] = 0;
      };
    };
    m_loopWeightsValid = true;
  };
}


void DOTS_AND_BOXES::BuildXitionTable(int xition[MAX_NODES][4][2])
{
  POINT dotNum;
  int width, height, boxNum;
  width = Width();
  height = Height();
  for (boxNum=0, dotNum.y=0; dotNum.y<height-1; dotNum.y++)
  {
    for (dotNum.x=0; dotNum.x<width-1; dotNum.x++, boxNum++)
    {
      //Up
      if ((dotNum.y > 0) && (HLineType(dotNum.x, dotNum.y) == LINE_NONE))
      {
        xition[boxNum][0][0] = boxNum-(width-1);
        xition[boxNum][0][1] = dotNum.y*(2*width-1) + dotNum.x;
      }
      else
      {
        xition[boxNum][0][0] = -1;
      };
      //Right
      if ((dotNum.x < width-2) && (VLineType(dotNum.x+1, dotNum.y) == LINE_NONE))
      {
        xition[boxNum][1][0] = boxNum + 1;
        xition[boxNum][1][1] = dotNum.y*(2*width-1) + width - 1+ dotNum.x + 1;
      }
      else
      {
        xition[boxNum][1][0] = -1;
      };
      //Down
      if ((dotNum.y < height-2) && (HLineType(dotNum.x, dotNum.y+1) == LINE_NONE))
      {
        xition[boxNum][2][0] = boxNum + (width-1);
        xition[boxNum][2][1] = (dotNum.y+1)*(2*width-1) + dotNum.x;
      }
      else
      {
        xition[boxNum][2][0] = -1;
      };
      //Left
      if ((dotNum.x > 0) && (VLineType(dotNum.x, dotNum.y) == LINE_NONE))
      {
        xition[boxNum][3][0] = boxNum-1;
        xition[boxNum][3][1] = dotNum.y*(2*width-1) + width - 1 + dotNum.x;
      }
      else
      {
        xition[boxNum][3][0] = -1;
      };
    };
  };
}

/*
void DOTS_AND_BOXES::ComputeDivideAndConquerWeights_2(void)
{
  int width, height, numBox, numEdges;
  int xition[MAX_NODES][4][2];
  int boxToVisit[MAX_NODES];
  int approachLine[MAX_NODES];
  int approachIndex[MAX_NODES];
  int boxNum;
  int order[4];
  char visited[MAX_NODES];
  int parent[MAX_NODES];
  int counts[MAX_EDGES];
  if (m_divideWeightsValid) return;
  order[0]=0; order[1]=1; order[2]=2; order[3]=3;
  width = Width();
  height = Height();
  BuildXitionTable(xition);
  numBox = (width-1)*(height-1);
  numEdges = height*(width-1) + width*(height-1);
  memset(counts, 0, numEdges*sizeof(counts[0]));
  for (boxNum=0; boxNum<numBox; boxNum++)
  {
    int numToVisit, numVisited;
    int thisBox, thatBox, backIndex;
    boxToVisit[0] = boxNum;
    approachLine[0] = -1;
    approachIndex[0] = -1;
    parent[0] = -1;
    numToVisit = 1;
    numVisited = 0;
    memset(visited, 0, numBox*sizeof(visited[0]));
    visited[boxNum] = 1;
    while (numToVisit > numVisited)
    {
      int j, offset, temp;
      thisBox = boxToVisit[numVisited];
      offset = MyRandom(4);
      temp=order[offset];
      order[offset] = order[0];
      order[0] = temp;
      for (j=0; j<4; j++)
      {
        int k;
        k = order[j];
        thatBox = xition[thisBox][k][0];
        if (thatBox < 0) continue;
        if (visited[thatBox] != 0) continue;
        visited[thatBox] = 1;
        boxToVisit[numToVisit] = thatBox;
        approachLine[numToVisit] = xition[thisBox][k][1];
        approachIndex[numToVisit] = numVisited;
        if (thatBox > boxNum)
        {
          for (backIndex=numToVisit; backIndex>0; backIndex=approachIndex[backIndex])
          {
            int j = approachLine[backIndex];
            counts[j]++;
          };
        };
        numToVisit++;
      };
      numVisited++;
    };
  };
  for (int i=0; i<numEdges; i++)
  {
    if (m_avoid[i] == 0)
    {
      m_divideAndConquerWeight[i] = counts[i];
    }
    else
    {
      m_divideAndConquerWeight[i] = 0;
    };
  };
  m_divideWeightsValid = true;
}
*/

void DIVIDE_3::AddPartner(int index, int pointNum)
{
  //Add all partners of pointNum to list at index.
  int i, x, y;
  for (i=0; i<m_numPartners[index]; i++)
  {
    if (m_partners[m_1stPartner[index]+i] == pointNum) return; //Already in list
  };
  m_partners[m_totalPartners++] = pointNum;
  m_numPartners[index]++;
  y = pointNum / m_width;
  x = pointNum % m_width;
  if (x < m_width-1)
  {
    if (m_pDB->HLineType(x, y) != LINE_NONE)
    {
      AddPartner(index, pointNum+1);
    };
  };
  if (x > 0)
  {
    if (m_pDB->HLineType(x-1, y) != LINE_NONE)
    {
      AddPartner(index, pointNum-1);
    };
  };
  if (y < m_height-1)
  {
    if (m_pDB->VLineType(x, y) != LINE_NONE)
    {
      AddPartner(index, pointNum+m_width);
    };
  };
  if (y > 0)
  {
    if (m_pDB->VLineType(x, y-1) != LINE_NONE)
    {
      AddPartner(index, pointNum-m_width);
    };
  };
}


void DIVIDE_3::FindNeighbors(int index, int pointNum)
{
  //Add all neighbors of pointNum to list at index.
  //This is pretty easy.  We just look in the four directions.
  int x, y;
  y = pointNum / m_width;
  x = pointNum % m_width;
  if (x < m_width-1)
  {
    if ((m_pDB->AvoidFlag(x, y, false) & (AV_PERIMETER|AV_OCCUPIED|AV_SACRIFICE)) == 0)
    {
      m_neighbors[m_totalNeighbors++] = pointNum+1;
      m_numNeighbors[index]++;
    };
  };
  if (x > 0)
  {
    if ((m_pDB->AvoidFlag(x-1, y, false) & (AV_PERIMETER|AV_OCCUPIED|AV_SACRIFICE)) == 0)
    {
      m_neighbors[m_totalNeighbors++] = pointNum-1;
      m_numNeighbors[index]++;
    };
  };
  if (y < m_height-1)
  {
    if ((m_pDB->AvoidFlag(x, y, true) & (AV_PERIMETER|AV_OCCUPIED|AV_SACRIFICE)) == 0)
    {
      m_neighbors[m_totalNeighbors++] = pointNum+m_width;
      m_numNeighbors[index]++;
    };
  };
  if (y > 0)
  {
    if ((m_pDB->AvoidFlag(x, y-1, true) & (AV_PERIMETER|AV_OCCUPIED|AV_SACRIFICE)) == 0)
    {
      m_neighbors[m_totalNeighbors++] = pointNum-m_width;
      m_numNeighbors[index]++;
    };
  };
}

DIVIDE_3::DIVIDE_3(const DOTS_AND_BOXES *pDB)
{
  //First we build a table of partners and neighbors
  //
  POINT dotNum;
  int pointNum;
  m_pDB = pDB;
  m_width = m_pDB->Width();
  m_height = m_pDB->Height();
  m_numNodes = m_width*m_height;
  m_totalPartners =0;
  m_totalNeighbors = 0;
  for (dotNum.y=0; dotNum.y<m_height; dotNum.y++)
  {
    for (dotNum.x=0; dotNum.x<m_width; dotNum.x++)
    {
      pointNum = dotNum.y*m_width+dotNum.x;
      m_numPartners[pointNum] = 0;
      m_1stPartner[pointNum] = m_totalPartners;
      AddPartner(pointNum, pointNum);
      m_numNeighbors[pointNum] = 0;
      m_1stNeighbor[pointNum] = m_totalNeighbors;
      FindNeighbors(pointNum, pointNum);
    };
  };
}

void DIVIDE_3::FindDistances(int *src, int *distances)
{
  int i;
  int listed[MAX_NODES];
  int list[MAX_NODES];
  int numInList, numVisited;
  memset(listed,0,4*MAX_NODES);
  for (i=0; i<MAX_NODES; i++) distances[i] = 9999999;
  numInList = 0;
  while (*src >= 0)
  {
    int j;
    for (j=0; j<m_numPartners[*src]; j++)
    {
      int partner;
      partner = m_partners[m_1stPartner[*src]+j];
      if (listed[partner] == 0)
      {
        list[numInList++] = partner;
        listed[partner] = 1;
        distances[partner] = 0;
      };
    };
    src++;
  };
  numVisited = 0;
  while (numVisited < numInList)
  {
    int i, pointNum;
    pointNum = list[numVisited++];
    for (i=0; i<m_numNeighbors[pointNum]; i++)
    {
      int neighbor;
      neighbor = m_neighbors[m_1stNeighbor[pointNum]+i];
      if (distances[neighbor] > distances[pointNum]+1) 
      {
        int j;
        for (j=0; j<m_numPartners[neighbor]; j++)
        {
          int neighborPoint;
          neighborPoint = m_partners[m_1stPartner[neighbor] + j];
          distances[neighborPoint] = distances[pointNum] + 1;
          list[numInList++] = neighborPoint;
        };
      };
    };
  };
}


int DIVIDE_3::LineNumber(int p1, int p2)
{
  int x, y, line;
  x = p1%m_width;
  y = p1/m_width;
  line = y*(2*m_width-1)+x;
  if (p2 == p1+1) return line;
  else if (p2 == p1-1) return line-1;
  else if (p2 < p1) return line-m_width;
  else return line+m_width-1;
}

void DIVIDE_3::FindWeights(int *src, int *dst, int *weights)
{
  int i;
  int srcDistances[MAX_NODES], dstDistances[MAX_NODES];
  FindDistances(src, srcDistances);
  FindDistances(dst, dstDistances);
  for (i=0; i<m_numNodes; i++)
  {
    int j;
    int distance;
    distance = srcDistances[i] + dstDistances[i];
    for (j=0; j<m_numNeighbors[i]; j++)
    {
      int neighbor;
      neighbor = m_neighbors[m_1stNeighbor[i]+j];
      if (srcDistances[neighbor]+dstDistances[neighbor] == distance)
      {
        if (srcDistances[neighbor] != srcDistances[i])
        {
          int line;
          line = LineNumber(i, neighbor);
          if (weights[line] > 100 * distance) weights[line] = 100*distance;
        };
      };
    };
  };
}  


void DOTS_AND_BOXES::ComputeDivideAndConquerWeights_3(void)
{
  int i, numEdges;
  int src[10], dst[10];
  int weights[MAX_EDGES];
  if (m_divideWeightsValid) return;
  numEdges = Width()*(Height()-1) + Height()*(Width()-1);
  for (i=0; i<numEdges; i++) weights[i] = 999999999;
  FindAvoids();
  DIVIDE_3 divide(this);
  //
  // Small line at top to long line at bottom
  for (i=2; i<m_width-2; i++) src[i-2] = i;
  src[i-2] = -1;
  for (i=1; i<m_width-1; i++) dst[i-1] = (m_height-1)*m_width+i;
  dst[i-1] = -1;
  divide.FindWeights(src, dst, weights);
  // Small line at bottom to long line at top
  for (i=1; i<m_width-1; i++) src[i-1] = i;
  src[i-1] = -1;
  for (i=2; i<m_width-2; i++) dst[i-2] = (m_height-1)*m_width+i;
  dst[i-2] = -1;
  divide.FindWeights(src, dst, weights);
  // Small line at left to long line at right
  for (i=2; i<m_height-2; i++) src[i-2] = m_width*i;
  src[i-2] = -1;
  for (i=1; i<m_height-1; i++) dst[i-1] = m_width*(i+1)-1;
  dst[i-1] = -1;
  divide.FindWeights(src, dst, weights);
  // Small line at left to long line at right
  for (i=2; i<m_height-2; i++) src[i-2] = m_width*i;
  src[i-2] = -1;
  for (i=1; i<m_height-1; i++) dst[i-1] = m_width*(i+1)-1;
  dst[i-1] = -1;
  divide.FindWeights(src, dst, weights);
  //Simply move the weights into m_divideAndConquerWeight
  {
    int maxWeight, numEdge;
    numEdge = (m_width-1)*m_height + (m_height-1)*m_width;
    maxWeight = 0;
    for (i=0; i<numEdge; i++)
    {
      if (weights[i] == 999999999) continue;
      if (weights[i] > maxWeight) maxWeight = weights[i];
    };
    for (i=0; i<numEdge; i++)
    {
      if (weights[i] == 999999999) m_divideAndConquerWeight[i] = 0;
      else m_divideAndConquerWeight[i] = maxWeight+1-weights[i];
    };
  };
  m_divideWeightsValid = true;
}


POINT DOTS_AND_BOXES::MinMaxLoopWeight(void) const
{
  int i, numEdge;
  POINT result;
  result.x = 99999999;
  result.y = 0;
  numEdge = m_width*(m_height-1) + m_height*(m_width-1);
  for (i=0; i<numEdge; i++)
  {
    if (    (m_breakUpLoopsWeight[i] < result.x)
         && (m_breakUpLoopsWeight[i] != 0)
       )
    {
      result.x = m_breakUpLoopsWeight[i];
    };
    if (m_breakUpLoopsWeight[i] > result.y)
    {
      result.y = m_breakUpLoopsWeight[i];
    };
  };
  return result;
}

POINT DOTS_AND_BOXES::MinMaxDivideWeight(void) const
{
  int i, numEdge;
  POINT result;
  result.x = 99999999;
  result.y = 0;
  numEdge = m_width*(m_height-1) + m_height*(m_width-1);
  for (i=0; i<numEdge; i++)
  {
    if (    (m_divideAndConquerWeight[i] < result.x)
         && (m_divideAndConquerWeight[i] != 0)
       )
    {
      result.x = m_divideAndConquerWeight[i];
    };
    if (m_divideAndConquerWeight[i] > result.y)
    {
      result.y = m_divideAndConquerWeight[i];
    };
  };
  return result;
}