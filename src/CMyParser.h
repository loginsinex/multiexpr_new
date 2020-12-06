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

/* Header of an example class */

#pragma once

#include "CExprParserTemplate.h"
#include <complex>
#include <math.h>

typedef struct _tag_CComplex
{
	BOOL				undef;
	std::complex<long double>	v;
	CStringOp			name;
	BOOL				var;

	_tag_CComplex(const std::complex<long double> & xv)
		: v( xv ), undef( FALSE ), var(FALSE) { }

	_tag_CComplex()
		: undef( FALSE ), var(FALSE) {}

	_tag_CComplex(long double real, long double imag)
		: v(real, imag), undef(FALSE), var(FALSE) {}

	_tag_CComplex(long double real)
		: v(real, 0.0), undef(FALSE), var(FALSE) {}
} CComplex, *PCComplex;


typedef CComplex TOK;

class CMyParser : public CExprParser<TOK>
{
	void ParseNumeric( const CStringOp & sExpression, size_t & uAtChar, TOK & d );
	void ParseDouble( const CStringOp & sExpression, size_t & uAtChar, long double & d );
	BOOL IsVariable( const CStringOp & sExpression, size_t & uAtChar, TOK & dPossibleValue );

public:
	CMyParser( );
};
