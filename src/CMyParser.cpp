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

/* An example of derived from universal parser class. Parsing the complex numbers expressions */

#include "CMyParser.h"

#define M_PIC 	3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120

#define D(x, a)		{}; // printf("%s(%Lf,%Lf)\n", (x), (a).v.real(), (a).v.imag() ); }
#define D2(x, a, b)	{}; // printf("(%Lf,%Lf)%s(%Lf,%Lf)\n", (a).v.real(), (a).v.imag(), (x), (b).v.real(), (b).v.imag() ); }
#define ASSERT_UNDEF(a)	{ if ( (a).undef ) throw CExprParserException(CStringOp().Format(TEXT("The variable %s is undefined"), a.name.GetString()).GetString()); }
#define ASSERT_NOTVAR(a) { if ( !(a).var ) throw CExprParserException(TEXT("Assignment is not valid for non-variable tokens")); }

CMyParser::CMyParser()
	: CExprParser<TOK>( _T('('), _T(')'), _T(','), 0 )
{
	auto opPlus = []( TOK & a, TOK & b ) { D2("+", a, b); ASSERT_UNDEF(a); ASSERT_UNDEF(b); return a.v + b.v; };
	auto opSubs = []( TOK & a, TOK & b ) { D2("-", a, b); ASSERT_UNDEF(a); ASSERT_UNDEF(b); return a.v - b.v; };
	auto opMult = []( TOK & a, TOK & b ) { D2("*", a, b); ASSERT_UNDEF(a); ASSERT_UNDEF(b); return a.v * b.v; };
	auto opDivd = []( TOK & a, TOK & b ) { D2("/", a, b); ASSERT_UNDEF(a); ASSERT_UNDEF(b); return a.v / b.v; };
	auto opPow = []( TOK & a, TOK & b ) { D2("^", a, b); ASSERT_UNDEF(a); ASSERT_UNDEF(b); return std::pow(a.v, b.v); };
	auto opSemicolon = []( TOK & a, TOK & b ) { D2(";", a, b); ASSERT_UNDEF(a); ASSERT_UNDEF(b); return b; };
	auto opEqu = []( TOK & a, TOK & b ) { D2("=", a, b); ASSERT_NOTVAR(a); ASSERT_UNDEF(b); a = b; return a; };

	auto unPlus = []( const TOK & a ) { D("+", a); ASSERT_UNDEF(a); return a; };
	auto unNegt = []( const TOK & a ) { D("-", a); ASSERT_UNDEF(a); return -a.v; };
	auto unRevr = []( const TOK & a ) { D("~", a); ASSERT_UNDEF(a); return TOK( a.v.real(), -a.v.imag() ); };
	auto unFact = []( const TOK & a )
		{
			D("!", a);
			ASSERT_UNDEF(a);
			if ( abs(a.v.imag()) < 1e-11 )
			{
				return std::tgamma( a.v.real() + 1.0 );
			}
			else
			{
				throw CExprParserException(TEXT("Factorial takes only real numbers"));
			}
		};

	auto fsin = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::sin( varg[0].v ); };
	auto fcos = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::cos( varg[0].v ); };
	auto ftan = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::tan( varg[0].v ); };
	auto fctan = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return TOK(1.0, 0.0).v / std::tan( varg[0].v ); };
	auto fasin = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::asin( varg[0].v ); };
	auto facos = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::acos( varg[0].v ); };
	auto fatan = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::atan( varg[0].v ); };
	auto factan = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::atan( TOK(1.0,0.0).v / varg[0].v ); };
	auto fpi = []( const std::vector<TOK> & varg ) { return M_PIC; };
	auto fe = []( const std::vector<TOK> & varg ) { return std::exp(1); };
	auto fexp = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::exp(varg[0].v); };
	auto fsqrt = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::sqrt(varg[0].v); };
	auto fcbrt = []( const std::vector<TOK> & varg ) { ASSERT_UNDEF(varg[0]); return std::pow(varg[0].v, 1.0/3.0); };

	AddOp( _T( '+' ), 10 ) = opPlus;
	AddOp( _T( '-' ), 10 ) = opSubs;
	AddOp( _T( '*' ), 5 ) = opMult;
	AddOp( nullptr, 5 ) = opMult;
	AddOp( _T( '/' ), 5 ) = opDivd;
	AddOp( _T( '^' ), 4, etaRightOriented ) = opPow;
	AddOp( _T( ';' ), 40, etaRightOriented ) = opSemicolon;
	AddOp( _T( '=' ), 20, etaRightOriented ) = opEqu;

	AddUnaryOp( _T('+'), TRUE, 10 ) = unPlus;
	AddUnaryOp( _T('-'), TRUE, 10 ) = unNegt;
	AddUnaryOp( _T('~'), TRUE, 1 ) = unRevr;
	AddUnaryOp( _T('!'), FALSE, 1 ) = unFact;

	AddFunc( TEXT("sin"), 1 ) = fsin;
	AddFunc( TEXT("cos"), 1 ) = fcos;
	AddFunc( TEXT("tg"), 1 ) = ftan;
	AddFunc( TEXT("ctg"), 1 ) = fctan;
	AddFunc( TEXT("arcsin"), 1 ) = fasin;
	AddFunc( TEXT("arccos"), 1 ) = facos;
	AddFunc( TEXT("arctg"), 1 ) = fatan;
	AddFunc( TEXT("arcctg"), 1 ) = factan;
	AddFunc( TEXT("pi"), 0 ) = fpi;
	AddFunc( TEXT("e"), 0 ) = fe;
	AddFunc( TEXT("exp"), 1 ) = fexp;
	AddFunc( TEXT("sqrt"), 1 ) = fsqrt;
	AddFunc( TEXT("cbrt"), 1 ) = fcbrt;
}

