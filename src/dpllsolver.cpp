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
#include "dpllsolver.h"

#include <iostream>
#include <log4c.h>
#include "formula.h"
#include "interpretation.h"
#include "history.h"
#include "utils.h"
#include "log.h"


/**
 * Starter function of the solver.
 * Implements "solver.h"
 * 
 * @param p_formula
 *            the SAT formula to solve
 * 
 * @return an interpretation (satisfiable or not)
 */
Interpretation* DpllSolver::solve(Formula& p_formula) {
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Starting the DPLL algorithm.");
	Interpretation* interpretation = new Interpretation();
	unsigned int backtrackCounter = main(p_formula, *interpretation, 0);
	std::cout << "c Backtracked " << backtrackCounter << " times" << std::endl;
	return interpretation;
}


/**
 * Check whether a given solution is a valid interpretation of a formula.
 * 
 * @param p_formula
 *            the formula
 * @param p_solution
 *            the solution to check
 * 
 * @return true if the solution satisfies the formula,
 *         false otherwise
 */
bool DpllSolver::checkSolution(Formula& p_formula, std::list<RawLiteral>* p_solution) {
	for (auto literal = p_solution->cbegin(); literal != p_solution->cend(); ++literal) {
		if (!reduce(p_formula, *literal)) {
			std::cout << "An unsatisfiable clause was obtained." << std::endl;
			return false;
		}
	}
	
	if (!p_formula.hasMoreClauses()) {
		std::cout << "All clauses could be interpreted." << std::endl;
		return true;
	}
	std::cout << "Some clauses could not be interpreted." << std::endl;
	return false;
}


/**
 * Main loop of the Davis-Putnam algorithm.
 * 
 * @param p_formula
 *            the SAT formula to solve
 * @param p_interpretation
 *            the current interpretation
 * @param p_backtrackCounter
 *            the current number of backtracks
 * 
 * @return the new count of backtracks
 */
unsigned int DpllSolver::main(Formula& p_formula, Interpretation& p_interpretation, unsigned int p_backtrackCounter) {
	unsigned int backtrackCounter = p_backtrackCounter;

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Current state:");
	p_formula.log();
	p_interpretation.log();

	/*
	 * Stop case: if there is no clause left, we are done.
	 */
	if (!p_formula.hasMoreClauses()) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "No more clauses.");
		return backtrackCounter;
	}

	/*
	 * Choose the reduction literal.
	 * This is the crucial step, performance-wise.
	 */
	Literal chosen_literal = selectLiteral(p_formula);

	/*
	 * First reduction with the chosen literal.
	 */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "First reduction attempt...");
	History history;
	bool satisfiable = reduce(p_formula, chosen_literal, history);
	
	/*
	 * The reduced interpretation is satisfiable: we are done.
	 */
	if (satisfiable) {
		// Add the chosen literal to the current interpretation
		p_interpretation.push(chosen_literal);
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Added %sx%u to the interpretation.", (chosen_literal.isNegative() ? "¬" : ""), chosen_literal.id());

		// Loop again
		backtrackCounter = main(p_formula, p_interpretation, backtrackCounter);
		if (p_interpretation.isSatisfiable())
			return backtrackCounter;
		else // Remove the current literal from the interpretation
			p_interpretation.pop();
	}

	/*
	 * The interpretation is not satisfiable: we try with the opposite literal.
	 */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "The current interpretation is unsatisfiable.");
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Rebuilding the formula before second attempt...");
	history.replay(p_formula);
	p_formula.log();
	p_interpretation.setSatisfiable();
	p_interpretation.log();

	// Seconde réduction et test du résultat
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Second reduction attempt...");
	satisfiable = reduce(p_formula, -chosen_literal, history);

	/*
	 * The interpretation is satisfiable: we are done. We can return the current interpretation.
	 */
	if (satisfiable) {
		// Add the chosen literal to the current interpretation
		p_interpretation.push(-chosen_literal);
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Added %sx%u to the interpretation.", (chosen_literal.isPositive() ? "¬" : ""), chosen_literal.id());

		// Loop again
		backtrackCounter = main(p_formula, p_interpretation, backtrackCounter);
		if (p_interpretation.isSatisfiable())
			return backtrackCounter;
		else // Remove the current literal from the interpretation
			p_interpretation.pop();
	}
	/*
	 * The interpretation is also unsatisfiable with the opposite literal.
	 */
	else
		p_interpretation.setUnsatisfiable();

	/* Restoring state before backtracking. */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Restoring state before backtracking...");
	backtrackCounter++;

	// Reconstruction du graphe
	history.replay(p_formula);

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Restored state:");
	p_formula.log();
	p_interpretation.log();

	return backtrackCounter;
}


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @param p_formula
 *            the SAT formula
 * 
 * @return the literal
 */
Literal DpllSolver::selectLiteral(Formula& p_formula) {
	// Search a literal from a unitary clause
	Literal chosen_literal = p_formula.findLiteralFromUnaryClause();
	if (chosen_literal.var() != 0)
		return chosen_literal;

	// If there is no unitary clause
	return p_formula.selectLiteral();
}


/**
 * Reduces all the graph's formulas using a literal.
 * The history is used for backtracking.
 * 
 * @param p_formula
 *            the SAT graph of formulas
 * @param p_literal
 *            the literal used to reduce the formulas
 * @param p_history
 *            the backtracking history
 *
 * @return true if the reduction is satisfiable
 *         false if it is unsatisfiable
 */
bool DpllSolver::reduce(Formula& p_formula, Literal p_literal, History& p_history) {
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Reduction using literal %sx%u...", (p_literal.isNegative() ? "¬" : ""), p_literal.id());

	// Remove the clauses that contain the same sign as the given literal
	p_formula.removeClausesWithLiteral(p_literal, p_history);

	// Remove the literal from the clauses that contain the oposite sign
	bool satisfiable = p_formula.removeOppositeLiteralFromClauses(p_literal, p_history);

	// The variable is now empty, we can remove it
	p_formula.removeVariable(p_literal.var());

	return satisfiable;
}


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
bool DpllSolver::reduce(Formula& p_formula, const RawLiteral& p_literal) {
	History history;

	// Rebuild a Literal from a RawLiteral
	for (auto it = p_formula.beginVariable(); it != p_formula.endVariable(); ++it) {
		Variable* variable = *it;
		
		if (variable->id() == p_literal.id())
			return reduce(p_formula, Literal(variable, p_literal.sign()), history);
	}

	return true;
}
