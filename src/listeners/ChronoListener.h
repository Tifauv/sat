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
#ifndef CHRONO_LISTENER_H
#define CHRONO_LISTENER_H

#include "NoopSolverListener.h"
#include <ostream>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;


namespace sat {
namespace solver {
namespace listeners {

/**
 * @brief Statistics gathering listener.
 * It maintains a counter for each of the listenable operation.
 * 
 */
class ChronoListener : public NoopSolverListener {
public:
	/**
	 * Starts the chrono.
	 */
	void init() override;

	/**
	 * Stops the chrono.
	 */
	void cleanup() override;

	/**
	 * Prints the current elapsed time to the given stream.
	 */
	friend std::ostream& operator<<(std::ostream& p_outStream, const ChronoListener& p_stats);

private:
	/** The number of backtracks. */
	Clock::time_point m_start;
	Clock::time_point m_end;
};

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat

#endif // LoggingListener_h
