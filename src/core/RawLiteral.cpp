/*  Copyright 2015 Olivier Serve
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */
#include "RawLiteral.h"

#include "Variable.h"


// CONSTRUCTORS
RawLiteral::RawLiteral(int p_literal) :
m_literal(p_literal) {
}


// GETTERS
Id RawLiteral::id() const {
	if (m_literal >= 0)
		return m_literal;
	return - m_literal;
}


int RawLiteral::sign() const {
	if (m_literal >= 0)
		return SIGN_POSITIVE;
	return SIGN_NEGATIVE;
}


bool RawLiteral::isPositive() const {
	return sign() >= 0;
}


bool RawLiteral::isNegative() const {
	return sign() < 0;
}


// OPERATORS
RawLiteral RawLiteral::operator-() {
	return RawLiteral(-m_literal);
}


bool RawLiteral::operator==(const RawLiteral& p_literal) {
	return id() == p_literal.id();
}
