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

/* header of CStringOp */

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>
#include <vector>
// #include <tchar.h>
#include <stdarg.h>

#include "w32def.h"

class CStringOp;
CStringOp operator+( LPCTSTR sz, const CStringOp & x );
CStringOp operator+( TCHAR chr, const CStringOp & x );

class CStringOp
{
	std::vector<TCHAR>						m_s;
	int										cmpstr( const CStringOp &s2 ) const;
	void									cutstr();

public:
	CStringOp();
	CStringOp( const CStringOp & s );
	CStringOp( const wchar_t * sz );
	CStringOp( const char * sz );

	CStringOp operator=( const CStringOp & x );
	CStringOp operator=( LPCTSTR sz );
	bool operator==( const CStringOp & x ) const;
	bool operator>( const CStringOp & x ) const;
	bool operator>=( const CStringOp & x ) const;
	bool operator<( const CStringOp & x ) const;
	bool operator<=( const CStringOp & x ) const;
	LPCTSTR GetString() const;
	operator LPCTSTR() const;
	CStringOp Format( LPCTSTR psz, ... );
	CStringOp FormatV( LPCTSTR psz, va_list va );
	TCHAR & operator[]( size_t n );
	CStringOp operator+( const CStringOp & x ) const;
	CStringOp operator+( LPCTSTR sz ) const;
	CStringOp operator+( TCHAR chr ) const;
	CStringOp & operator+=( const CStringOp & s );
	CStringOp & operator+=( LPCTSTR psz );
	CStringOp & operator+=( TCHAR chr );
	CStringOp Mid( size_t nFrom = size_t( -1 ), size_t nTo = size_t( -1 ) ) const;
	size_t GetLength() const;

	friend CStringOp operator+( LPCTSTR sz, const CStringOp & x );
	friend CStringOp operator+( TCHAR chr, const CStringOp & x );
};
