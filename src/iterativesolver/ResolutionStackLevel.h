/*  Copyright 2016 Olivier Serve
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
#ifndef RESOLUTION_STACK_LEVEL_H
#define RESOLUTION_STACK_LEVEL_H

#include <list>
#include <memory>
#include <string>
#include "History.h"


namespace sat {

using namespace history;

namespace solver {


/**
 * @brief One level of the resolution stack.
 * One level combines the literals asserted since the last decision level,
 * and the history to replay in case of backtrack.
 */
class ResolutionStackLevel {
public:
	/**
	 * Appends a literal.
	 *
	 * @param p_literal
	 *            the literal
	 */
	void pushLiteral(Literal p_literal);

	/**
	 * Gives the first literal.
	 * This is the literal that was selected by a decision.
	 *
	 * @return the last literal
	 */
	Literal firstLiteral() const;

	/**
	 * Returns the literals in this level.
	 *
	 * @return the current list of literals
	 */
	const std::list<Literal> literals() const;

	/**
	 * Save a removed clause in the history.
	 * 
	* @param p_clause
	*            the clause that was removed from the formula
	 */
	void saveRemovedClause(const std::shared_ptr<Clause>& p_clause);

	/**
	 * Save a literal removed from a clause in the history.
	 *
	 * @param p_clause
	 *            the clause that was modified
	 * @param p_literal
	 *            the literal removed from that clause
	 */
	void saveRemovedLiteralFromClause(const std::shared_ptr<Clause>& p_clause, Literal p_literal);

	/**
	 * Replays the current history upon a given formula.
	 *
	 * @param p_formula
	 *            the formula in which to replay the history
	 */
	void replay(Formula& p_formula);

private:
	/** The ordered list of asserted literals. */
	std::list<Literal> m_literals;

	/** The history of operations done on the formula during this level. */
	History m_history;
};

} // namespace sat::solver
} // namespace sat

#endif // ResolutionStackLevel_h
