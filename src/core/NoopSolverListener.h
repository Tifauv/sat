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
#ifndef NOOP_SOLVER_LISTENER_H
#define NOOP_SOLVER_LISTENER_H

#include "SolverListener.h"


namespace sat {
namespace solver {
namespace listeners {


/**
 * Implementation of a SolverListener that does nothing.
 * This is meant to be the base class for all implementations.
 */
class NoopSolverListener : public SolverListener {
public:
	/**
	 * Initialization called before a solving run. 
	 * Does nothing.
	 */
	virtual void init() override;


	/**
	 * Called when a literal has been selected.
	 * Does nothing.
	 */
	virtual void onDecide(Literal&) override;


	/**
	 * Called when a unit-literal has been propagated.
	 * Does nothing.
	 */
	virtual void onPropagate(Literal&) override;


	/**
	 * Called when a literal has been used to reduce the formula.
	 * Does nothing.
	 */
	virtual void onAssert(Literal&) override;


	/**
	 * Called when a conflict has been reached.
	 * Does nothing.
	 */
	virtual void onConflict(Clause&) override;


	/**
	 * Called when backtracking from the reduction of the formula by a literal.
	 * Does nothing.
	 */
	virtual void onBacktrack(Literal&) override;


	/**
	 * Cleanup method called after a solving run.
	 * Does nothing.
	 */
	virtual void cleanup() override;
};

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat

#endif // NOOP_DPLL_SOLVER_H

