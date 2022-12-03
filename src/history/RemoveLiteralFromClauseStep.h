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
#ifndef REMOVE_LITERAL_FROM_CLAUSE_STEP_H
#define REMOVE_LITERAL_FROM_CLAUSE_STEP_H

#include "HistoryStep.h"


namespace sat {

class Formula;
class Clause;

namespace history {


/**
 * A step representing a "literal removed from clause" operation.
 */
class RemoveLiteralFromClauseStep : public HistoryStep {
public:
	/**
	 * Initialize the step.
	 * 
	 * @param p_clause
	 *            the recorded clause
	 * @param p_literal
	 *            the recorded literal
	 */
	RemoveLiteralFromClauseStep(const std::shared_ptr<Clause>& p_clause, Literal p_literal);
	
	
	/**
	 * Replay the operation of the step.
	 * Must be implemented in subclasses.
	 * 
	 * @param p_formula
	 *            the formula in which to replay the operation
	 */
	void undo(Formula& p_formula) const override;
}; // class RemoveLiteralFromClauseStep

} // namespace sat::history
} // namespace sat

#endif // RemoveLiteralFromClauseStep_h
