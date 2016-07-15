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
#ifndef LISTENABLE_SOLVER_H
#define LISTENABLE_SOLVER_H

#include "Solver.h"
#include "ListenerDispatcher.h"


namespace sat {
namespace solver {

using namespace listeners;

class SolverListener;


/**
 * Abstract class that adds support for to listeners to the Solver interface.
 */
class ListenableSolver : public Solver {
public:
	/**
	 * Register a listener
	 *
	 * @param p_listener
	 *            the listener to add
	 */
	void addListener(SolverListener& p_listener);

protected:
	ListenerDispatcher& listeners() ;

private:
	/** The listener dispatcher. */
	ListenerDispatcher m_listeners;
};

} // namespace sat::solver
} // namespace sat

#endif // LISTENABLE_SOLVER_H
