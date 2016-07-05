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

#ifndef ITERATIVE_DPLL_SOLVER_H
#define ITERATIVE_DPLL_SOLVER_H

#include "Solver.h"
#include "ResolutionStack.h"
#include "ListenerDispatcher.h"


namespace sat {
class Literal;
class Formula;

namespace solver {

using namespace listeners;

class Valuation;
class LiteralSelector;
class SolverListener;


/**
 * @brief Iterative implementation of a DPLL solver.
 */
class IterativeDpllSolver : public Solver {
public:
	/**
	 * Constructor.
	 *
	 * @param p_formula
	 *            the initial formula to solve
	 * @param p_literalSelector
	 *            the literal selection strategy
	 */
	explicit IterativeDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector);


	/**
	 * Destructor.
	 */
	~IterativeDpllSolver();


	/**
	 * Register a listener
	 *
	 * @param p_listener
	 *            the listener to add
	 */
	void addListener(SolverListener& p_listener);


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
	 * Main loop of the DPLL algorithm.
	 */
	void dpll();

	bool atTopLevel() const;
	bool allVariablesAssigned() const;

	void fullUnitPropagate();
	bool applyUnitPropagate();
	void propagateLiteral(Literal p_literal);
	void removeClausesWithLiteral(Literal& p_literal);
	void removeOppositeLiteralFromClauses(Literal& p_literal);

	bool isConflicting() const;
	Clause* getConflictClause() const;
	void setConflictClause(Clause*);
	void resetConflictClause();

	void applyBackjump();

	void applyDecide();


private:
	/** The formula beeing worked on. */
	Formula& m_formula;

	/** The valuation. */
	Valuation m_valuation;

	/** A conflicting clause. */
	Clause* m_conflictClause;

	/** The algorithm's resolution stack. */
	ResolutionStack m_resolution;

	/** The literal selection algorithm. */
	LiteralSelector& m_literalSelector;

	/** The listener dispatcher. */
	ListenerDispatcher m_listeners;
};

} // namespace sat::solver
} // namespace sat

#endif // ITERATIVE_DPLL_SOLVER_H
