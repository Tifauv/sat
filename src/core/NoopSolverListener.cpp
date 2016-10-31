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
#include "NoopSolverListener.h"

#include "Literal.h"
#include "Clause.h"


namespace sat {
namespace solver {
namespace listeners {


// INTERFACE METHODS
/**
 * Initialization called before a solving run. 
 * Does nothing.
 */
void NoopSolverListener::init() {
	// Do nothing
}


/**
 * Called when a literal has been selected.
 * Does nothing.
 */
void NoopSolverListener::onDecide(Literal&) {
	// Do nothing
}


/**
 * Called when a unit-literal has been propagated.
 * Does nothing.
 */
void NoopSolverListener::onPropagate(Literal&) {
	// Do nothing
}


/**
 * Called when a literal has been used to reduce the formula.
 * Does nothing.
 */
void NoopSolverListener::onAssert(Literal&) {
	// Do nothing
}


/**
 * Called when a conflict has been reached.
 * Does nothing.
 */
void NoopSolverListener::onConflict(shared_ptr<Clause>) {
	// Do nothing
}


/**
 * Called when backtracking from the reduction of the formula by a literal.
 * Does nothing.
 */
void NoopSolverListener::onBacktrack(Literal&) {
	// Do nothing
}


/**
 * Cleanup method called after a solving run.
 * Does nothing.
 */
void NoopSolverListener::cleanup() {
	// Do nothing
}

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat
