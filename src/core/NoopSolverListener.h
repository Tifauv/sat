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
	virtual void onDecide(Formula&, Literal&) override;


	/**
	 * Called when the formula has been reduced by a literal.
	 * Does nothing.
	 */
	virtual void onReduce(Formula&, Literal&) override;


	/**
	 * Called when backtracking from the reduction of the formula by a literal.
	 * Does nothing.
	 */
	virtual void onBacktrack(Formula&) override;


	/**
	 * Cleanup method called after a solving run.
	 * Does nothing.
	 */
	virtual void cleanup() override;
};

#endif // NOOP_DPLL_SOLVER_H

