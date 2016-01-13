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

#ifndef DPLL_SOLVER_H
#define DPLL_SOLVER_H

#include <list>
#include "Solver.h"

class Interpretation;
class RawLiteral;
class Literal;
class Formula;
class History;
class LiteralSelector;


/**
 * Implementation of a SAT solver using the DPLL algorithm.
 */
class DpllSolver : public Solver {
public:
	/**
	 * Constructor.
	 * 
	 * @param p_literalSelector
	 *            the literal selection strategy
	 * @param p_formula
	 *            the initial formula to solve
	 */
	explicit DpllSolver(LiteralSelector& p_literalSelector, Formula& p_formula);


	/**
	 * Destructor.
	 */
	~DpllSolver();


	/**
	 * Common entry point for all solvers.
	 *
	 * @return the interpretation found
	 */
	Interpretation& solve();

	
	/**
	 * Check whether a given solution is a valid interpretation of a formula.
	 * 
	 * @param p_solution
	 *            the solution to check
	 * 
	 * @return true if the solution satisfies the formula,
	 *         false otherwise
	 */
	bool checkSolution(std::list<RawLiteral>* p_solution);

protected:
	/**
	 * Main loop of the Davis-Putnam algorithm.
	 * 
	 * @param p_backtrackCounter
	 *            the current number of backtracks
	 * 
	 * @return the new count of backtracks
	 */
	unsigned int main(unsigned int p_backtrackCounter);

	
	/**
	 * Selects a literal to be used for the reduction phase.
	 * 
	 * @return the literal
	 */
	virtual Literal selectLiteral();


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
	 * Overloaded implementation that takes a RawLiteral instead of a Literal.
	 * 
	 * @param p_literal
	 *            the raw literal used to reduce the formula
	 * 
	 * @return true if the reduction is satisfiable
	 *         false if it is unsatisfiable
	 */
	bool reduce(const RawLiteral& p_literal);


	/**
	 * Restores the previous state before backtracking.
	 * 
	 * @param p_history
	 *            the history to replay
	 */
	void backtrack(History& p_history);

private:
	LiteralSelector& m_literalSelector;
	Formula& m_formula;
	Interpretation m_interpretation;
};

#endif // DPLL_SOLVER_H

