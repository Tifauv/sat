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

#include "ListenableSolver.h"
#include "ResolutionStack.h"


namespace sat {
class Literal;
class Formula;

namespace solver {

class Valuation;
class LiteralSelector;


/**
 * @brief Iterative implementation of a DPLL solver.
 */
class IterativeDpllSolver : public ListenableSolver {
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

	// ITERATION CONTROL
	bool atTopLevel() const;
	bool allVariablesAssigned() const;

	// UNIT PROPAGATION
	/**
	 * Runs a unit propagation until there is no more unit literal
	 * or a conflict is found.
	 */
	void fullUnitPropagate();

	/**
	 * Searches a unit literal and propagates it.
	 *
	 * @return true if a unit literal is found and propagated,
	 *         false otherwise
	 */
	bool applyUnitPropagate();

	// LITERAL ASSERTION
	/**
	 * Like #reduceFormula(Literal) but also notifies the listeners of
	 * the onAssert() event.
	 *
	 * @param p_literal
	 *            the literal to propagate
	 *
	 * @see #reduceFormula(Literal)
	 */
	void assertLiteral(Literal p_literal);

	/**
	 * Reduces a formula with a given literal.
	 * First, the clauses that contain the literal are removed from the formula.
	 * Then, the opposite of the literal is removed from the clauses that contain it.
	 * Last, the literal is added to the resolution stack.
	 *
	 * @param p_literal
	 *            the literal to propagate
	 *
	 * @see #removeClausesWithLiteral(Literal&)
	 * @see #removeOppositeLiteralFromClauses(Literal&)
	 */
	void reduceFormula(Literal p_literal);

	/**
	 * Removes the clauses containing the given literal.
	 *
	 * @param p_literal
	 *            the literal to propagate
	 */
	void removeClausesWithLiteral(Literal& p_literal);

	/**
	 * Removes the opposite of the given literal from the clauses.
	 * If an empty clause is found, it is unsatisfiable and
	 *
	 * @param p_literal
	 *            the selected literal
	 *
	 * @return true if all the clauses could be reduced without producing an unsatisfiable one;
	 *         false if an unsatisfiable clause was produced.
	 */
	void removeOppositeLiteralFromClauses(Literal& p_literal);

	// CONFLICT
	/**
	 * Tells whether a conflict clause has been encountered or not.
	 *
	 * @return true if a conclict clause has been reached
	 */
	bool isConflicting() const;

	/**
	 * Gives the conflict clause encountered.
	 *
	 * @return the conflict clause
	 */
	Clause* getConflictClause() const;

	/**
	 * Updates the conflict clause.
	 *
	 * @param p_clause
	 *            the new conflict clause
	 */
	void setConflictClause(Clause*);

	/**
	 * Resets the conflict clause to nullptr.
	 */
	void resetConflictClause();

	/**
	 * Applies the conflict rule.
	 * The listeners are notified of the onConflict() event,
	 * then the conflict clause is reset.
	 */
	void applyConflict();

	// BACKTRACK
	/**
	 * Rewinds the whole current resolution level and try with the opposite of the last decision literal.
	 * This means the current history is replayed then the current resolution level is deleted.
	 * The listeners are notified of the onBacktrack() event.
	 * Finally, the opposite of the current decision literal is tried.
	 */
	void applyBackjump();

	// DECIDE
	/**
	 * Creates a new resolution level, selects a decision literal,
	 * calls the listeners' onDecide() event then asserts the literal.
	 */
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
};

} // namespace sat::solver
} // namespace sat

#endif // ITERATIVE_DPLL_SOLVER_H
