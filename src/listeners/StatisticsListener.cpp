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
#include "StatisticsListener.h"

#include "Literal.h"


namespace sat {
namespace solver {
namespace listeners {

// CONSTRUCTORS
StatisticsListener::StatisticsListener() :
m_decisions(0),
m_propagations(0),
m_assertions(0),
m_conflicts(0),
m_backtracks(0) { }


// INTERFACE METHODS
/**
 * Resets the counters to zero.
 */
void StatisticsListener::init() {
	m_decisions = 0;
	m_propagations = 0;
	m_assertions = 0;
	m_conflicts = 0;
	m_backtracks = 0;
}


/**
 * Increments the literal decision counter.
 */
void StatisticsListener::onDecide(Literal&) {
	m_decisions++;
}


/**
 * Increments the unit propagation counter.
 */
void StatisticsListener::onPropagate(Literal&) {
	m_propagations++;
}


/**
 * Increments the literal assertion counter.
 */
void StatisticsListener::onAssert(Literal&) {
	m_assertions++;
}


/**
 * Increments the conflicts counter.
 */
void StatisticsListener::onConflict(Clause&) {
	m_conflicts++;
}


/**
 * Increments the backtracks counter.
 */
void StatisticsListener::onBacktrack(Literal&) {
	m_backtracks++;
}


/**
 * Prints the current statistics to the given stream.
 */
std::ostream& operator<<(std::ostream& p_outStream, const StatisticsListener& p_stats) {
	p_outStream << "Statistics [ " <<
			p_stats.m_decisions    << " decisions  |  "    <<
			p_stats.m_propagations << " propagations  |  " <<
			p_stats.m_assertions   << " assertions  |  "   <<
			p_stats.m_conflicts    << " conflicts  |  "    <<
			p_stats.m_backtracks   << " backtracks ]";
	return p_outStream;
}

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat
