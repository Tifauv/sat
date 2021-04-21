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
#include "ListenableSolver.h"


namespace sat {
namespace solver {

// METHODS
/**
 * Register a listener.
 *
 * @param p_listener
 *            the listener to add
 */
void ListenableSolver::addListener(SolverListener& p_listener) {
	m_listeners.addListener(p_listener);
}


/**
 * @brief ListenableSolver::dispatcher
 * @return
 */
ListenerDispatcher& ListenableSolver::listeners() {
	return m_listeners;
}


} // namespace sat::solver
} // namespace sat
