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
#ifndef VALUATION_H
#define VALUATION_H

#include "Literal.h"
#include <list>
#include <ostream>


namespace sat {
namespace solver {


/**
 * A SAT valuation is a list of literals.
 * Additionnaly, it can be marked unsatisfiable.
 */
class Valuation {
public:
	/**
	 * Creates an empty satisfiable valuation.
	 */
	Valuation();

	/**
	 * Tells whether this valuation is satisfiable.
	 */
	bool isSatisfiable() const;

	/**
	 * Tells whether this valuation is unsatisfiable.
	 */
	bool isUnsatisfiable() const;

	/**
	 * Sets the valuation satisfiable.
	 */
	void setSatisfiable();

	/**
	 * Sets the valuation unsatisfiable.
	 */
	void setUnsatisfiable();

	/**
	 * Gives a reference to the inner list of literals.
	 */
	std::list<Literal>& getLiterals();

	/**
	 * Appends a literal.
	 * 
	 * @param p_literal
	 *            the literal to add
	 */
	void push(Literal p_literal);

	/**
	 * Gives the last literal.
	 * This is the one that would be removed by a call to {@link #pop()}.
	 *
	 * @return the last literal
	 */
	Literal top() const;

	/**
	 * Removes the last literal.
	 */          
	void pop();

	/**
	 * Logs the valuation.
	 */
	void log();

	/**
	 * Prints the valuation to the given stream.
	 */
	friend std::ostream& operator<<(std::ostream& p_outStream, const Valuation& p_valuation);


private:
	/** The satisfiability flag. */
	bool m_unsatisfiable;
	
	/** The list of literals. */
	std::list<Literal> m_literals;
};

} // namespace sat::solver
} // namespace sat

#endif // VALUATION_H
