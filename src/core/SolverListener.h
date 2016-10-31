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
#ifndef SOLVER_LISTENER_H
#define SOLVER_LISTENER_H

#include <memory>

using namespace std;

namespace sat {

class Literal;
class Clause;

namespace solver {


/**
 * Interface for Solver Listeners.
 */
class SolverListener {
public:
	/**
	 * Initialization called before a solving run. 
	 */
	virtual void init() = 0;
	
	
	/**
	 * Called when a literal has been selected.
	 * 
	 * @param p_literal
	 *            the selected literal
	 */
	virtual void onDecide(Literal& p_literal) = 0;
	
	
	/**
	 * Called when a unit-literal has been propagated.
	 * 
	 * @param p_literal
	 *            the literal propagated through the formula
	 */
	virtual void onPropagate(Literal& p_literal) = 0;


	/**
	 * Called when a literal has been used to reduce the formula.
	 *
	 * @param p_literal
	 *            the literal that has been used
	 */
	virtual void onAssert(Literal& p_literal) = 0;
	

	/**
	 * Called when a conflict has been reached.
	 * The clause that generated the conflict is given as parameter.
	 *
	 * @param p_conflictClause
	 *            the clause that generated the conflict
	 */
	virtual void onConflict(shared_ptr<Clause> p_conflictClause) = 0;
	

	/**
	 * Called when backtracking from the reduction of the formula by a literal.
	 * 
	 * @param p_literal
	 *            the literal
	 */
	virtual void onBacktrack(Literal& p_literal) = 0;
	
	
	/**
	 * Cleanup method called after a solving run.
	 */
	virtual void cleanup() = 0;
};

} // namespace sat::solver
} // namespace sat

#endif // DPLL_SOLVER_H

