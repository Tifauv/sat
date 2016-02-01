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
#include "DpllSolver.h"

#include <iostream>
#include <log4c.h>
#include "Formula.h"
#include "Interpretation.h"
#include "History.h"
#include "LiteralSelector.h"
#include "utils.h"
#include "log.h"


// CONSTRUCTORS
/**
 * Constructor.
 * 
 * @param p_literalSelector
 *            the literal selection strategy
 */
DpllSolver::DpllSolver(LiteralSelector& p_literalSelector, Formula& p_formula) : 
m_literalSelector(p_literalSelector),
m_formula(p_formula) {
	log4c_category_debug(log_dpll, "DPLL Solver created.");
}


// DESTRUCTORS
/**
 * Destructor.
 */
DpllSolver::~DpllSolver() {
	log4c_category_debug(log_dpll, "DPLL Solver deleted.");	
}


// INTERFACE METHODS
/**
 * Starter function of the solver.
 * Implements Solver.
 * 
 * @return an interpretation (satisfiable or not)
 */
Interpretation& DpllSolver::solve() {
	log4c_category_debug(log_dpll, "Starting the DPLL algorithm.");
	unsigned int backtrackCounter = main(0);
	std::cout << "c Backtracked " << backtrackCounter << " times" << std::endl;
	return m_interpretation;
}


/**
 * Check whether a given solution is a valid interpretation of the formula.
 * 
 * @param p_solution
 *            the solution to check
 * 
 * @return true if the solution satisfies the formula,
 *         false otherwise
 */
bool DpllSolver::checkSolution(std::list<RawLiteral>* p_solution) {
	for (auto literal = p_solution->cbegin(); literal != p_solution->cend(); ++literal) {
		if (!reduce(*literal)) {
			std::cout << "An unsatisfiable clause was obtained." << std::endl;
			return false;
		}
	}
	
	if (!m_formula.hasClauses()) {
		std::cout << "All clauses could be interpreted." << std::endl;
		return true;
	}
	std::cout << "Some clauses could not be interpreted." << std::endl;
	return false;
}


// METHODS
/**
 * Main loop of the Davis-Putnam algorithm.
 * 
 * @param p_backtrackCounter
 *            the current number of backtracks
 * 
 * @return the new count of backtracks
 */
unsigned int DpllSolver::main(unsigned int p_backtrackCounter) {
	unsigned int backtrackCounter = p_backtrackCounter;

	log4c_category_info(log_dpll, "Current state:");
	m_formula.log();
	m_interpretation.log();

	/*
	 * Stop case: if there is no clause left, we are done.
	 */
	if (!m_formula.hasClauses()) {
		log4c_category_info(log_dpll, "No more clauses.");
		return backtrackCounter;
	}

	
	/*
	 * Stop case: if there is no variable left, we are done.
	 */
	if (!m_formula.hasVariables()) {
		log4c_category_info(log_dpll, "No more variables.");
		return backtrackCounter;
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
		backtrackCounter = main(backtrackCounter);
		if (m_interpretation.isSatisfiable())
			return backtrackCounter;
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
		backtrackCounter = main(backtrackCounter);
		if (m_interpretation.isSatisfiable())
			return backtrackCounter;
		else // Remove the current literal from the interpretation
			m_interpretation.pop();
	}
	/*
	 * The interpretation is also unsatisfiable with the opposite literal.
	 */
	else
		m_interpretation.setUnsatisfiable();

	backtrack(history);
	return backtrackCounter + 1;
}


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @return the literal
 */
Literal DpllSolver::decide() {
	// Search a literal from a unitary clause
	Literal chosen_literal = m_formula.findLiteralFromUnaryClause();
	if (chosen_literal.var() != nullptr)
		return chosen_literal;

	// If there is no unitary clause, use the selector
	return m_literalSelector.getLiteral(m_formula);
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
bool DpllSolver::reduce(Literal p_literal, History& p_history) {
	log4c_category_info(log_dpll, "Reduction using literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());

	// Remove the clauses that contain the same sign as the given literal
	m_formula.removeClausesWithLiteral(p_literal, p_history);

	// Remove the literal from the clauses that contain the oposite sign
	bool satisfiable = m_formula.removeOppositeLiteralFromClauses(p_literal, p_history);

	// The variable is now empty, we can remove it
	m_formula.removeVariable(p_literal.var());

	return satisfiable;
}


/**
 * Overloaded implementation that takes a raw literal instead of a literal.
 * 
 * @param p_literal
 *            the raw literal used to reduce the formula
 * 
 * @return true if the reduction is satisfiable
 *         false if it is unsatisfiable
 */
bool DpllSolver::reduce(const RawLiteral& p_literal) {
	History history;

	// Rebuild a Literal from a RawLiteral
	for (auto it = m_formula.beginVariable(); it != m_formula.endVariable(); ++it) {
		Variable* variable = *it;
		
		if (variable->id() == p_literal.id())
			return reduce(Literal(variable, p_literal.sign()), history);
	}

	return true;
}


/**
 * Restores the previous state before backtracking.
 * 
 * @param p_history
 *            the history to replay
 */
void DpllSolver::backtrack(History& p_history) {
	/* Restoring state before backtracking. */
	log4c_category_debug(log_dpll, "Restoring state before backtracking...");
	
	// Reconstruction du graphe
	p_history.replay(m_formula);
	
	log4c_category_debug(log_dpll, "Restored state:");
	m_formula.log();
	m_interpretation.log();
}
