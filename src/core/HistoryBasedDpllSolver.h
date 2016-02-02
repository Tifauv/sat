/*  Copyright (c) 2015 Olivier Serve
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

#ifndef HISTORY_BASED_DPLL_SOLVER_H
#define HISTORY_BASED_DPLL_SOLVER_H

#include <vector>
#include <functional>
#include "Solver.h"

class Interpretation;
class Literal;
class Formula;
class History;
class LiteralSelector;
class SolverListener;


/**
 * Implementation of a DPLL solver.
 */
class HistoryBasedDpllSolver : public Solver {
public:
	/**
	 * Constructor.
	 * 
	 * @param p_formula
	 *            the initial formula to solve
	 * @param p_literalSelector
	 *            the literal selection strategy
	 */
	explicit HistoryBasedDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector);


	/**
	 * Destructor.
	 */
	~HistoryBasedDpllSolver();


	/**
	 * Register a listener
	 * 
	 * @param p_listener
	 *            the listener to add
	 */
	void registerListener(SolverListener& p_listener);
	

	/**
	 * Common entry point for all solvers.
	 *
	 * @return the interpretation found
	 */
	Interpretation& solve() override;


protected:
	/**
	 * Main loop of the Davis-Putnam algorithm.
	 */
	void main();


	/**
	 * Selects a literal to be used for the reduction phase.
	 * 
	 * @return the literal
	 */
	Literal decide();


	/**
	 * Reduces all the graph's formulas using a literal.
	 * The history is used for backtracking.
	 * 
	 * @param p_literal
	 *            the literal used to reduce the formula
	 * @param p_history
	 *            the backtracking history
	 * 
	 * @return true if the reduction is satisfiable
	 *         false if it is unsatisfiable
	 */
	bool reduce(Literal p_literal, History& p_history);


	/**
	 * Restores the previous state before backtracking.
	 * 
	 * @param p_history
	 *            the history to replay
	 */
	void backtrack(History& p_history);

private:
	/** The formula beeing worked on. */
	Formula& m_formula;

	/** The interpretation . */
	Interpretation m_interpretation;

	/** The literal selection algorithm. */
	LiteralSelector& m_literalSelector;

	/** The listeners. */
	std::vector<std::reference_wrapper<SolverListener>> m_listeners;
};

#endif // HISTORY_BASED_DPLL_SOLVER_H

