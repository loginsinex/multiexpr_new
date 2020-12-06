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

/* Template of universal parser */

#pragma once

#include "CExprParser.h"

typedef enum _tagEXPR_TOKEN_ASSOC
{
	etaLeftOriented,
	etaRightOriented
} EXPR_TOKEN_ASSOC, *PEXPR_TOKEN_ASSOC;

typedef enum _tagEXPR_TOKEN_TYPE
{
	ettNone,
	ettUnaryPre,
	ettNumber,
	ettVariable,
	ettUnaryPost,
	ettUnaryPostFunc,
	ettOpToken,
	ettFunc,
	ettBraceLeft,
	ettBraceRight,
	ettComma
} EXPR_TOKEN_TYPE, *PEXPR_TOKEN_TYPE;

template <class FUNC>
class CExprToken
{

protected:
	FUNC					m_tokFunc;

	CExprToken( )
		: m_tokFunc( nullptr )
	{
	}

public:
	FUNC &					TokFunc()
	{
		return m_tokFunc;
	}

	const FUNC &			Func() const
	{
		return m_tokFunc;
	}
};

template <class NUM>
class CExprParser;

template <class FUNC>
class CExprTokenOperator: public CExprToken<FUNC>
{
	int						m_prio;

protected:
	CExprTokenOperator( int prio = -1 )
		: m_prio( prio ) {  }

public:
	int								Prio()	const
	{
		return m_prio;
	}
};

template <class NUM>
class CExprTokenOp: public CExprTokenOperator<std::function<NUM( NUM&, NUM& )>>
{
	EXPR_TOKEN_ASSOC				m_eta;

public:
	CExprTokenOp( int prio = -1, EXPR_TOKEN_ASSOC eta = etaLeftOriented )
		: CExprTokenOperator<std::function<NUM( NUM&, NUM& )>>( prio ), m_eta( eta ) {}

	bool							IsHigher( const CExprTokenOp<NUM> & tok ) const
	{
		if ( etaRightOriented == m_eta )
		{
			return ( this->Prio() > tok.Prio() );
		}

		return ( this->Prio() >= tok.Prio() );
	}

	EXPR_TOKEN_ASSOC				Assoc()
	{
		return m_eta;
	}
};

template <class NUM>
class CExprTokenUn: public CExprTokenOperator<std::function<NUM( const NUM& )>>
{
public:
	CExprTokenUn( int prio = -1 )
		: CExprTokenOperator<std::function<NUM( const NUM& )>>( prio ) {}
};

template <class NUM>
class CExprTokenFunc: public CExprToken<std::function<NUM( const std::vector < NUM >& )>>
{
	size_t				m_nArgs;

public:
	CExprTokenFunc( size_t nArgs = 0 )
		: m_nArgs( nArgs ) {}

	size_t				Args() const
	{
		return m_nArgs;
	}
};

template <class NUM>
struct PARSER_TREE
{
	size_t													uAtChar;
	EXPR_TOKEN_TYPE											ett;
	NUM														dValue;
	std::vector<CExprTokenUn<NUM>>							uPreOp;
	std::vector<CExprTokenUn<NUM>>							uPostOp;
	CStringOp												sVariableId;

	CExprTokenOp<NUM>										op;

	struct
	{
		std::function<NUM( const std::vector<NUM> & )>		func;
		size_t												nargs;
	} fn;

	PARSER_TREE( size_t _uAtChar = size_t( -1 ) )
		: uAtChar( _uAtChar ), ett( ettNone )
	{
		fn.nargs = size_t( 0 );
		fn.func = nullptr;
	}

	PARSER_TREE( const NUM & _dValue, size_t _uAtChar = size_t( -1 ) )
		: uAtChar( _uAtChar ), dValue( _dValue ), ett( ettNumber )
	{
		fn.nargs = size_t( 0 );
		fn.func = nullptr;
	}

	PARSER_TREE( const NUM & _dValue, const std::vector<CExprTokenUn<NUM>> & _uPreOp, const std::vector<CExprTokenUn<NUM>> & _uPostOp, size_t _uAtChar = size_t( -1 ) )
		: uAtChar( _uAtChar ), dValue( _dValue ), ett( ettNumber ), uPreOp( _uPreOp ), uPostOp( _uPostOp )
	{
		fn.nargs = size_t( 0 );
		fn.func = nullptr;
	}

	PARSER_TREE( const CExprTokenOp<NUM> & _op, size_t _uAtChar = size_t( -1 ) )
		: uAtChar( _uAtChar ), op( _op ), ett( ettOpToken )
	{
		fn.nargs = size_t( 0 );
		fn.func = nullptr;
	}

