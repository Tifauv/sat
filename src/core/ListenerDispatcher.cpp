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
#include "ListenerDispatcher.h"


namespace sat {
namespace solver {
namespace listeners {


// CONSTRUCTORS
/**
 * Initializes the counter to zero.
 */
ListenerDispatcher::ListenerDispatcher() {
}


// METHODS
/**
 * Register a listener
 * 
 * @param p_listener
 *            the listener to add
 */
void ListenerDispatcher::addListener(SolverListener& p_listener) {
	m_listeners.push_back(std::ref(p_listener));
}


/**
 * Initializes all the listeners.
 */
void ListenerDispatcher::init() {
	for (const auto& listener : m_listeners)
		listener.get().init();
}


/**
 * Called when a literal has been selected.
 */
void ListenerDispatcher::onDecide(Literal& p_literal) {
	for (const auto& listener : m_listeners)
		listener.get().onDecide(p_literal);
}


/**
 * Called when a literal has been propagated through the formula.
 */
void ListenerDispatcher::onPropagate(Literal& p_literal) {
	for (const auto& listener : m_listeners)
		listener.get().onPropagate(p_literal);
}


/**
 * Called when a conflict clause has been produced.
 */
void ListenerDispatcher::onConflict(Clause* p_clause) {
	for (const auto& listener : m_listeners)
		listener.get().onConflict(p_clause);
}


/**
 * Called when backtracking from the reduction of the formula by a literal.
 */
void ListenerDispatcher::onBacktrack(Literal& p_literal) {
	for (const auto& listener : m_listeners)
		listener.get().onBacktrack(p_literal);
}


/**
 * Cleanup method called after a solving run.
 */
void ListenerDispatcher::cleanup() {
	for (const auto& listener : m_listeners)
		listener.get().cleanup();
}

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat
