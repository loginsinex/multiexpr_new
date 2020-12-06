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

/* CExprParserException implementation */

#include "Controls.h"

CExprParserException::CExprParserException( LPCTSTR pszMessage, size_t u )
	: m_uChar( u ), m_sMessage( pszMessage ? pszMessage : TEXT( "" ) )
{

}

size_t CExprParserException::AtChar()
{
	return m_uChar;
}

CStringOp CExprParserException::Message()
{
	CStringOp msg;
	if ( m_uChar == size_t( -1 ) )
	{
		msg = m_sMessage;
	}
	else
	{
		msg.Format( TEXT( "%s at position %ld" ), m_sMessage.GetString(), m_uChar + 1 );
	}

	return msg.GetString();
}