	PARSER_TREE( const CExprTokenFunc<NUM> & _fn, size_t _uAtChar = size_t( -1 ) )
		: uAtChar( _uAtChar ), ett( ettFunc )
	{
		fn.func = _fn.Func();
		fn.nargs = _fn.Args();
	}

	PARSER_TREE( const CExprTokenFunc<NUM> & _fn, const std::vector<CExprTokenUn<NUM>> & _uPreOp, const std::vector<CExprTokenUn<NUM>> & _uPostOp, size_t _uAtChar = size_t( -1 ) )
		: uAtChar( _uAtChar ), ett( ettFunc ), uPreOp( _uPreOp ), uPostOp( _uPostOp )
	{
		fn.func = _fn.Func();
		fn.nargs = _fn.Args();
	}
};


template <class NUM>
class CExprParser
{
	const			TCHAR						m_opLeftBrace;
	const			TCHAR						m_opRightBrace;
	const			TCHAR						m_opComma;
	const			TCHAR						m_opVariable;

	CStringOp		m_sExpression;
	NUM				m_dResult;

	std::vector<PARSER_TREE<NUM>>				m_vop;

	struct
	{
		std::map < BOOL, std::map<CStringOp, CExprTokenUn<NUM>>>	vUnary;
		std::map<CStringOp, CExprTokenFunc<NUM>>					vFunc;
		std::map<CStringOp, CExprTokenOp<NUM>>						vOp;
		std::map<CStringOp, NUM>									mvarList;

		struct
		{
			CExprTokenOp<NUM>			vEmptyOp;
			CExprTokenUn<NUM>			vEmptyUOp;
			CExprTokenFunc<NUM>			vEmptyFunc;
		} unused;
	} m_token;

	VOID			PreParse()
	{
		size_t c = m_sExpression.GetLength();
		CStringOp & s = m_sExpression;

		int _lBracket = 0;

		for ( size_t i = 0; i < c; ++i )
		{
			const TCHAR & chr = s[ i ];
			if ( chr == m_opLeftBrace )
			{
				_lBracket++;
			}
			else if ( chr == m_opRightBrace )
			{
				_lBracket--;
			}

			if ( _lBracket < 0 )
			{
				throw CExprParserBracketMismatch( i );
			}
		}

		if ( _lBracket )
		{
			throw CExprParserBracketMismatch( s.GetLength() - 1 );
		}
	}


	BOOL			PartialCompare( LPCTSTR pszStr1, LPCTSTR pszStr2 )
	{
		size_t l = 0;
		do
		{
			const TCHAR & t1 = pszStr1[ l ];
			const TCHAR & t2 = pszStr2[ l ];
			if ( !t1 || !t2 )
			{
				return ( !t1 );
			}
			else if ( t1 != t2 )
			{
				break;
			}
		} while ( ++l );

		return FALSE;
	}

	template <class T>
	BOOL			FindDup( const std::map<CStringOp, T> & tok, LPCTSTR pszName )
	{
		return ( tok.find( pszName ) != tok.end() );
	}

	const CExprTokenFunc<NUM> & FindFunc( size_t & uAtChar, CStringOp & key )
	{
		return FindToken( uAtChar, m_token.vFunc, key );
	}

	virtual BOOL	IsVariable( const CStringOp & sExpression, size_t & uAtChar, NUM & dPossibleValue )
	{
		return FALSE;
	}

	virtual BOOL	IsFunction(
		const CStringOp & sExpression,
		size_t & uAtChar,
		std::function<NUM( const std::vector<NUM> & vargs )> & fn, size_t & nArgs )
	{
		return FALSE;
	}

	BOOL			TestFunction( size_t & uAtChar, PARSER_TREE<NUM> & pt )
	{
		size_t uAtNewChar = 0;

		const CExprTokenFunc<NUM> & func = FindFunc( uAtChar, pt.sVariableId );
		pt.fn.nargs = func.Args();
		pt.fn.func = func.Func();

		if ( !IsNextChar( uAtChar, m_opLeftBrace ) || !pt.fn.func )
		{
			uAtNewChar = uAtChar;
			if ( IsFunction( m_sExpression, uAtNewChar, pt.fn.func, pt.fn.nargs ) )
			{
				pt.sVariableId = m_sExpression.Mid( uAtChar, uAtNewChar );
				if ( pt.sVariableId.GetLength() > 0 && pt.fn.func )
				{
					AddFunc( pt.sVariableId.GetString(), pt.fn.nargs ) = pt.fn.func;
					pt.ett = ettFunc;
					uAtChar = uAtNewChar;
					return TRUE;
				}
			}
		}
		else
		{
			pt.ett = ettFunc;
			return TRUE;
		}

		return FALSE;
	}

