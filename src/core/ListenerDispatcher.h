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
#ifndef LISTENER_DISPATCHER_H
#define LISTENER_DISPATCHER_H

#include <vector>
#include <functional>
#include "SolverListener.h"

using namespace std;

namespace sat {
namespace solver {
namespace listeners {


/**
 * Implementation of a SolverListener that acts as a central point
 * to dispatch events to other Listeners.
 */
class ListenerDispatcher : public SolverListener {
public:
	/**
	 * Register a listener
	 * 
	 * @param p_listener
	 *            the listener to add
	 */
	void addListener(SolverListener& p_listener);


	/**
	 * Initialization called before a solving run.
	 */
	void init() override;


	/**
	 * Called when a literal has been selected.
	 */
	void onDecide(Literal&) override;


	/**
	 * Called when a unit-literal has been propagated.
	 */
	void onPropagate(Literal&) override;


	/**
	 * Called when a literal has been used to reduce the formula.
	 */
	void onAssert(Literal&) override;

	/**
	 * Called when a conflict has been reached.
	 */
	void onConflict(Clause&) override;


	/**
	 * Called when backtracking from the reduction of the formula by a literal.
	 */
	void onBacktrack(Literal&) override;


	/**
	 * Cleanup method called after a solving run.
	 */
	void cleanup() override;

private:
	/** The listeners. */
	vector<reference_wrapper<SolverListener>> m_listeners;

};

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat

#endif // LISTENER_DISPATCHER_H

