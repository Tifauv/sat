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
#ifndef STATISTICS_LISTENER_H
#define STATISTICS_LISTENER_H

#include "NoopSolverListener.h"
#include <ostream>

using namespace std;


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
	 * Increments the literal decision counter.
	 */
	void onDecide(Literal&) override;

	/**
	 * Increments the unit propagation counter.
	 */
	void onPropagate(Literal&) override;

	/**
	 * Increments the literal assertion counter.
	 */
	void onAssert(Literal&) override;

	/**
	 * Increments the conflicts counter.
	 */
	void onConflict(shared_ptr<Clause>) override;

	/**
	 * Increments the backtracks counter.
	 */
	void onBacktrack(Literal&) override;

	/**
	 * Prints the current statistics to the given stream.
	 */
	friend ostream& operator<<(ostream& p_outStream, const StatisticsListener& p_stats);

private:
	/** The number of literal decisions made. */
	unsigned int m_decisions;

	/** The number of unit propagations done. */
	unsigned int m_propagations;

	/** The number of literal asserted. */
	unsigned int m_assertions;

	/** The number of conflicts. */
	unsigned int m_conflicts;

	/** The number of backtracks. */
	unsigned int m_backtracks;
};

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat

#endif // LoggingListener_h
