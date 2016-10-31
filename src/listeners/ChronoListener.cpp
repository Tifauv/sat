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
#include "ChronoListener.h"

#include "Literal.h"


namespace sat {
namespace solver {
namespace listeners {

// INTERFACE METHODS
/**
 * Records the start of processing time.
 */
void ChronoListener::init() {
	m_start = Clock::now();
}


/**
 * Records the end of processing time.
 */
void ChronoListener::cleanup() {
	m_end = Clock::now();
}

/**
 * Prints the current statistics to the given stream.
 */
ostream& operator<<(ostream& p_outStream, const ChronoListener& p_stats) {
	p_outStream << "Took " << chrono::duration_cast<chrono::milliseconds>(p_stats.m_end - p_stats.m_start).count() << " milliseconds";
	return p_outStream;
}

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat
