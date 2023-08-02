// stdafx.h : include file for standard system include files, 
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F8548F61_5D2B_498E_AE67_E6592A020FB7__INCLUDED_)
#define AFX_STDAFX_H__F8548F61_5D2B_498E_AE67_E6592A020FB7__INCLUDED_


#define PREAMBLE02

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define INLINE _forceinline

#define HASH_CARRY

const int NIM_MAX_CANON = 999;
const int SW_MAX_CANON = 999;


typedef unsigned    int   ui;  
typedef unsigned  _int8  ui8;
typedef          _int16  i16;
typedef unsigned _int16 ui16;
typedef          _int32  i32;
typedef unsigned _int32 ui32;
typedef          _int64  i64;
typedef unsigned _int64 ui64;


const int NIMHASHSIZE =  2973203;
const int SWHASHSIZE  = 2973207;//15973203;
const int CDHASHSIZE  = 2973203;


enum DISPLAY_MODE
{
  DISPLAY_GAME = 1,
  DISPLAY_LOOP,
  DISPLAY_DIVIDE
};

extern DISPLAY_MODE displayMode;


class VECTOR: public POINT
{
public:
  VECTOR(void){};
  VECTOR(int a, int b){x=a;y=b;};
  VECTOR& operator =(const POINT& point){x=point.x;y=point.y;return *this;};
  VECTOR  operator +(const VECTOR& vector){VECTOR w;w.x=x+vector.x; w.y=y+vector.y; return w;};
  VECTOR  operator *(int n){VECTOR v;v.x=x*n; v.y=y*n; return v;};
  VECTOR  operator *(const VECTOR& v){VECTOR w;w.x=x*v.x;w.y=y*v.y;return w;};
  bool    operator != (const VECTOR& v){return (x!=v.x) || (y!=v.y);};
};




void die(const char *msg);
void Information(const char *msg);
void NotImplemented(char *msg);
void Note(const char *msg);
void NoteUI(char *msg, unsigned int n);
void ImplementationError(const char *msg);



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F8548F61_5D2B_498E_AE67_E6592A020FB7__INCLUDED_)
  