	BOOL			TestVariable( size_t & uAtChar, CStringOp & key, PARSER_TREE<NUM> & pt )
	{
		BOOL fVariableFound = FALSE;
		try
		{
			FindToken( uAtChar, m_token.mvarList, pt.sVariableId );
			pt.ett = ettVariable;
			return TRUE;	
			// variable was found, but user parser can find another
			// for example our can be 'var' and user finds 'variable'
		}
		catch ( CExprParserNoSuchToken & e )
		{
			UNREFERENCED_PARAMETER( e );
		}

		size_t uAtNewChar = uAtChar;
		NUM dPossibleValue;
		if ( IsVariable( m_sExpression, uAtNewChar, dPossibleValue ) )
		{
			CStringOp key = m_sExpression.Mid( uAtChar, uAtNewChar );
			if ( key.GetLength() > 0 )
			{
				pt.ett = ettVariable;

				// if variable not found in our list, or new variable is bigger than our
				// then return NEW variable
				if ( !fVariableFound || key.GetLength() > pt.sVariableId.GetLength() )
				{
					uAtChar = uAtNewChar;
					pt.sVariableId = key;
					return AddVariable( key.GetString(), dPossibleValue );
				}
			}
		}

		return fVariableFound;
	}

	BOOL			FindOp( size_t & uAtChar, PARSER_TREE<NUM> & pt )
	{
		try
		{
			CStringOp key;
			const CExprTokenOp<NUM> & op = FindToken( uAtChar, m_token.vOp, key );
			pt.op = op;
			pt.ett = ettOpToken;
			pt.sVariableId = key;
			return TRUE;
		}
		catch ( CExprParserException & e )
		{
			UNREFERENCED_PARAMETER( e );
		}
		return FALSE;
	}

	template <class T>
	const T&		FindToken( size_t & uAtChar, const std::map<CStringOp, T> & tok, CStringOp & key )
	{
		CStringOp s = AtChar( uAtChar );
		LPCTSTR sz = s.GetString();
		size_t length = s.GetLength();

		// find the biggest string for compare
		auto v = tok.rbegin();
		for ( ; v != tok.rend(); v++ )
		{
			if ( v->first.GetLength() <= length )
			{
				break;
			}
		}

		// find token for this string
		for ( ; v != tok.rend(); v++ )
		{
			if ( PartialCompare( v->first.GetString(), sz ) )
			{
				uAtChar += v->first.GetLength();
				key = v->first;
				return v->second;
			}
		}

		throw CExprParserNoSuchToken();
	}

	virtual void	ParseNumeric( const CStringOp & sExpression, size_t & uAtChar, NUM & d ) PURE;

	VOID					ParseExpression( std::vector<PARSER_TREE<NUM>> & tree )
	{
		size_t uAtChar = 0;
		try
		{
			tree.clear();
			ParseLoop( uAtChar, tree );

			if ( tree.size() < 1 )
			{
				throw CExprParserException( TEXT( "Internal error" ) );
			}
		}
		catch ( CExprParserException & e )
		{
			throw CExprParserException( e.Message().GetString(), uAtChar );
		}
	}

	VOID					EvaluateToken( const PARSER_TREE<NUM> & pt, const PARSER_TREE<NUM> & p1, const PARSER_TREE<NUM> & p2, PARSER_TREE<NUM> & result )
	{
		std::vector<CExprTokenUn<NUM>> uPreOp;
		const int ptPrio = pt.op.Prio();
		NUM dVal1, dVal2;
		uPreOp.reserve( p1.uPreOp.size() );

		auto v = std::find_if( p1.uPreOp.begin(), p1.uPreOp.end(), [ &ptPrio ]( const auto & pr ) { return pr.Prio() <= ptPrio; } );

		// all lower priorities prefix tokens will be added to the result
		result.uPreOp.insert( result.uPreOp.begin(), p1.uPreOp.begin(), v );
		// others will be calculated here
		uPreOp.insert( uPreOp.begin(), v, p1.uPreOp.end() );

		result.ett = ettNumber;
		result.dValue = pt.op.Func()(
			EvaluateOperand( p1, uPreOp, p1.uPostOp, dVal1 ), 
			EvaluateOperand( p2, p2.uPreOp, p2.uPostOp, dVal2 )
			);
	}

	NUM		&				EvaluateOperand( const PARSER_TREE<NUM> & p, const std::vector<CExprTokenUn<NUM>> & uPreOp, const std::vector<CExprTokenUn<NUM>> & uPostOp, NUM & dVal )
	{
		// for variable token we may return reference to that variable
		if ( p.ett == ettVariable )
		{
			// if no operators applied to it
			if ( uPreOp.empty() && uPostOp.empty() )
			{
				return m_token.mvarList.find( p.sVariableId )->second;
			}

			// otherwise return value
			dVal = m_token.mvarList.find( p.sVariableId )->second;
		}
		else
		{
			dVal = p.dValue;
		}

		
		for ( const auto & v : uPostOp )
			dVal = v.Func()( dVal );

		// reverse order for prefix operands
		for ( auto v = uPreOp.rbegin(); v != uPreOp.rend(); v++ )
			dVal = v->Func()( dVal );

		return dVal;
	}