void CMyParser::ParseDouble( const CStringOp & sExpression, size_t & uAtChar, long double & d )
{
	size_t length = sExpression.GetLength();
	int mode = 0;		// 0 - before dot, 1 - after dot, 2 - after 'e'
	long double d1 = 0, d2 = 0, l = 10;
	int ex = 0, exSign = 1;
	bool fContinue = true;

	size_t u = uAtChar;

	for ( ; u < length && fContinue; ++u )
	{
		const TCHAR & s = sExpression[ LODWORD( u ) ];
		if ( s >= _T( '0' ) && s <= _T( '9' ) )
		{
			switch ( mode )
			{
				case 0:		// multiply for 10
					{
						d1 = 10 * d1 + static_cast<long double>( s - _T( '0' ) );
						break;
					}
				case 1:		// divide by 10
					{
						d2 = d2 + static_cast<long double>( s - _T( '0' ) ) / l;
						l *= 10;
						break;
					}
				case 2:		// exponenta
				case 3:
					{
						ex = 10 * ex + ( s - _T( '0' ) );
						// switch to mode '3' to identify complete exponenta
						mode = 3;
						break;
					}
			}
		}
		else
		{
			switch ( s )
			{
				case _T( '.' ):
					{
						if ( mode > 0 )	// duplicate dot? Bail out...
						{
							fContinue = false;
							break;
						}
						mode = 1;
						break;
					}
				case _T( 'e' ):
				case _T( 'E' ):
					{
						if ( mode > 1 )	// duplicate 'e'? Bail out...
						{
							fContinue = false;
							break;
						}
						mode = 2;

						// check for sign
						if ( u + 1 < length )
						{
							if ( _T( '-' ) == sExpression[ LODWORD( u + 1 ) ] )
							{
								exSign *= -1;
								u++;
							}
							else if ( _T( '+' ) == sExpression[ LODWORD( u + 1 ) ] )
							{
								u++;
							}
						}
						break;
					}
				default:
					{
						// unknown character
						fContinue = false;
						break;
					}
			}
		}

		if ( !fContinue )
		{
			break;
		}
	}

	if ( u > uAtChar && 2 != mode )	// complete exponenta have mode==3
	{
		uAtChar = u;
		d = ( d1 + d2 )*std::pow( 10, exSign*ex );
	}
	else
	{
		throw CExprParserInvalidNumeric();
	}
}

void CMyParser::ParseNumeric( const CStringOp & sExpression, size_t & uAtChar, TOK & d)
{
	size_t length = sExpression.GetLength();
	if ( sExpression[ uAtChar ] == _T('i') )
	{
		uAtChar++;
		d = TOK(0.0, 1.0);
		return;
	}
	else
	{
		long double dl = 0.0;
		ParseDouble( sExpression, uAtChar, dl );
		d = TOK(dl, 0.0);
		return;
	}

	throw CExprParserInvalidNumeric();
}

BOOL CMyParser::IsVariable( const CStringOp & sExpression, size_t & uAtChar, TOK & dPossibleValue )
{
	size_t length = sExpression.GetLength();
	size_t i = uAtChar, c = 0;
	
	for(; i < length; ++i, ++c)
	{
		const TCHAR & chr = sExpression[i];
		if ( c < 1 )
		{
			if ( chr >= _T('A') && chr <= _T('Z') || chr >= _T('a') && chr <= _T('z') || chr == _T('_') )
			{
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			if ( chr >= _T('A') && chr <= _T('Z') || chr >= _T('a') && chr <= _T('z') || chr >= _T('0') && chr <= _T('9') || chr == _T('_') )
			{
				continue;
			}
			else
			{
				break;
			}
		}
	}

	if ( c < 1 || c == 1 && sExpression[ uAtChar ] == _T('i') )	// dont allow to use 'i' as variable
	{
		return FALSE;
	}

	dPossibleValue = TOK(0.0, 0.0);
	dPossibleValue.undef = TRUE;
	dPossibleValue.var = TRUE;
	dPossibleValue.name = sExpression.Mid(uAtChar, c);
	uAtChar = i;
	return TRUE;
}
