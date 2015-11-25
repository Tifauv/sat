/* Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki
   Copyright (C) 2015 Olivier Serve

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
*/

#ifndef DPLL_SOLVER_H
#define DPLL_SOLVER_H

#include "solver.h"
#include "formula.h"
#include "interpretation.h"
#include "history.h"


/**
 * Implementation of a SAT solver using the DPLL algorithm.
 */
class DpllSolver : public Solver {
public:
	/**
	 * Common entry point for all solvers.
	 *
	 * @param p_formula
	 *            the formula to resolve
	 *
	 * @return the interpretation found
	 */
	Interpretation* solve(Formula& p_formula);

	
	/**
	 * Check whether a given solution is a valid interpretation of a formula.
	 * 
	 * @param p_formula
	 *            the formula
	 * @param p_solution
	 *            the solution to check
	 * 
	 * @return true if the solution satisfies teh formula,
	 *         false otherwise
	 */
	bool checkSolution(Formula& p_formula, std::list<RawLiteral>* p_solution);

protected:
	/**
	 * Main loop of the Davis-Putnam algorithm.
	 * 
	 * @param p_formula
	 *            the SAT formula to solve
	 * @param p_intr
	 *            the current interpretation
	 */
	void main(Formula& p_formula, Interpretation& p_interpretation);

	
	/**
	 * Selects a literal to be used for the reduction phase.
	 * 
	 * @param p_formula
	 *            the SAT formula
	 * 
	 * @return the literal
	 */
	virtual Literal selectLiteral(Formula& p_formula);


	/**
	 * Reduces all the graph's formulas using a literal.
	 * The history is used for backtracking.
	 * 
	 * @param p_formula
	 *            the SAT formula
	 * @param p_literal
	 *            the literal used to reduce the formula
	 * @param p_history
	 *            the backtracking history
	 * 
	 * @return true if the reduction is satisfiable
	 *         false if it is unsatisfiable
	 */
	bool reduce(Formula& p_formula, Literal p_literal, History& p_history);


	/**
	 * Overloaded implementation that takes a raw literal instead of a literal.
	 * 
	 * @param p_formula
	 *            the SAT formula
	 * @param p_literal
	 *            the raw literal used to reduce the formula
	 * 
	 * @return true if the reduction is satisfiable
	 *         false if it is unsatisfiable
	 */
	bool reduce(Formula& p_formula, const RawLiteral& p_literal);
};

#endif // DPLL_SOLVER_H

