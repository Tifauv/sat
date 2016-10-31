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
#ifndef HISTORY_STEP_H
#define HISTORY_STEP_H

#include <memory>
#include "Literal.h"

using namespace std;

namespace sat {

class Formula;
class Clause;

namespace history {


/**
 * @brief This is a step of the history.
 * 
 * This is a virtual class. Subclasses must implement the {@link #undo(Formula&) const} method.
 */
class HistoryStep {
public:
	/**
	 * Initialize a step with a clause.
	 * 
	 * @param p_clause
	 *            the clause to record
	 */
	explicit HistoryStep(shared_ptr<Clause> p_clause);


	/**
	 * Initialize a step with a clause.
	 * 
	 * @param p_clause
	 *            the clause to record
	 * @param p_literal
	 *            the literal to record
	 */
	explicit HistoryStep(shared_ptr<Clause> p_clause, Literal p_literal);


	/**
	 * Give the clause recorded in this step.
	 * 
	 * @return a pointer to the clause
	 */
	shared_ptr<Clause> clause() const;


	/**
	 * Give the literal recorder in this step.
	 * 
	 * @return the literal (can be empty)
	 */
	Literal literal() const;


	/**
	 * Replay the operation of the step.
	 * Must be implemented in subclasses.
	 * 
	 * @param p_formula
	 *            the formula in which to replay the operation
	 */
	virtual void undo(Formula& p_formula) const = 0;


private:
	shared_ptr<Clause> m_clause;
	Literal            m_literal;
};

} // namespace sat::history
} // namespace sat

#endif // HistoryStep_h
