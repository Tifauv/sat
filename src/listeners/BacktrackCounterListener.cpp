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
#include "BacktrackCounterListener.h"


namespace sat {
namespace solver {
namespace listeners {


// CONSTRUCTORS
/**
 * Initializes the counter to zero.
 */
BacktrackCounterListener::BacktrackCounterListener() :
m_counter(0) {
}


// GETTERS
/**
 * Gives the current value of the counter.
 */
unsigned int BacktrackCounterListener::counter() {
	return m_counter;
}


// METHODS
/**
 * Resets the counter to zero.
 */
void BacktrackCounterListener::init() {
	m_counter = 0;
}


/**
 * Called when backtracking from the reduction of the formula by a literal.
 * Increments the counter.
 */
void BacktrackCounterListener::onBacktrack(Literal&) {
	m_counter++;
}

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat
