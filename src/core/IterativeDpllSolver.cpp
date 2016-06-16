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

#include "log4c.h"
#include "log.h"
#include "Literal.h"
#include "Clause.h"
#include "Formula.h"
#include "SolverStack.h"
#include "LiteralSelector.h"

namespace sat {
namespace solver {


// ENUMS
enum class Result { UNDEFINED, SATISFIABLE, UNSATISFIABLE };


// CONSTRUCTORS
IterativeDpllSolver::IterativeDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector) :
m_formula(p_formula),
m_conflictClause(nullptr),
m_literalSelector(p_literalSelector) {

}


// DESTRUCTORS
IterativeDpllSolver::~IterativeDpllSolver() {

}


// METHODS
/**
 * Register a listener.
 *
 * @param p_listener
 *            the listener to register
 */
void IterativeDpllSolver::addListener(SolverListener& p_listener) {
	m_listeners.addListener(p_listener);
}


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
	m_listeners.init();

	// Solving
	dpll();

	// Cleaning the listeners
	m_listeners.cleanup();

	return m_valuation;
}


/**
 * Main loop of the DPLL algorithm.
 */
void IterativeDpllSolver::dpll() {
	m_stack.nextLevel();
	Result result = Result::UNDEFINED;

	while (result == Result::UNDEFINED) {
		m_valuation.log();

		// First, propagate all unitary clauses
		fullUnitPropagate();

		// Conflict case
		if (isConflicting()) {
			// Stop case
			if (atTopLevel())
				result = Result::UNSATISFIABLE;
			else {
				//applyConflict();
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
}


// Recursion control
bool IterativeDpllSolver::atTopLevel() const {
	return m_stack.currentLevel() == 0;
}


bool IterativeDpllSolver::allVariablesAssigned() const {
	return !m_formula.hasVariables();
}


// Unit propagation
/**
 * Runs a unit propagation until there is no more unit literal
 * or a conflict is found.
 */
void IterativeDpllSolver::fullUnitPropagate() {
	bool satisfiable;

	do {
		satisfiable = applyUnitPropagate();
	} while (satisfiable && !isConflicting());
}


/**
 * Searches a unit literal and propagates it.
 *
 * @return true if a unit literal is found and propagated,
 *         false otherwise
 */
bool IterativeDpllSolver::applyUnitPropagate() {
	// Search a unit literal
	Literal literal = m_formula.findUnitLiteral();

	// Exit if there is no unit literal
	if (literal.var() == nullptr)
		return false;

	// Notify listeners
	/* TODO enable later when the API is cleaned
	m_listeners.onPropagate(literal);
	m_listeners.onAssert(literal);
	*/

	propagateLiteral(literal);

	return true;
}


/**
 * Propagates a literal.
 * First, the clauses that contain the literal are removed from the formula.
 * Then, the opposite of the literal is removed from the clauses that contain it.
 *
 * @param p_literal
 *            the literal to propagate
 *
 * @see #removeClausesWithLiteral(Literal&)
 * @see #removeOppositeLiteralFromClauses(Literal&)
 */
void IterativeDpllSolver::propagateLiteral(Literal p_literal) {
	log4c_category_info(log_dpll, "Propagating literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());

	// Remove the clauses that contain the same sign as the given literal
	removeClausesWithLiteral(p_literal);

	// Remove the literal from the clauses that contain the oposite sign
	removeOppositeLiteralFromClauses(p_literal);

	// The variable is now empty, we can remove it
	m_formula.removeVariable(p_literal.var());

	// Notify the listeners
	m_listeners.onPropagate(p_literal);
}


/**
 * Removes the clauses containing the given literal.
 *
 * @param p_literal
 *            the literal to propagate
 */
void IterativeDpllSolver::removeClausesWithLiteral(Literal& p_literal) {
	log4c_category_info(log_dpll, "Removing clauses that contain the literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	for (Clause* clause = p_literal.occurence(); clause != nullptr; clause = p_literal.occurence()) {
		log4c_category_debug(log_dpll, "Saving clause %u in the history.", clause->id());
		m_stack.addClause(clause);

		m_formula.removeClause(clause);
		log4c_category_info(log_dpll, "Clause %u removed.", clause->id());
	}
}


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
void IterativeDpllSolver::removeOppositeLiteralFromClauses(Literal& p_literal) {
	log4c_category_info(log_dpll, "Removing literal %sx%u from the clauses.", (p_literal.isPositive() ? "¬" : ""), p_literal.id());
	for (Clause* clause = p_literal.oppositeOccurence(); clause != nullptr; clause = p_literal.oppositeOccurence()) {
		log4c_category_debug(log_dpll, "Saving literal %sx%u of clause %u in the history.", (p_literal.isPositive() ? "¬" : ""), p_literal.id(), clause->id());
		m_stack.addLiteral(clause, -p_literal);

		// Remove the literal from the clause
		m_formula.removeLiteralFromClause(clause, -p_literal);

		// Check if the clause is still satisfiable
		if (clause->isUnsatisfiable()) {
			log4c_category_info(log_dpll, "The produced clause is unsatisfiable.");
			setConflictClause(clause);
		}
	}
}


// Conflict
bool IterativeDpllSolver::isConflicting() const {
	return m_conflictClause != nullptr;
}


Clause* IterativeDpllSolver::getConflictClause() const {
	return m_conflictClause;
}


void IterativeDpllSolver::setConflictClause(Clause* p_clause) {
	m_conflictClause = p_clause;
}


void IterativeDpllSolver::resetConflictClause() {
	m_conflictClause = nullptr;
}


// Backtracking
void IterativeDpllSolver::applyBackjump() {
	resetConflictClause();

	// Current implementation only backtracks once at a time
	Literal currentLiteral = m_valuation.top();
	backtrack();

	// Try with the opposite literal
	propagateLiteral(-currentLiteral);
	m_valuation.push(-currentLiteral);
}


void IterativeDpllSolver::backtrack() {
	m_stack.replay(m_formula);
	m_stack.popLevel();
	m_valuation.pop();
}


// Decide
void IterativeDpllSolver::applyDecide() {
	m_stack.nextLevel();

	Literal selectedLiteral = m_literalSelector.getLiteral(m_formula);
	m_listeners.onDecide(selectedLiteral);

	propagateLiteral(selectedLiteral);
	m_valuation.push(selectedLiteral);
}


} // namespace sat::solver
} // namespace sat