	VOID			SkipSpaces( size_t & uAtChar )
	{
		while ( m_sExpression[ uAtChar ] == _T( ' ' ) ) uAtChar++;
	}

	// requires for function with 1 argument
	// causes to absence of comma in that case (as is in functions without arguments)
	VOID			ArgumentPresent( std::vector<size_t> & vfuncarg, std::vector<bool> & vfargpresent )
	{
		if ( vfuncarg.size() && !vfuncarg.back() )
		{
			vfuncarg.back()++;
		}

		if ( vfargpresent.size() )
		{
			vfargpresent.back() = TRUE;
		}
	}

	VOID			ParseLoop( size_t & uAtChar, std::vector<PARSER_TREE<NUM>> & tree )
	{
		size_t length = m_sExpression.GetLength();
		std::vector<PARSER_TREE<NUM>> stack;
		PARSER_TREE<NUM> pt;
		std::vector<size_t> vfuncArgs;
		std::vector<bool> vfargpresent;

		EXPR_TOKEN_TYPE etExpected = ettUnaryPre;

		uAtChar = 0;

		while ( uAtChar < length )
		{
			SkipSpaces( uAtChar );
			pt.uAtChar = uAtChar;
//			_tprintf(TEXT("Char[%ld] = %c, Expected: %d\n"), uAtChar, m_sExpression[uAtChar], etExpected);

			switch ( etExpected )
			{
				case ettUnaryPre:
					{
						try
						{
							CStringOp key;
							const auto & uOp = FindToken( uAtChar, m_token.vUnary[ TRUE ], key );
							pt.uPreOp.push_back( uOp );
#ifdef _DEBUG
							_tprintf( TEXT( "Found prefix operand: '%s'\n" ), key.GetString() );
#endif
							// the next token is can be again UnaryPre
						}
						catch ( CExprParserNoSuchToken & e )
						{
							UNREFERENCED_PARAMETER( e );
							etExpected = ettNumber;	// seems to be a variable or number or function?
						}
						break;

					}
				case ettUnaryPost:
					{
						try
						{
							CStringOp key;
							const auto & uOp = FindToken( uAtChar, m_token.vUnary[ FALSE ], key );
							pt.uPostOp.push_back( uOp );
#ifdef _DEBUG
							_tprintf( TEXT( "Found postfix operand: '%s'\n" ), key.GetString() );
#endif
							// still unary post...
						}
						catch ( CExprParserNoSuchToken & e )
						{
							UNREFERENCED_PARAMETER( e );
							etExpected = ettComma;
							SortToken( tree, stack, pt );
						}
						break;
					}
				case ettUnaryPostFunc:
					{
						if ( !tree.size() )
						{
							throw CExprParserNoSuchToken();
						}

						PARSER_TREE<NUM> & fnpt = tree.back();
						if ( fnpt.ett != ettFunc )
						{
							throw CExprParserNoSuchToken();
						}

						try
						{
							CStringOp key;
							const auto & uOp = FindToken( uAtChar, m_token.vUnary[ FALSE ], key );
							fnpt.uPostOp.push_back( uOp );
#ifdef _DEBUG
							_tprintf( TEXT( "Found postfix function operand: '%s'\n" ), key.GetString() );
#endif
							// still unary post...
						}
						catch ( CExprParserNoSuchToken & e )
						{
							UNREFERENCED_PARAMETER( e );
							etExpected = ettComma;
						}
						break;
					}
				case ettNumber:
					{
						// user parser MUST throw exception if its not a number
						try
						{
							size_t uNewAtChar = uAtChar;
							ParseNumeric( m_sExpression, uNewAtChar, pt.dValue );
							uAtChar = uNewAtChar;
							pt.ett = ettNumber;
							etExpected = ettUnaryPost;
							ArgumentPresent( vfuncArgs, vfargpresent );
						}
						catch ( CExprParserException & e )
						{
							UNREFERENCED_PARAMETER( e );
							etExpected = ettFunc;
						}
						break;
					}
				case ettFunc:
					{
						// if begins not from variable char, test as function
						if ( ( !m_opVariable || m_opVariable != m_sExpression[ uAtChar ] ) && TestFunction( uAtChar, pt ) )
						{
							SortToken( tree, stack, pt );
							etExpected = ettUnaryPre;

							// we've a new argument
							ArgumentPresent( vfuncArgs, vfargpresent );
							// push the new value of args to the func args stack
							vfuncArgs.push_back( 0 );
							vfargpresent.push_back( FALSE );

							SkipSpaces( uAtChar );

							// function without arguments list?
							if ( m_sExpression[ uAtChar ] == m_opRightBrace )
							{
								// go to the ettBraceCheck
								etExpected = ettBraceRight;
							}
						}
						else
						{
							etExpected = ettVariable;
						}
						break;
					}
				case ettVariable:
					{
						if ( !m_opVariable || IsNextChar( uAtChar, m_opVariable ) )
						{
							// search for variable
							if ( TestVariable( uAtChar, pt.sVariableId, pt ) )
							{
								etExpected = ettUnaryPost;
								ArgumentPresent( vfuncArgs, vfargpresent );
							}
							else
							{
								// was defined as variable, but this variable was not found
								throw CExprParserNoSuchVariable();
							}
						}
						else
						{
							// its a not function, not a number and not a variable
							if ( vfuncArgs.size() && vfuncArgs.back() > 0 )
							{
								// we're from comma, and from comma no operands found 
								throw CExprParserUnexpectedEndOfExpression();
							}
							else
							{
								// here we're from function
								// but test for variable mark is fails
								// so throw non-existed function
								throw CExprParserNoSuchToken();
							}
						}
						break;
					}
				case ettComma:
					{
						if ( IsNextChar( uAtChar, m_opComma ) )
						{
							pt.ett = ettComma;
							SortToken( tree, stack, pt );
							etExpected = ettUnaryPre;

							// in the func args stack MUST be a value
							if ( !vfuncArgs.size() )
							{
								throw CExprParserNoSuchToken();
							}

							vfuncArgs.back()++;
							vfargpresent.back() = FALSE;
						}
						else
						{
							// its not a comma, close brace?
							etExpected = ettBraceRight;
						}
						break;
					}
				case ettBraceRight:
					{
						if ( IsNextChar( uAtChar, m_opRightBrace ) )
						{
							// so pop number of args from the stack
							size_t nlastArgs = vfuncArgs.back();
							BOOL fargpresent = vfargpresent.back();
							vfuncArgs.pop_back();
							vfargpresent.pop_back();
							pt.ett = ettBraceRight;
							SortToken( tree, stack, pt );
							etExpected = ettUnaryPostFunc;

							// function at the top of tree stack (checked in SortToken)
							// check for number of arguments for last function
							if ( nlastArgs > 0 && !fargpresent )	// was comma and no operand
							{
								throw CExprParserUnexpectedEndOfExpression();
							}
							else if ( nlastArgs != tree.back().fn.nargs )
							{
								// if function have (-1) as count of arguments,
								// so it can eat any number of arguments
								PARSER_TREE<NUM> & fnpt = tree.back();
								if ( fnpt.fn.nargs == size_t( -1 ) )
								{
									// put for this function calculated arguments
									fnpt.fn.nargs = nlastArgs;
								}
								else
								{
									throw CExprParserWrongArguments( fnpt.fn.nargs, nlastArgs );
								}
							}
						}
						else
						{
							// its not an end of function. Check for token
							etExpected = ettOpToken;
						}
						break;
					}
				case ettOpToken:
					{
						if ( FindOp( uAtChar, pt ) )
						{
							SortToken( tree, stack, pt );
							etExpected = ettUnaryPre;
						}
						else
						{
							throw CExprParserNoSuchToken();
						}

						break;
					}
				default:
					{
						// internal error
						throw CExprParserException( TEXT( "Internal error while parsing" ) );
					}
			}
		}


		// last token is lost, so add it
		switch ( pt.ett )
		{
			case ettNumber:
			case ettFunc:
			case ettVariable:
				{
					SortToken( tree, stack, pt );
					break;
				}
			default:
				{
					break;
				}
		}

		tree.insert( tree.end(), stack.rbegin(), stack.rend() );

#ifdef _DEBUG
		for ( const auto & v : tree )
		{
			_tprintf( TEXT( "Operand: %ld" ), v.ett );
			if ( v.ett == ettNumber )
			{
				_tprintf( TEXT( ", Value: %f\n" ), v.dValue );
			}
			else if ( v.ett == ettVariable )
			{
				_tprintf( TEXT( ", Variable: '%s'\n" ), v.sVariableId.GetString() );
			}
			else if ( v.ett == ettFunc )
			{
				_tprintf( TEXT( ", Function: '%s'\n" ), v.sVariableId.GetString() );
			}
			else if ( v.ett == ettOpToken )
			{
				_tprintf( TEXT( ", Token: '%s'\n" ), v.sVariableId.GetString() );
			}
			else
			{
				_tprintf( TEXT( "\n" ) );
			}
		}
		_tprintf( TEXT( "---\n\n" ) );
#endif
	}


	VOID			SortToken( std::vector<PARSER_TREE<NUM>> & tree, std::vector<PARSER_TREE<NUM>> & stack, PARSER_TREE<NUM> & op )
	{
		switch ( op.ett )
		{
			case ettNumber:
			case ettVariable:
				{
					tree.push_back( op );
					break;
				}
			case ettFunc:
				{
					stack.push_back( op );
					break;
				}
			case ettBraceRight:
				{
					while ( stack.size() > 0 )
					{
						const auto & back_op = stack.back();
						if ( back_op.ett != ettFunc )
						{
							tree.push_back( back_op );
							stack.pop_back();
						}
						else
						{
							break;
						}
					}

					if ( stack.size() > 0 )
					{
						tree.push_back( stack.back() );
						stack.pop_back();
					}
					else
					{
						throw CExprParserNoSuchToken();
					}
					break;
				}
			case ettOpToken:
				{
					while ( stack.size() > 0 && stack.back().ett == ettOpToken )
					{
						const PARSER_TREE<NUM> & back_op = stack.back();
						if ( op.op.IsHigher( back_op.op ) )
						{
							tree.push_back( back_op );
							stack.pop_back();
						}
						else
						{
							break;
						}
					}

					stack.push_back( op );
					break;
				}
			case ettComma:
				{
					while ( stack.size() > 0 )
					{
						const auto & back_op = stack.back();
						if ( back_op.ett != ettFunc )
						{
							tree.push_back( back_op );
							stack.pop_back();
						}
						else
						{
							break;
						}
					}

					if ( !stack.size() )
					{
						throw CExprParserNoSuchToken();
					}
					break;
				}
			default:
				{
					break;
				}
		}

		op = PARSER_TREE<NUM>();
	}

