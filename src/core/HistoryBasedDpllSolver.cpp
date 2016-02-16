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
#include "HistoryBasedDpllSolver.h"

#include <log4c.h>
#include <algorithm>
#include "Clause.h"
#include "Formula.h"
#include "Interpretation.h"
#include "History.h"
#include "LiteralSelector.h"
#include "SolverListener.h"
#include "utils.h"
#include "log.h"


namespace sat {
namespace solver {


// CONSTRUCTORS
/**
 * Constructor.
 * 
 * @param p_formula
 *            the initial formula to solve
 * @param p_literalSelector
 *            the literal selection strategy
 */
HistoryBasedDpllSolver::HistoryBasedDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector) : 
m_formula(p_formula),
m_literalSelector(p_literalSelector) {
	log4c_category_debug(log_dpll, "DPLL Solver created.");
}


// DESTRUCTORS
/**
 * Destructor.
 */
HistoryBasedDpllSolver::~HistoryBasedDpllSolver() {
	log4c_category_debug(log_dpll, "DPLL Solver deleted.");
}


// METHODS
/**
 * Register a listener
 * 
 * @param p_listener
 *            the listener to add
 */
void HistoryBasedDpllSolver::addListener(SolverListener& p_listener) {
	m_listeners.push_back(std::ref(p_listener));
}


/**
 * Gives the current interpretation.
 * 
 * @return the current interpretation
 */
const Interpretation& HistoryBasedDpllSolver::getInterpretation() const {
	return m_interpretation;
}


/**
 * Starter function of the solver.
 * Implements Solver.
 * 
 * @return an interpretation (satisfiable or not)
 */
Interpretation& HistoryBasedDpllSolver::solve() {
	// Initialize the listeners
	log4c_category_debug(log_dpll, "Initializing listeners...");
	for (const auto& listener : m_listeners)
		listener.get().init();

	// Solving
	log4c_category_debug(log_dpll, "Starting the DPLL algorithm.");
	main();

	// Cleaning the listeners
	log4c_category_debug(log_dpll, "Cleaning listeners...");
	for (const auto& listener : m_listeners)
		listener.get().cleanup();

	return m_interpretation;
}


/**
 * Main loop of the Davis-Putnam algorithm.
 */
void HistoryBasedDpllSolver::main() {
	log4c_category_info(log_dpll, "Current state:");
	m_formula.log();
	m_interpretation.log();

	/*
	 * Stop case: if there is no clause left, we are done.
	 */
	if (!m_formula.hasClauses()) {
		log4c_category_info(log_dpll, "No more clauses.");
		return;
	}

	/*
	 * Stop case: if there is no variable left, we are done.
	 */
	if (!m_formula.hasVariables()) {
		log4c_category_info(log_dpll, "No more variables.");
		return;
	}

	/*
	 * Choose the reduction literal.
	 * This is the crucial step, performance-wise.
	 */
	Literal chosen_literal = decide();

	/*
	 * First reduction with the chosen literal.
	 */
	log4c_category_info(log_dpll, "First reduction attempt...");
	History history;
	bool satisfiable = propagate(chosen_literal, history);

	/*
	 * The reduced interpretation is satisfiable: we are done.
	 */
	if (satisfiable) {
		// Add the chosen literal to the current interpretation
		m_interpretation.push(chosen_literal);
		log4c_category_info(log_dpll, "Added %sx%u to the interpretation.", (chosen_literal.isNegative() ? "¬" : ""), chosen_literal.id());

		// Loop again
		main();
		if (m_interpretation.isSatisfiable())
			return;
		else // Remove the current literal from the interpretation
			m_interpretation.pop();
	}

	/*
	 * The interpretation is not satisfiable: we try with the opposite literal.
	 */
	log4c_category_info(log_dpll, "The current interpretation is unsatisfiable.");
	log4c_category_debug(log_dpll, "Rebuilding the formula before second attempt...");
	history.replay(m_formula);
	m_formula.log();
	m_interpretation.setSatisfiable();
	m_interpretation.log();

	// Seconde réduction et test du résultat
	log4c_category_debug(log_dpll, "Second reduction attempt...");
	satisfiable = propagate(-chosen_literal, history);

	/*
	 * The interpretation is satisfiable: we are done. We can return the current interpretation.
	 */
	if (satisfiable) {
		// Add the chosen literal to the current interpretation
		m_interpretation.push(-chosen_literal);
		log4c_category_info(log_dpll, "Added %sx%u to the interpretation.", (chosen_literal.isPositive() ? "¬" : ""), chosen_literal.id());

		// Loop again
		main();
		if (m_interpretation.isSatisfiable())
			return;
		else // Remove the current literal from the interpretation
			m_interpretation.pop();
	}
	/*
	 * The interpretation is also unsatisfiable with the opposite literal.
	 */
	else
		m_interpretation.setUnsatisfiable();

	backtrack(chosen_literal, history);
}


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @return the literal
 */
Literal HistoryBasedDpllSolver::decide() {
	// Search a literal from a unitary clause
	Literal chosen_literal = m_formula.findLiteralFromUnaryClause();

	// If there is no unitary clause, use the selector
	if (chosen_literal.var() == nullptr)
		chosen_literal = m_literalSelector.getLiteral(m_formula);

	// Notify the listeners
	log4c_category_debug(log_dpll, "Notifying listeners...");
	for (const auto& listener : m_listeners)
		listener.get().onDecide(chosen_literal);

	return chosen_literal;
}


/**
 * Reduces all the graph's formulas using a literal.
 * The history is used for backtracking.
 * 
 * @param p_literal
 *            the literal propagated through the formula
 * @param p_history
 *            the backtracking history
 *
 * @return true if the reduction is satisfiable
 *         false if it is unsatisfiable
 */
bool HistoryBasedDpllSolver::propagate(Literal p_literal, History& p_history) {
	log4c_category_info(log_dpll, "Propagating literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());

	// Remove the clauses that contain the same sign as the given literal
	removeClausesWithLiteral(p_literal, p_history);

	// Remove the literal from the clauses that contain the oposite sign
	bool satisfiable = removeOppositeLiteralFromClauses(p_literal, p_history);

	// The variable is now empty, we can remove it
	m_formula.removeVariable(p_literal.var());

	return satisfiable;
}


/**
 * Restores the previous state before backtracking.
 * 
 * @param p_literal
 *            the literal whose propagation is backtracked
 * @param p_history
 *            the history to replay
 */
void HistoryBasedDpllSolver::backtrack(Literal p_literal, History& p_history) {
	log4c_category_debug(log_dpll, "Restoring state before backtracking...");

	// Reconstruction du graphe
	p_history.replay(m_formula);

	// Notify the listeners
	log4c_category_debug(log_dpll, "Notifying listeners...");
	for (const auto& listener : m_listeners)
		listener.get().onBacktrack(p_literal);

	log4c_category_debug(log_dpll, "Restored state:");
	m_formula.log();
	m_interpretation.log();
}


/**
 * Removes the clauses containing the given literal.
 * 
 * @param p_literal
 *            the selected literal
 * @param p_history
 *            the history to save the operations
 */
void HistoryBasedDpllSolver::removeClausesWithLiteral(Literal& p_literal, History& p_history) {
	log4c_category_info(log_dpll, "Removing clauses that contain the literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	for (Clause* clause = p_literal.occurence(); clause != nullptr; clause = p_literal.occurence()) {
		log4c_category_debug(log_dpll, "Saving clause %u in the history.", clause->id());
		p_history.addClause(clause);
		
		m_formula.removeClause(clause);
		log4c_category_info(log_dpll, "Clause %u removed.", clause->id());
		
		// Notify the listeners
		log4c_category_debug(log_dpll, "Notifying listeners...");
		for (const auto& listener : m_listeners)
			listener.get().onPropagate(p_literal, clause);
	}
}


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
bool HistoryBasedDpllSolver::removeOppositeLiteralFromClauses(Literal& p_literal, History& p_history) {
	log4c_category_info(log_dpll, "Removing literal %sx%u from the clauses.", (p_literal.isPositive() ? "¬" : ""), p_literal.id());
	for (Clause* clause = p_literal.oppositeOccurence(); clause != nullptr; clause = p_literal.oppositeOccurence()) {
		log4c_category_debug(log_dpll, "Saving literal %sx%u of clause %u in the history.", (p_literal.isPositive() ? "¬" : ""), p_literal.id(), clause->id());
		p_history.addLiteral(clause, -p_literal);
		
		// Remove the literal from the clause
		m_formula.removeLiteralFromClause(clause, -p_literal);

		// Notify the listeners
		log4c_category_debug(log_dpll, "Notifying listeners...");
		for (const auto& listener : m_listeners)
			listener.get().onPropagate(p_literal, clause);

		// Check if the clause is still satisfiable
		if (clause->isUnsatisfiable()) {
			log4c_category_info(log_dpll, "The produced clause is unsatisfiable.");
			return false;
		}
	}
	
	return true;
}

} // namespace sat::solver
} // namespace sat
