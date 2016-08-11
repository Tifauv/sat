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
#ifndef REMOVE_CLAUSE_STEP_H
#define REMOVE_CLAUSE_STEP_H

#include "HistoryStep.h"


namespace sat {

class Formula;

namespace solver {
namespace history {


/**
 * A step representing a "clause removed" operation.
 */
class RemoveClauseStep : public HistoryStep {
public:
	/**
	 * Initialize the step.
	 * 
	 * @param p_clause
	 *            the recorded clause
	 */
	explicit RemoveClauseStep(Clause* p_clause);


	/**
	 * Replay the operation of the step.
	 * Must be implemented in subclasses.
	 * 
	 * @param p_formula
	 *            the formula in which to replay the operation
	 */
	void undo(Formula& p_formula) const override;
}; // class RemoveClauseStep

} // namespace sat::solver::history
} // namespace sat::solver
} // namespace sat

#endif // RemoveClauseStep_h
