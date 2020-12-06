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

/* CExprParserException types */

#pragma once

#include "CStringOp.h"
#include <functional>
#include <map>
#include <vector>

class CExprParserException
{
	const size_t		m_uChar;
	const CStringOp		m_sMessage;

public:
	CExprParserException( LPCTSTR pszMessage, size_t u = size_t( -1 ) );

	size_t				AtChar();
	CStringOp			Message();
};

class CExprParserDivisionByZero : public CExprParserException
{
public:
	CExprParserDivisionByZero( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Division by zero" ), uChar ) {}
};

class CExprParserBracketMismatch : public CExprParserException
{
public:
	CExprParserBracketMismatch( size_t uChar = size_t( -1 ) ) 
		: CExprParserException( TEXT( "Mismatched brackets" ), uChar ) {}
};

class CExprParserNoSuchVariable : public CExprParserException
{
public:
	CExprParserNoSuchVariable( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Bad variable" ), uChar ) {}
};

class CExprParserNoSuchFunction : public CExprParserException
{
public:
	CExprParserNoSuchFunction( LPCTSTR pszFuncname, size_t uChar = size_t( -1 ) )
		: CExprParserException( CStringOp().Format( TEXT( "No such function '%s'" ), pszFuncname ), uChar ) {}
};

class CExprParserNoSuchToken: public CExprParserException
{
public:
	CExprParserNoSuchToken( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Bad token" ), uChar ) {}
};

class CExprParserInvalidNumeric : public CExprParserException
{
public:
	CExprParserInvalidNumeric( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Bad numeric token" ), uChar ) {}
};

class CExprParserWrongArguments : public CExprParserException
{
public:
	CExprParserWrongArguments( size_t uRequiredArguments, size_t nGiven, size_t uChar = size_t( -1 ) )
		: CExprParserException( CStringOp().Format( TEXT( "Function requires %ld arguments but %ld given" ), uRequiredArguments, nGiven ), uChar ) {}
};

class CExprParserTopLevelArgumentsNotAllowed : public CExprParserException
{
public:
	CExprParserTopLevelArgumentsNotAllowed( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Arguments not allowed at the top level expression" ), uChar ) { };
};

class CExprParserTreeAlreadyInitialized : public CExprParserException
{
public:
	CExprParserTreeAlreadyInitialized( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Parser's tree leaf is already initialized" ), uChar ) { };
};

class CExprParserTreeNotInitialized : public CExprParserException
{
public:
	CExprParserTreeNotInitialized( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Parser's tree leaf was not properly initialized" ), uChar ) { };
};

class CExprParserUnexpectedEndOfExpression : public CExprParserException
{
public:
	CExprParserUnexpectedEndOfExpression( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Unexpected end of expression" ), uChar ) { };
};

class CExprParserCantAssignNumeric: public CExprParserException
{
public:
	CExprParserCantAssignNumeric( size_t uChar = size_t( -1 ) )
		: CExprParserException( TEXT( "Can't assign value to an numeric operand" ), uChar ) { }
};
