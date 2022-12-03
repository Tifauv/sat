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
#include "IterativeDpllSolver.h"

#include "log.h"
#include "Literal.h"
#include "Clause.h"
#include "Formula.h"
#include "LiteralSelector.h"

namespace sat {
namespace solver {


// ENUMS
enum class Result { UNDEFINED, SATISFIABLE, UNSATISFIABLE };


// CONSTRUCTORS
IterativeDpllSolver::IterativeDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector) :
m_formula(p_formula),
m_conflictClause(nullptr),
m_literalSelector(p_literalSelector) { }


// METHODS
/**
 * Gives the current valuation.
 *
 * @return the current valuation
 */
const Valuation& IterativeDpllSolver::getValuation() const {
	return m_valuation;
}


/**
 * Starter function of the solver.
 * Implements Solver.
 *
 * @return a valuation (satisfiable or not)
 */
Valuation& IterativeDpllSolver::solve() {
	// Initialize the listeners
	listeners().init();

	// Solving
	dpll();

	// Cleaning the listeners
	listeners().cleanup();

	return m_valuation;
}


/**
 * Main loop of the DPLL algorithm.
 */
void IterativeDpllSolver::dpll() {
	Result result = Result::UNDEFINED;

	while (result == Result::UNDEFINED) {
		m_resolution.logCurrentLiterals();

		// First, propagate all unitary clauses
		fullUnitPropagate();

		// Conflict case
		if (isConflicting()) {
			// Stop case
			if (atTopLevel())
				result = Result::UNSATISFIABLE;
			else {
				applyConflict();
				//applyExplain();
				//applySubsumption();
				applyBackjump();
			}
		}
		// General case
		else {
			// Restart
			/*
			if (m_restartStrategy.shouldRestart()) {
				applyRestart();
				applySimplify();
			}
			*/

			// Forget
			/*
			if (m_forgetStrategy.shouldForget())
				applyForget();
			*/

			// Stop case
			if (allVariablesAssigned())
				result = Result::SATISFIABLE;
			else
				applyDecide();
		}
	}

	if (result == Result::UNSATISFIABLE)
		m_valuation.setUnsatisfiable();
	else
		m_valuation = m_resolution.generateValuation();
}


// ITERATION CONTROL
bool IterativeDpllSolver::atTopLevel() const {
	return m_resolution.currentLevel() <= 1;
}


bool IterativeDpllSolver::allVariablesAssigned() const {
	return !m_formula.hasVariables();
}


// UNIT PROPAGATION
/**
 * Runs a unit propagation until there is no more unit literal
 * or a conflict is found.
 */
void IterativeDpllSolver::fullUnitPropagate() {
	bool satisfiable;

	do {
		satisfiable = applyUnitPropagate();
	} while (satisfiable);
}


/**
 * Searches a unit literal and propagates it.
 *
 * @return {@code true} iif a unit literal is found and propagated
 */
bool IterativeDpllSolver::applyUnitPropagate() {
	// Search a unit literal
	Literal literal = m_formula.findUnitLiteral();

	// Exit if there is no unit literal
	if (literal.var() == nullptr)
		return false;

	// Notify listeners
	listeners().onPropagate(literal);

	assertLiteral(literal);
	return !isConflicting();
}


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
void IterativeDpllSolver::assertLiteral(Literal p_literal) {
	reduceFormula(p_literal);

	// Notify the listeners
	listeners().onAssert(p_literal);
}


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
void IterativeDpllSolver::reduceFormula(Literal p_literal) {
	// Remove the clauses that contain the same sign as the given literal
	removeClausesWithLiteral(p_literal);

	// Remove the literal from the clauses that contain the oposite sign
	removeOppositeLiteralFromClauses(p_literal);

	// The variable is now empty, we can remove it
	m_formula.removeVariable(p_literal.var());

	// Add the literal to the current valuation
	m_resolution.pushLiteral(p_literal);
}


/**
 * Removes the clauses containing the given literal.
 *
 * @param p_literal
 *            the literal to propagate
 */
void IterativeDpllSolver::removeClausesWithLiteral(Literal& p_literal) {
	log_info(log_dpll, "Removing clauses that contain the literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	for (auto clause = p_literal.occurence(); clause != nullptr; clause = p_literal.occurence()) {
		log_debug(log_dpll, "Saving clause %u in the history.", clause->id());
		m_resolution.addClause(clause);

		m_formula.removeClause(clause);
		log_info(log_dpll, "Clause %u removed.", clause->id());
	}
}


/**
 * Removes the opposite of the given literal from the clauses.
 * If an empty clause is found, it is unsatisfiable and that clause is set as the conflict source.
 *
 * @param p_literal
 *            the selected literal
 *
 * @see setConflictClause() if an empty (unsatisfiable) clause is produced
 */
void IterativeDpllSolver::removeOppositeLiteralFromClauses(Literal& p_literal) {
	log_info(log_dpll, "Removing literal %sx%u from the clauses.", (p_literal.isPositive() ? "¬" : ""), p_literal.id());
	for (auto clause = p_literal.oppositeOccurence(); clause != nullptr; clause = p_literal.oppositeOccurence()) {
		log_debug(log_dpll, "Saving literal %sx%u of clause %u in the history.", (p_literal.isPositive() ? "¬" : ""), p_literal.id(), clause->id());
		m_resolution.addLiteral(clause, -p_literal);

		// Remove the literal from the clause
		m_formula.removeLiteralFromClause(clause, -p_literal);

		// Check if the clause is still satisfiable
		if (clause->isUnsatisfiable()) {
			log_info(log_dpll, "The produced clause is unsatisfiable.");
			setConflictClause(clause);
			break;
		}
	}
}


// CONFLICT
/**
 * Tells whether a conflict clause has been encountered or not.
 *
 * @return true if a conclict clause has been reached
 */
bool IterativeDpllSolver::isConflicting() const {
	return m_conflictClause != nullptr;
}


/**
 * Gives the conflict clause encountered.
 *
 * @return the conflict clause
 */
std::shared_ptr<Clause> IterativeDpllSolver::getConflictClause() const {
	return m_conflictClause;
}


/**
 * Updates the conflict clause.
 *
 * @param p_clause
 *            the new conflict clause
 */
void IterativeDpllSolver::setConflictClause(const std::shared_ptr<Clause>& p_clause) {
	m_conflictClause = p_clause;
}


/**
 * Resets the conflict clause to nullptr.
 */
void IterativeDpllSolver::resetConflictClause() {
	m_conflictClause = nullptr;
}


/**
 * Applies the conflict rule.
 * The listeners are notified of the onConflict() event,
 * then the conflict clause is reset.
 */
void IterativeDpllSolver::applyConflict() {
	// Notify the listeners
	listeners().onConflict(*getConflictClause());

	// Clean the conflict clause
	resetConflictClause();
}


// BACKTRACKING
/**
 * Rewinds the whole current resolution level and try with the opposite of the last decision literal.
 * This means the current history is replayed then the current resolution level is deleted.
 * The listeners are notified of the onBacktrack() event.
 * Finally, the opposite of the current decision literal is tried.
 */
void IterativeDpllSolver::applyBackjump() {
	// Rewind to the last decision literal
	Literal currentLiteral = m_resolution.lastDecisionLiteral();
	m_resolution.replay(m_formula);
	m_resolution.popLevel();

	// Notify the listeners
	listeners().onBacktrack(currentLiteral);

	// Try with the opposite literal
	reduceFormula(-currentLiteral);
}


// DECIDE
/**
 * Creates a new resolution level, selects a decision literal,
 * calls the listeners' onDecide() event then asserts the literal.
 */
void IterativeDpllSolver::applyDecide() {
	m_resolution.nextLevel();

	Literal selectedLiteral = m_literalSelector.getLiteral(m_formula);
	listeners().onDecide(selectedLiteral);

	assertLiteral(selectedLiteral);
}

} // namespace sat::solver
} // namespace sat

