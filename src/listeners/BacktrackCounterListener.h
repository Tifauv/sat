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

#ifndef BACKTRACK_COUNTER_LISTENER_H
#define BACKTRACK_COUNTER_LISTENER_H

#include "NoopSolverListener.h"


namespace sat {
namespace solver {
namespace listeners {


/**
 * Implementation of a SolverListener that counts the number of backtracks.
 */
class BacktrackCounterListener : public NoopSolverListener {
public:
	/**
	 * Initializes the counter to zero.
	 */
	explicit BacktrackCounterListener();


	/**
	 * Gives the current value of the counter.
	 */ 
	unsigned int counter();


	/**
	 * Resets the counter to zero.
	 */
	void init() override;


	/**
	 * Called when backtracking from the reduction of the formula by a literal.
	 * Increments the counter.
	 */
	void onBacktrack(Literal&) override;


private:
	unsigned int m_counter;
};

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat

#endif // BACKTRACK_COUNTER_LISTENER_H