	BOOL			IsNextChar( size_t & uAtChar, TCHAR chr )
	{
		SkipSpaces( uAtChar );
		BOOL result = ( m_sExpression[ uAtChar ] == chr );

		if ( result && uAtChar + 1 <= m_sExpression.GetLength() )
		{
			uAtChar++;
		}

		return result;
	}

	CStringOp		AtChar( size_t uAtChar )
	{
		return m_sExpression.Mid( uAtChar );
	}

	VOID			PreEvaluate()
	{
		size_t cop = m_vop.size();
		std::vector<PARSER_TREE<NUM>> stack;
		stack.reserve( cop );

		for ( size_t n = 0; n < cop; ++n )
		{
			const PARSER_TREE<NUM> & pt = m_vop[ n ];

			try
			{
				switch ( pt.ett )
				{
					case ettNumber:
						{
							stack.push_back( pt );
							break;
						}
					case ettVariable:
						{
							stack.push_back( pt );
							break;
						}
					case ettFunc:
						{
							if ( stack.size() < pt.fn.nargs )
							{
								throw CExprParserWrongArguments( pt.fn.nargs, stack.size() );
							}

							auto v = stack.end() - pt.fn.nargs;
							NUM ev_result;
							// if one of operand in the stack is not a number, we cannot evaluate this function
							if ( std::find_if( v, stack.end(), [] ( const auto & pt ) { return pt.ett != ettNumber; } ) == stack.end() )
							{
								std::vector<NUM> args( pt.fn.nargs, 0 );
								std::generate( 
									args.begin(), args.end(), 
									[ &v, this, &ev_result ] { const auto & result = EvaluateOperand( *v, v->uPreOp, v->uPostOp, ev_result ); v++; return result; }
								);

								stack.erase( stack.end() - pt.fn.nargs, stack.end() );
								stack.push_back( PARSER_TREE<NUM>( pt.fn.func( args ), pt.uPreOp, pt.uPostOp, pt.uAtChar ) );
							}
							else
							{
								stack.push_back( pt );
							}

							break;
						}
					case ettOpToken:
						{
							if ( stack.size() < 2 )
							{
								throw CExprParserUnexpectedEndOfExpression( pt.uAtChar );
							}
							else
							{
								const PARSER_TREE<NUM> p2 = *( stack.rbegin() + 0 );
								const PARSER_TREE<NUM> p1 = *( stack.rbegin() + 1 );

								if ( p1.ett == ettNumber && p2.ett == ettNumber )
								{
									PARSER_TREE<NUM> result;
									stack.pop_back();	// remove operands from stack
									stack.pop_back();
									result.uAtChar = pt.uAtChar;
									EvaluateToken( pt, p1, p2, result );
									stack.push_back( result );
								}
								else
								{
									stack.push_back( pt );
								}
							}
							break;
						}
					default:
						{
							throw CExprParserException( TEXT( "Internal error while pre-evaluating" ), pt.uAtChar );
							break;
						}
				}
			}
			catch ( CExprParserException & e )
			{
				throw CExprParserException( e.Message(), e.AtChar() == size_t( -1 ) ? pt.uAtChar : e.AtChar() );
			}
		}

		if ( stack.size() > 0 )
		{
			m_vop = stack;
			return;
		}

		throw CExprParserNoSuchToken();
	}

