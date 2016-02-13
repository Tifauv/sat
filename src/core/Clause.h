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
#ifndef CLAUSE_H
#define CLAUSE_H

#include <vector>
#include "FormulaObject.h"


namespace sat {

class Literal;


/**
 * \brief Represents a Horn clause of a CNF formula.
 * A clause has an identifier and a list of literals.
 */
class Clause : public FormulaObject {
public:
	/**
	 * Initializes a clause.
	 *
	 * @param p_id
	 *            the clause identifier.
	 */
	explicit Clause(Id p_id);


	/**
	 * Cleans the memory used by the clause.
	 */
	~Clause();


	/**
	 * Adds a literal to the clause.
	 *
	 * @param p_literal
	 *            the literal to add
	 */
	void addLiteral(const Literal& p_literal);


	/**
	 * Removes a literal from the clause.
	 *
	 * @param p_literal
	 *            a reference to the literal to remove.
	 */
	void removeLiteral(const Literal& p_literal);


	/**
	 * Tells whether the clause is unsatisfiable.
	 * This is the case when it has no more literal.
	 *
	 * @return true if the clause is unsatisfiable (no more literal),
	 *         false otherwise
	 */
	bool isUnsatisfiable() const;


	/**
	 * Tells whether the clause is unary (has only one literal).
	 *
	 * @return true if the clause has exactly one literal,
	 *         false otherwise.
	 */
	bool isUnary() const;


	/**
	 * Gives the first literal of the clause.
	 *
	 * @return the first literal of the clause,
	 *         or a literal pointing to the nullptr variable if the clause is empty
	 */
	Literal firstLiteral() const;


	/**
	 * Gives an iterator on the first literal of the clause.
	 *
	 * @return a 'begin' iterator of the literals
	 */
	std::vector<Literal>::const_iterator beginLiteral() const;


	/**
	 * Gives an iterator of the end of the literals list.
	 *
	 * @return an 'end' iterator of the literals
	 */
	std::vector<Literal>::const_iterator endLiteral()   const;


private:
	/** The literals of the clause. */
	std::vector<Literal> m_literals;
};

} // namespace sat

#endif // CLAUSE_H
