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
#ifndef LITERAL_H
#define LITERAL_H

#include "variable.h"

class Variable;
class Clause;


class Literal {
public:
	Literal();
	Literal(Variable* p_variable, int p_sign);
	Literal(const Literal& p_literal);
	~Literal();

	Variable* var()  const;
	Id        id()   const;
	int       sign() const;

	bool isPositive() const;
	bool isNegative() const;

	std::list<Clause*>::iterator beginOccurence();
	std::list<Clause*>::iterator endOccurence();
	std::list<Clause*>::iterator erase(std::list<Clause*>::iterator);

	std::list<Clause*>::iterator beginOppositeOccurence();
	std::list<Clause*>::iterator endOppositeOccurence();
	std::list<Clause*>::iterator eraseOpposite(std::list<Clause*>::iterator);

	Literal operator-();
	bool operator==(const Literal& p_literal);
	bool operator!=(const Literal& p_literal);
	
private:
	Variable* m_variable;
	int m_sign;
};

#endif // LITERAL_H
