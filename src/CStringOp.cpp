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

/* Special implementation of strings. This strings sorts by length and contents. So '566' '1' '82' will be sorted as '1' '82' '566' */


#include "CStringOp.h"

CStringOp::CStringOp()
	: m_s( 1, 0 )
{

}

CStringOp::CStringOp( const CStringOp & x )
{
	m_s = x.m_s;
	cutstr();
	m_s.push_back( 0 );
}

CStringOp::CStringOp( const wchar_t * sz )
{

	if ( !sz || !sz[ 0 ] )
	{
		operator=( nullptr );
		return;
	}

#ifdef _UNICODE
	operator=( sz );
#else
	std::vector<TCHAR> vsz( wcslen( sz ) + 1, 0 );
	std::mbstate_t state = std::mbstate_t();
	std::wcsrtombs( vsz.data(), &sz, vsz.size(), &state );
	operator=( vsz.data() );
#endif
}

CStringOp::CStringOp( const char * sz )
{
	if ( !sz || !sz[ 0 ] )
	{
		operator=( nullptr );
		return;
	}

#ifndef _UNICODE
	operator=( sz );
#else
	std::vector<TCHAR> vsz( strlen( sz ) + 1, 0 );
	std::mbstate_t state = std::mbstate_t();
	std::mbsrtowcs( vsz.data(), &sz, vsz.size(), &state );
	operator=( vsz.data() );
#endif
}

void CStringOp::cutstr()
{
	m_s.erase( std::find( m_s.begin(), m_s.end(), 0 ), m_s.end() );
}

int CStringOp::cmpstr( const CStringOp & s2 ) const
{
	auto vend = m_s.end();
	auto v1 = m_s.begin();
	auto v2 = s2.m_s.begin();
	size_t l1 = m_s.size(), l2 = s2.m_s.size();
	if ( l1 > l2 )
	{
		return 1;
	}
	else if ( l1 < l2 )
	{
		return -1;
	}

	while ( *v1 == *v2 )
	{
		v1++; v2++;
		if ( v1 == vend )
		{
			return 0;
		}
	}

	if ( *v1 > *v2 )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

CStringOp CStringOp::operator=( const CStringOp & x )
{
	m_s = x.m_s;
	cutstr();
	m_s.push_back( 0 );
	return *this;
}

CStringOp CStringOp::operator=( LPCTSTR sz )
{
	size_t length = ( sz ? _tcslen( sz ) : 0 );
	size_t it = 0;
	m_s.clear();
	m_s.reserve( length + 1 );
	m_s.insert( m_s.begin(), length + 1, 0 );
	std::generate( m_s.begin(), m_s.end() - 1, [ &it, &sz ] () { return sz[ it++ ]; } );
	return *this;
}

bool CStringOp::operator==( const CStringOp & x ) const
{
	return ( 0 == cmpstr( x ) );
}

bool CStringOp::operator>( const CStringOp & x ) const
{
	return ( cmpstr( x ) > 0 );
}

bool CStringOp::operator>=( const CStringOp & x ) const
{
	return ( cmpstr( x ) >= 0 );
}

bool CStringOp::operator<( const CStringOp & x ) const
{
	return ( cmpstr( x ) < 0 );
}

bool CStringOp::operator<=( const CStringOp & x ) const
{
	return ( cmpstr( x ) <= 0 );
}

size_t CStringOp::GetLength() const
{
	return m_s.size() - 1;
}

LPCTSTR CStringOp::GetString() const
{
	return m_s.data();
}

CStringOp::operator LPCTSTR() const
{
	return GetString();
}

CStringOp CStringOp::Format( LPCTSTR psz, ... )
{
	va_list argptr;
	va_start( argptr, psz );
	FormatV( psz, argptr );
	va_end( argptr );
	return *this;
}

CStringOp CStringOp::FormatV( LPCTSTR psz, va_list va )
{
	if ( psz )
	{
		CStringOp sfmt;
		size_t length = 4 * _tcslen( psz ) + 1;
		sfmt.m_s.insert( sfmt.m_s.begin(), length, 0 );

		while ( -1 == _vsntprintf( sfmt.m_s.data(), sfmt.m_s.size(), psz, va ) )
		{
			sfmt.m_s.insert( sfmt.m_s.end(), length, 0 );
		}

		sfmt.cutstr();
		sfmt.m_s.push_back( 0 );

		m_s = sfmt.m_s;
	}

	return *this;
}

TCHAR & CStringOp::operator[]( size_t n )
{
	return m_s[ n ];
}

CStringOp CStringOp::operator+( const CStringOp & x ) const
{
	CStringOp s = *this;
	auto v = std::find( x.m_s.begin(), x.m_s.end(), 0 );
	s.cutstr();
	s.m_s.reserve( x.GetLength() + 1 );
	s.m_s.insert( s.m_s.begin(), x.m_s.begin(), v );
	s.m_s.push_back( 0 );
	return s;
}

CStringOp CStringOp::operator+( LPCTSTR sz ) const
{
	return operator+( CStringOp( sz ) );
}

CStringOp & CStringOp::operator+=( const CStringOp & s )
{
	cutstr();
	m_s.reserve( m_s.size() + s.m_s.size() + 1 );
	m_s.insert( m_s.end(), s.m_s.begin(), std::find( s.m_s.begin(), s.m_s.end(), 0 ) );
	m_s.push_back( 0 );
	return *this;
}

CStringOp & CStringOp::operator+=( LPCTSTR psz )
{
	return operator+=( CStringOp( psz ) );
}

CStringOp & CStringOp::operator+=( TCHAR chr )
{
	cutstr();
	m_s.push_back( chr );
	m_s.push_back( 0 );
	return *this;
}

CStringOp operator+( LPCTSTR sz, const CStringOp & x )
{
	CStringOp s( sz );
	return s.operator+( x );
}

CStringOp CStringOp::operator+( TCHAR chr ) const
{
	CStringOp s = *this;
	if ( chr )
	{
		s.cutstr();
		s.m_s.push_back( chr );
		s.m_s.push_back( 0 );
	}
	return s;
}

CStringOp operator+( TCHAR chr, const CStringOp & x )
{
	CStringOp str = x;
	if ( chr )
	{
		str.m_s.insert( str.m_s.begin(), chr );
	}
	return str;
}

CStringOp CStringOp::Mid( size_t nFrom, size_t nTo ) const
{
	CStringOp s;
	size_t length = GetLength();
	// cutstr();
	// m_s.push_back( 0 );

	if ( size_t( -1 ) == nFrom && size_t( -1 ) == nTo || !length )
	{
		return *this;
	}
	else if ( size_t( -1 ) == nFrom )
	{
		nFrom = 0;
	}
	else if ( size_t( -1 ) == nTo || nTo >= length )
	{
		nTo = length;
	}
	else if ( nFrom >= nTo )
	{
		return TEXT( "" );
	}

	s.m_s.clear();
	s.m_s.insert( s.m_s.begin(), m_s.begin() + nFrom, m_s.begin() + nTo );

	return s;
}

