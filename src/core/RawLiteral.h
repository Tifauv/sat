/*  Copyright (c) 2015 Olivier Serve
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
#ifndef RAWLITERAL_H
#define RAWLITERAL_H

#include "FormulaObject.h"


class RawLiteral {
public:
	explicit RawLiteral(int p_literal);

	Id id() const;
	int sign() const;

	bool isPositive() const;
	bool isNegative() const;
	
	RawLiteral operator-();
	bool operator==(const RawLiteral& p_literal);

private:
	int m_literal;
};

#endif // RAWLITERAL_H