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
#ifndef HISTORY_H
#define HISTORY_H

#include <memory>
#include <stack>
#include "Literal.h"
#include "HistoryStep.h"

using namespace std;

namespace sat {

class Formula;
class Clause;

namespace history {


/**
 * An history is a list of steps.
 */
class History {
public:
	/**
	 * Adds an operation of type OP_ADD_CLAUSE as last step of the history.
	 *
	 * @param p_clause
	 *            the clause to save
	 */
	void addClause(shared_ptr<Clause> p_clause);

	/**
	 * Adds an operation of type OP_ADD_LITERAL_TO_CLAUSE as last step of the history.
	 *
	 * @param p_clause
	 *            the clause
	 * @param p_literal
	 *            the literal to save
	 */
	void addLiteral(shared_ptr<Clause> p_clause, Literal p_literal);

	/**
	 * Replays the modifications stored in the history.
	 *
	 * @param p_formula
	 *            the formula in which to replay the operations
	 */
	void replay(Formula& p_formula);

private:
	/** The steps of the history. */
	stack<unique_ptr<HistoryStep>> m_steps;
};

} // namespace sat::history
} // namespace sat

#endif // History_h

