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
#ifndef StatisticsListener_h
#define StatisticsListener_h

#include "NoopSolverListener.h"
#include <ostream>


namespace sat {
namespace solver {
namespace listeners {

/**
 * @brief Statistics gathering listener.
 * It maintains a counter for each of the listenable operation.
 * 
 */
class StatisticsListener : public NoopSolverListener {
public:
	/**
	 * Resets the counters to zero.
	 */
	void init() override;

	/**
	 * Increments the decision counter.
	 */
	void onDecide(Literal& p_literal) override;

	/**
	 * Increments the propagation counter.
	 */
	void onPropagate(Literal& p_literal) override;

	/**
	 * Increments the backtracks counter.
	 */
	void onBacktrack(Literal& p_literal) override;

	/**
	 * Prints the current statistics to the given stream.
	 */
	friend std::ostream& operator<<(std::ostream& p_outStream, const StatisticsListener& p_stats);

private:
	/** The number of decisions made. */
	unsigned int m_decisions;

	/** The number of literal propagations done. */
	unsigned int m_propagations;

	/** The number of backtracks. */
	unsigned int m_backtracks;
};

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat

#endif // LoggingListener_h