	VOID			EvaluateVars()
	{
		size_t cop = m_vop.size();
		std::vector<PARSER_TREE<NUM>> stack;

		for ( size_t n = 0; n < cop; ++n )
		{
			const PARSER_TREE<NUM> & pt = m_vop[ n ];

			try
			{
				switch ( pt.ett )
				{
					case ettNumber:
						{
							stack.push_back( pt );
							break;
						}
					case ettVariable:
						{
							auto v = m_token.mvarList.find( pt.sVariableId );
							if ( v != m_token.mvarList.end() )
							{
								stack.push_back( pt );
							}
							else
							{
								throw CExprParserNoSuchToken( pt.uAtChar );
							}
							break;
						}
					case ettFunc:
						{
							if ( stack.size() < pt.fn.nargs )
							{
								throw CExprParserWrongArguments( pt.fn.nargs, stack.size() );
							}

							auto v = stack.end() - pt.fn.nargs;
							// all operands in the stack MUST be a numbers, otherwise - bail out
							if ( std::find_if( v, stack.end(), [] ( const auto & pt ) { return ( pt.ett != ettVariable && pt.ett != ettNumber ); } ) != stack.end() )
							{
								throw CExprParserNoSuchToken( v->uAtChar );
							}

							// fill args with values from stack
							NUM ev_result;
							std::vector<NUM> args( pt.fn.nargs, 0 );
							std::generate(
								args.begin(), args.end(), 
								[ &v, this, &ev_result ] { const auto & result = EvaluateOperand( *v, v->uPreOp, v->uPostOp, ev_result ); v++; return result; } 
							);
							stack.erase( stack.end() - pt.fn.nargs, stack.end() );
							stack.push_back( PARSER_TREE<NUM>( pt.fn.func( args ), pt.uPreOp, pt.uPostOp, pt.uAtChar ) );

							break;
						}
					case ettOpToken:
						{
							if ( stack.size() < 2 )
							{
								throw CExprParserUnexpectedEndOfExpression( pt.uAtChar );
							}
							else
							{
								const PARSER_TREE<NUM> p2 = stack.back(); stack.pop_back();
								const PARSER_TREE<NUM> p1 = stack.back(); stack.pop_back();

								if ( ( p1.ett == ettNumber || p1.ett == ettVariable ) && ( p2.ett == ettNumber || p2.ett == ettVariable ) )
								{
									PARSER_TREE<NUM> result;
									result.uAtChar = pt.uAtChar;
									EvaluateToken( pt, p1, p2, result );
									stack.push_back( result );
								}
								else if ( p1.ett != ettNumber && p1.ett != ettVariable )
								{
									throw CExprParserNoSuchToken( p1.uAtChar );
								}
								else
								{
									throw CExprParserNoSuchToken( p2.uAtChar );
								}
							}
							break;
						}
					default:
						{
							throw CExprParserException( TEXT( "Internal error while evaluating" ), pt.uAtChar );
							break;
						}
				}
			}
			catch ( CExprParserException & e )
			{
				throw CExprParserException( e.Message(), e.AtChar() == size_t( -1 ) ? pt.uAtChar : e.AtChar() );
			}
		}

		if ( stack.size() > 0 )
		{
			const PARSER_TREE<NUM> & pt = stack.back();
			if ( pt.ett == ettNumber || pt.ett == ettVariable )
			{
				m_dResult = EvaluateOperand( pt, pt.uPreOp, pt.uPostOp, m_dResult );
				return;
			}
		}

		throw CExprParserNoSuchToken();
	}

	template <class T>
	T & AddToken( LPCTSTR psz, std::map<CStringOp, T> & mtok, T & emptyTok, const T & tok, BOOL fAllowEmpty = FALSE )
	{
		if ( ( !psz || !psz[ 0 ] ) && !fAllowEmpty )
		{
			return emptyTok;
		}

		mtok[ psz ] = tok;
		return mtok.find( psz )->second;
	}

	template <class T>
	T & AddToken( TCHAR u, std::map<CStringOp, T> & mtok, T & emptyTok, const T & tok, BOOL fAllowEmpty = FALSE )
	{
		TCHAR psz[] = { u, 0 };
		return AddToken( psz, mtok, emptyTok, tok, fAllowEmpty );
	}

protected:
	CExprParser( TCHAR opLeftBrace = _T( '(' ), TCHAR opRightBrace = _T( ')' ), TCHAR opComma = _T( ',' ), TCHAR opVariable = _T( '$' ) )
		: m_sExpression( TEXT( "" ) ),
		m_dResult( 0.0 ),
		m_opLeftBrace( opLeftBrace ), m_opRightBrace( opRightBrace ), m_opComma( opComma ), m_opVariable( opVariable )
	{
		
		CExprTokenFunc<NUM> fn( 1 );
		fn.TokFunc() = 
			[]( const std::vector<NUM> & vx )
			{
				return vx[ 0 ];
			};

		m_token.vFunc[ TEXT( "" ) ] = fn;
	}

	virtual ~CExprParser() {}

	std::function<NUM( NUM&, NUM& )> & AddOp( TCHAR u, int prio, EXPR_TOKEN_ASSOC eta = etaLeftOriented ) //, const std::function<NUM( const NUM &, const NUM& )> & expr, int prio )
	{
		CExprTokenOp<NUM> op( prio, eta );
		return AddToken( u, m_token.vOp, m_token.unused.vEmptyOp, op, TRUE ).TokFunc();
	}

	std::function<NUM( NUM&, NUM& )> & AddOp( LPCTSTR psz, int prio, EXPR_TOKEN_ASSOC eta = etaLeftOriented ) //, const std::function<NUM( const NUM &, const NUM& )> & expr, int prio )
	{
		CExprTokenOp<NUM> op( prio, eta );
		return AddToken( psz, m_token.vOp, m_token.unused.vEmptyOp, op, TRUE ).TokFunc();
	}

	std::function<NUM( const NUM& )> & AddUnaryOp( TCHAR u, BOOL fPrefix, int prio )
	{
		CExprTokenUn<NUM> unop( prio );

		return AddToken( u, m_token.vUnary[ !!fPrefix ], m_token.unused.vEmptyUOp, unop ).TokFunc();
	}

	std::function<NUM( const NUM& )> & AddUnaryOp( LPCTSTR psz, BOOL fPrefix, int prio )
	{
		CExprTokenUn<NUM> unop( prio );

		return AddToken( psz, m_token.vUnary[ !!fPrefix ], m_token.unused.vEmptyUOp, unop ).TokFunc();
	}

public:

	VOID			Compile( LPCTSTR pszExpression )
	{
		std::vector<PARSER_TREE<NUM>> tree;
		m_sExpression = pszExpression;
		m_vop.clear();

		PreParse();

		ParseExpression( tree );
		if ( tree.size() > 0 )
		{
			m_vop = tree;
			PreEvaluate();
		}
	}

	VOID			Variables( const std::map<size_t, NUM> & mvarList )
	{
		m_token.mvarList.clear();

		for ( const auto & v : mvarList )
		{
			AddVariable( v.first, v.second );
		}
	}

	BOOL			Evaluate()
	{
		if ( !m_sExpression.GetLength() )
		{
			return FALSE;
		}

		EvaluateVars();
		return TRUE;
	}

	VOID			Result( NUM & d )
	{
		d = m_dResult;
	}



	std::function<NUM( const std::vector<NUM>& )> & AddFunc( LPCTSTR pszName, size_t nArgsCount )
	{
		CExprTokenFunc<NUM> fn( nArgsCount );
		return AddToken( pszName, m_token.vFunc, m_token.unused.vEmptyFunc, fn ).TokFunc();
	}

	BOOL			AddVariable( LPCTSTR vId, const NUM & value )
	{
		m_token.mvarList[ vId ] = value;
		return TRUE;
	}

	BOOL			AddVariable( size_t vId, const NUM & value )
	{
		return AddVariable( CStringOp().Format( TEXT( "%ld" ), vId ).GetString(), value );
	}

	BOOL			RemoveVariable( size_t vId )
	{
		return RemoveVariable( CStringOp().Format( TEXT( "%ld" ), vId ).GetString() );
	}

	BOOL			RemoveVariable( LPCTSTR vId )
	{
		auto v = m_token.mvarList.find( vId );
		if ( v != m_token.mvarList.end() )
		{
			m_token.mvarList.erase( v );
			return TRUE;
		}
		return FALSE;
	}

	BOOL GetVariable( size_t vId, NUM & value )
	{
		return GetVariable( CStringOp().Format( TEXT( "%ld" ), vId ).GetString(), value );
	}

	BOOL GetVariable( LPCTSTR pszName, NUM & value )
	{
		auto v = m_token.mvarList.find( pszName );
		if ( v != m_token.mvarList.end() )
		{
			value = v->second;
			return TRUE;
		}
		return FALSE;
	}
};

