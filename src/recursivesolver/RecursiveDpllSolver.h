/*  Copyright 2015 Olivier Serve
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
#ifndef RECURSIVE_DPLL_SOLVER_H
#define RECURSIVE_DPLL_SOLVER_H

#include "ListenableSolver.h"


namespace sat {
class Literal;
class Formula;

namespace history {
	class History;
}
using namespace history;

namespace solver {


class Valuation;
class LiteralSelector;


/**
 * Recursive implementation of a DPLL solver.
 */
class RecursiveDpllSolver : public ListenableSolver {
public:
	/**
	 * Constructor.
	 * 
	 * @param p_formula
	 *            the initial formula to solve
	 * @param p_literalSelector
	 *            the literal selection strategy
	 */
	explicit RecursiveDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector);


	/**
	 * Gives the current valuation.
	 * 
	 * @return the current valuation
	 */
	const Valuation& getValuation() const override;
	

	/**
	 * Common entry point for all solvers.
	 *
	 * @return the Valuation found
	 */
	Valuation& solve() override;


protected:
	/**
	 * Main loop of the Davis-Putnam algorithm.
	 */
	void dpll();


	/**
	 * Selects a literal to be used for the reduction phase.
	 * 
	 * @return the literal
	 */
	Literal decide();


	/**
	 * Propagates the clause reduction by the literal to the formula.
	 * The history is used for backtracking.
	 * 
	 * @param p_literal
	 *            the literal propagated through the formula
	 * @param p_history
	 *            the backtracking history
	 *
	 * 
	 * @return true if the reduction is satisfiable
	 *         false if it is unsatisfiable
	 */
	bool propagate(Literal p_literal, History& p_history);


	/**
	 * Restores the previous state before backtracking.
	 * 
	 * @param p_literal
	 *            the literal whose propagation is backtracked
	 * @param p_history
	 *            the history to replay
	 */
	void backtrack(Literal p_literal, History& p_history);


	/**
	 * Removes the clauses containing the given literal.
	 * 
	 * @param p_literal
	 *            the selected literal
	 * @param p_history
	 *            the history to save the operations
	 */
	void removeClausesWithLiteral(Literal& p_literal, History& p_history);

	
	/**
	 * Removes the opposite of the given literal from the clauses.
	 * If an empty clause is found, it is unsatisfiable and 
	 * 
	 * @param p_literal
	 *            the selected literal
	 * @param p_history
	 *            the history to save the operations
	 * 
	 * @return true if all the clauses could be reduced without producing an unsatisfiable one;
	 *         false if an unsatisfiable clause was produced.
	 */
	bool removeOppositeLiteralFromClauses(Literal& p_literal, History& p_history);


private:
	/** The formula beeing worked on. */
	Formula& m_formula;

	/** The valuation . */
	Valuation m_valuation;

	/** The literal selection algorithm. */
	LiteralSelector& m_literalSelector;
};

} // namespace sat::solver
} // namespace sat

#endif // RECURSIVE_DPLL_SOLVER_H

