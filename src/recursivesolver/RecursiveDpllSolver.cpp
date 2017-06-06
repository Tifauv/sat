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
#include "RecursiveDpllSolver.h"

#include <algorithm>
#include "Clause.h"
#include "Formula.h"
#include "Valuation.h"
#include "History.h"
#include "LiteralSelector.h"
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
RecursiveDpllSolver::RecursiveDpllSolver(Formula& p_formula, LiteralSelector& p_literalSelector) : 
m_formula(p_formula),
m_literalSelector(p_literalSelector) {
}


// METHODS
/**
 * Gives the current valuation.
 * 
 * @return the current valuation
 */
const Valuation& RecursiveDpllSolver::getValuation() const {
	return m_valuation;
}


/**
 * Starter function of the solver.
 * Implements Solver.
 * 
 * @return a valuation (satisfiable or not)
 */
Valuation& RecursiveDpllSolver::solve() {
	listeners().init();

	// Solving
	log_debug(log_dpll, "Starting the DPLL algorithm.");
	dpll();

	listeners().cleanup();

	return m_valuation;
}


/**
 * Main loop of the Davis-Putnam algorithm.
 */
void RecursiveDpllSolver::dpll() {
	log_info(log_dpll, "Current state:");
	m_formula.log();
	m_valuation.log();

	/*
	 * Stop case: if there is no clause left, we are done.
	 */
	if (!m_formula.hasClauses()) {
		log_info(log_dpll, "No more clauses.");
		return;
	}

	/*
	 * Stop case: if there is no variable left, we are done.
	 */
	if (!m_formula.hasVariables()) {
		log_info(log_dpll, "No more variables.");
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
	log_info(log_dpll, "First reduction attempt...");
	History history;
	bool satisfiable = propagate(chosen_literal, history);

	/*
	 * The reduced valuation is satisfiable: we are done.
	 */
	if (satisfiable) {
		// Add the chosen literal to the current valuation
		m_valuation.push(chosen_literal);
		log_info(log_dpll, "Added %sx%u to the valuation.", (chosen_literal.isNegative() ? "¬" : ""), chosen_literal.id());

		// Loop again
		dpll();
		if (m_valuation.isSatisfiable())
			return;
		else // Remove the current literal from the valuation
			m_valuation.pop();
	}

	/*
	 * The valuation is not satisfiable: we try with the opposite literal.
	 */
	log_info(log_dpll, "The current valuation is unsatisfiable.");
	log_debug(log_dpll, "Rebuilding the formula before second attempt...");
	history.replay(m_formula);
	m_formula.log();
	m_valuation.setSatisfiable();
	m_valuation.log();

	// Seconde réduction et test du résultat
	log_info(log_dpll, "Second reduction attempt...");
	satisfiable = propagate(-chosen_literal, history);

	/*
	 * The valuation is satisfiable: we are done. We can return the current valuation.
	 */
	if (satisfiable) {
		// Add the chosen literal to the current valuation
		m_valuation.push(-chosen_literal);
		log_info(log_dpll, "Added %sx%u to the valuation.", (chosen_literal.isPositive() ? "¬" : ""), chosen_literal.id());

		// Loop again
		dpll();
		if (m_valuation.isSatisfiable())
			return;
		else // Remove the current literal from the valuation
			m_valuation.pop();
	}
	/*
	 * The valuation is also unsatisfiable with the opposite literal.
	 */
	else
		m_valuation.setUnsatisfiable();

	backtrack(chosen_literal, history);
}


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @return the literal
 */
Literal RecursiveDpllSolver::decide() {
	// Search a unit literal
	Literal chosen_literal = m_formula.findUnitLiteral();

	// If there is no unit literal, use the selector
	if (chosen_literal.var() == nullptr) {
		chosen_literal = m_literalSelector.getLiteral(m_formula);

		// Notify the listeners
		listeners().onDecide(chosen_literal);
	}

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
bool RecursiveDpllSolver::propagate(Literal p_literal, History& p_history) {
	log_info(log_dpll, "Propagating literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());

	// Remove the clauses that contain the same sign as the given literal
	removeClausesWithLiteral(p_literal, p_history);

	// Remove the literal from the clauses that contain the oposite sign
	bool satisfiable = removeOppositeLiteralFromClauses(p_literal, p_history);

	// The variable is now empty, we can remove it
	m_formula.removeVariable(p_literal.var());

	// Notify the listeners
	listeners().onPropagate(p_literal);

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
void RecursiveDpllSolver::backtrack(Literal p_literal, History& p_history) {
	log_debug(log_dpll, "Restoring state before backtracking...");

	// Reconstruction du graphe
	p_history.replay(m_formula);

	// Notify the listeners
	listeners().onBacktrack(p_literal);

	log_debug(log_dpll, "Restored state:");
	m_formula.log();
	m_valuation.log();
}


/**
 * Removes the clauses containing the given literal.
 * 
 * @param p_literal
 *            the selected literal
 * @param p_history
 *            the history to save the operations
 */
void RecursiveDpllSolver::removeClausesWithLiteral(Literal& p_literal, History& p_history) {
	log_info(log_dpll, "Removing clauses that contain the literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	for (auto clause = p_literal.occurence(); clause != nullptr; clause = p_literal.occurence()) {
		log_debug(log_dpll, "Saving clause %u in the history.", clause->id());
		p_history.addClause(clause);
		m_formula.removeClause(clause);
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
bool RecursiveDpllSolver::removeOppositeLiteralFromClauses(Literal& p_literal, History& p_history) {
	log_info(log_dpll, "Removing literal %sx%u from the clauses.", (p_literal.isPositive() ? "¬" : ""), p_literal.id());
	for (auto clause = p_literal.oppositeOccurence(); clause != nullptr; clause = p_literal.oppositeOccurence()) {
		log_debug(log_dpll, "Saving literal %sx%u of clause %u in the history.", (p_literal.isPositive() ? "¬" : ""), p_literal.id(), clause->id());
		p_history.addLiteral(clause, -p_literal);
		
		// Remove the literal from the clause
		m_formula.removeLiteralFromClause(clause, -p_literal);

		// Check if the clause is still satisfiable
		if (clause->isUnsatisfiable()) {
			log_info(log_dpll, "The produced clause is unsatisfiable.");
			return false;
		}
	}
	
	return true;
}

} // namespace sat::solver
} // namespace sat
