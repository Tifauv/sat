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
#include "HistoryBasedDpllSolver.h"

#include <log4c.h>
#include "Formula.h"
#include "Interpretation.h"
#include "History.h"
#include "LiteralSelector.h"
#include "SolverListener.h"
#include "utils.h"
#include "log.h"


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
void HistoryBasedDpllSolver::registerListener(SolverListener& p_listener) {
	m_listeners.push_back(std::ref(p_listener));
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
	for (auto listener : m_listeners)
		listener.get().init();

	// Solving
	log4c_category_debug(log_dpll, "Starting the DPLL algorithm.");
	main();

	// Cleaning the listeners
	log4c_category_debug(log_dpll, "Cleaning listeners...");
	for (auto listener : m_listeners)
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
	bool satisfiable = reduce(chosen_literal, history);
	
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
	satisfiable = reduce(-chosen_literal, history);

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

	backtrack(history);
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
	for (auto listener : m_listeners)
		listener.get().onDecide(m_formula, chosen_literal);

	return chosen_literal;
}


/**
 * Reduces all the graph's formulas using a literal.
 * The history is used for backtracking.
 * 
 * @param p_literal
 *            the literal used to reduce the formulas
 * @param p_history
 *            the backtracking history
 *
 * @return true if the reduction is satisfiable
 *         false if it is unsatisfiable
 */
bool HistoryBasedDpllSolver::reduce(Literal p_literal, History& p_history) {
	log4c_category_info(log_dpll, "Reduction using literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());

	// Remove the clauses that contain the same sign as the given literal
	m_formula.removeClausesWithLiteral(p_literal, p_history);

	// Remove the literal from the clauses that contain the oposite sign
	bool satisfiable = m_formula.removeOppositeLiteralFromClauses(p_literal, p_history);

	// The variable is now empty, we can remove it
	m_formula.removeVariable(p_literal.var());

	// Notify the listeners
	log4c_category_debug(log_dpll, "Notifying listeners...");
	for (auto listener : m_listeners)
		listener.get().onReduce(m_formula, p_literal);

	return satisfiable;
}


/**
 * Restores the previous state before backtracking.
 * 
 * @param p_history
 *            the history to replay
 */
void HistoryBasedDpllSolver::backtrack(History& p_history) {
	log4c_category_debug(log_dpll, "Restoring state before backtracking...");

	// Reconstruction du graphe
	p_history.replay(m_formula);

	// Notify the listeners
	log4c_category_debug(log_dpll, "Notifying listeners...");
	for (auto listener : m_listeners)
		listener.get().onBacktrack(m_formula);

	log4c_category_debug(log_dpll, "Restored state:");
	m_formula.log();
	m_interpretation.log();
}
