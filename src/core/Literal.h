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
#ifndef LITERAL_H
#define LITERAL_H

#include <memory>
#include "Variable.h"

using namespace std;

namespace sat {

class Clause;


class Literal {
public:
	Literal();
	Literal(const shared_ptr<Variable>& p_variable, int p_sign);
	Literal(const Literal& p_literal);

	shared_ptr<Variable> var()  const;
	Id        id()   const;
	int       sign() const;

	bool isPositive() const;
	bool isNegative() const;

	shared_ptr<Clause> occurence();
	shared_ptr<Clause> oppositeOccurence();

	Literal operator-();
	Literal& operator=(const Literal&) = default;
	bool operator==(const Literal& p_literal);
	bool operator!=(const Literal& p_literal);

private:
	shared_ptr<Variable> m_variable;
	int m_sign;
};

} // namespace sat

#endif // LITERAL_H
