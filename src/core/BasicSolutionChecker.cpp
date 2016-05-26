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
#include "BasicSolutionChecker.h"

#include <iostream>
#include <log4c.h>
#include "Variable.h"
#include "Literal.h"
#include "Clause.h"
#include "Formula.h"
#include "utils.h"
#include "log.h"


namespace sat {
namespace checker {


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
 * Check whether a given solution is a valid valuation of the formula.
 * 
 * @param p_solution
 *            the solution to check
 * 
 * @return true if the solution satisfies the formula,
 *         false otherwise
 */
bool BasicSolutionChecker::checkSolution(std::vector<RawLiteral>* p_solution) {
	for (const auto& literal : *p_solution) {
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
	// Rebuild a Literal from a RawLiteral
	for (auto it = m_formula.beginVariable(); it != m_formula.endVariable(); ++it) {
		Variable* variable = *it;
		
		if (variable->id() == p_rawLiteral.id()) {
			Literal literal(variable, p_rawLiteral.sign());
			log4c_category_info(log_dpll, "Reduction using literal %sx%u...", (literal.isNegative() ? "¬" : ""), literal.id());
		
			// Remove the clauses that contain the same sign as the given literal
			for (Clause* clause = literal.occurence(); clause != nullptr; clause = literal.occurence())
				m_formula.removeClause(clause);
		
			// Remove the literal from the clauses that contain the oposite sign
			bool satisfiable = true;
			for (Clause* clause = literal.oppositeOccurence(); clause != nullptr; clause = literal.oppositeOccurence()) {
				// Remove the literal from the clause
				m_formula.removeLiteralFromClause(clause, -literal);
				
				// Check if the clause is still satisfiable
				if (clause->isUnsatisfiable()) {
					log4c_category_info(log_dpll, "The produced clause is unsatisfiable.");
					satisfiable = false;
					break;
				}
			}
		
			// The variable is now empty, we can remove it
			m_formula.removeVariable(variable);
		
			return satisfiable;
		}
	}
	
	return true;
}

} // namespace sat::checker
} // namespace sat
