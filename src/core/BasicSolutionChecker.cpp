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
#include "BasicSolutionChecker.h"

#include <iostream>
#include <log4c.h>
#include "Formula.h"
#include "History.h"
#include "utils.h"
#include "log.h"


// CONSTRUCTORS
/**
 * Constructor.
 * 
 * @param p_formula
 *            the formula
 */
BasicSolutionChecker::BasicSolutionChecker(Formula& p_formula) : 
m_formula(p_formula) {
	log4c_category_debug(log_dpll, "Basic SolutionChecker created.");
}


// DESTRUCTORS
/**
 * Destructor.
 */
BasicSolutionChecker::~BasicSolutionChecker() {
	log4c_category_debug(log_dpll, "Basic SolutionChecker deleted.");
}


// INTERFACE METHODS
/**
 * Check whether a given solution is a valid interpretation of the formula.
 * 
 * @param p_solution
 *            the solution to check
 * 
 * @return true if the solution satisfies the formula,
 *         false otherwise
 */
bool BasicSolutionChecker::checkSolution(std::vector<RawLiteral>* p_solution) {
	for (auto literal : *p_solution) {
		if (!reduce(literal)) {
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
 * Reduces the formula using a raw literal.
 * 
 * @param p_rawLiteral
 *            the raw literal used to reduce the formula
 * 
 * @return true if the reduction is satisfiable
 *         false if it is unsatisfiable
 */
bool BasicSolutionChecker::reduce(const RawLiteral& p_rawLiteral) {
	History history;
	
	// Rebuild a Literal from a RawLiteral
	for (auto it = m_formula.beginVariable(); it != m_formula.endVariable(); ++it) {
		Variable* variable = *it;
		
		if (variable->id() == p_rawLiteral.id()) {
			Literal literal(variable, p_rawLiteral.sign());
			log4c_category_info(log_dpll, "Reduction using literal %sx%u...", (literal.isNegative() ? "¬" : ""), literal.id());
		
			// Remove the clauses that contain the same sign as the given literal
			m_formula.removeClausesWithLiteral(literal, history);
		
			// Remove the literal from the clauses that contain the oposite sign
			bool satisfiable = m_formula.removeOppositeLiteralFromClauses(literal, history);
		
			// The variable is now empty, we can remove it
			m_formula.removeVariable(variable);
		
			return satisfiable;
		}
	}
	
	return true;
}
