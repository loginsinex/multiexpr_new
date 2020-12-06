/*
    An universal parser for math-like expressions
    Copyright (C) 2019 ALXR aka loginsin
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* definitions of win32 types (realy win32 is not used in this code, just types) */

#pragma once

#include <string>
#include <string.h>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#ifdef _DEBUG
#define _tprintf	printf
#else
#define _tprintf(...)	{}
#endif

#ifdef _UNICODE
#define __TEXT(quote)		L ## quote
#define TEXT(quote)		__TEXT(quote)
#define __T(x)			L ## x
#define _T(x)			__T(x)
#define tprintf			wprintf
typedef wchar_t* 		LPTSTR;
typedef wchar_t			TCHAR;
typedef const wchar_t*		LPCTSTR;
#define _tcslen			wcslen
#define _vsntprintf		vswprintf
#else
#define TEXT(quote)			quote
#define _T(x)				x
#define tprintf			printf
typedef char*			LPTSTR;
typedef char			TCHAR;
typedef const char*		LPCTSTR;
#define _tcslen			strlen
#define _vsntprintf		vsnprintf
#endif

typedef unsigned long DWORD;
typedef unsigned long long ULONG_PTR;
typedef unsigned long long ULONGLONG;
#define LODWORD(l) ((DWORD)(ULONG_PTR(l)&0xFFFFFFFF))
#define HIDWORD(l) ((DWORD)(((ULONG_PTR(l)>>32)&0xFFFFFFFF))

typedef int	BOOL;
typedef void	VOID;
typedef void*	PVOID;

#define UNREFERENCED_PARAMETER(p)	(p);
#define PURE	=0;
#define TRUE	1
#define FALSE	0

