/* Copyright (C) 2015 Olivier Serve

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
*/
#include "formula.h"

#include <algorithm>
#include <functional>
#include <log4c.h>
#include "utils.h"
#include "log.h"

// CONSTRUCTORS
/**
 * Creates an empty formula.
 */
Formula::Formula() {
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Formula created.");
}


// DESTRUCTORS
/**
 * Cleans the memory used by the formula. 
 */
Formula::~Formula() {
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Formula deleted.");
}


// METHODS
/**
 * Creates a clause in the formula.
 *
 * @param p_clauseId
 *            the identifier of the clause
 * @param p_literals
 *            the literals
 *
 * @return the new clause
 */
Clause* Formula::createClause(Id p_clauseId, std::list<CnfLiteral>& p_literals) {
	// Create the clause & add it to the list
	Clause* clause = new Clause(p_clauseId);
	m_clauses.insert(clause);
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u added.", p_clauseId);
	
	// Link the new clause with its literals
	for (auto iterator = p_literals.cbegin(); iterator != p_literals.cend(); ++iterator) {
		CnfLiteral literal = *iterator;
		
		// Find the variable for the literal
		Variable* variable = findOrCreateVariable(literal.id());

		// Link clause -> variable
		clause->addLiteral(new Literal(variable, literal.sign()));

		// Link variable -> clause
		variable->addOccurence(clause, literal.sign());
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u added to clause %u.", (literal.sign() > 0 ? "¬" : ""), literal.id(), p_clauseId);
	}

	return clause;
}


/**
 * Searches a variable with the given id in the list of used variables.
 * If no one is found, a new one is created and added.
 *
 * @param p_variableId
 *            the identifier of the variable to find
 *
 * @return the variable, either found or created
 */
Variable* Formula::findOrCreateVariable(VariableId p_variableId) {
	Variable* variable = NULL;

	// Search the variable
	auto match_id([p_variableId](Variable* variable) { return variable->id() == p_variableId; } );
	auto iterator = std::find_if(m_variables.cbegin(), m_variables.cend(), std::cref(match_id));

	// If the variable was found, select it
	if (iterator != m_variables.cend()) {
		variable = *iterator;
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Variable x%u found.", variable->id());
	}
	// Otherwise, create & add it
	else {
		variable = new Variable(p_variableId);
		m_variables.insert(variable);
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Variable x%u added.", variable->id());
	}
	
	return variable;
}


/**
 * 
 */
Literal* Formula::findLiteralFromUnaryClause() const {
	// Search an unary clause
	auto isUnary([](Clause* clause) { return clause->isUnary(); } );
	auto iterator = std::find_if(m_clauses.cbegin(), m_clauses.cend(), std::cref(isUnary));

	// If a clause has been found, retrieve its literal
	if (iterator != m_clauses.cend()) {
		Clause* clause = *iterator;
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Unary clause %u found.", clause->id());
		return clause->firstLiteral();
	}

	// No unary clause found
	return NULL;
}


/**
 * 
 */
Literal* Formula::selectLiteral() const {
	// Parameters check
	if (m_variables.empty()) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "There is no more used literal in the formula.");
		return NULL;
	}

	Variable* variable = *m_variables.cbegin();
	int sign = SIGN_POSITIVE;
	if (!variable->hasPositiveOccurence())
		sign = SIGN_NEGATIVE;

	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u selected.", (sign == SIGN_NEGATIVE ? "¬" : ""), variable->id());
	return new Literal(variable, sign);
}


void Formula::removeClause(Clause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Cannot remove a NULL clause.");
		return;
	}

	// Remove all links variable -> clause
	for (auto it = p_clause->beginLiteral(); it != p_clause->endLiteral(); ++it) {
		unlinkVariable(p_clause, *it);
	}
	
	// Move the clause to the unused list
	m_clauses.erase(p_clause);
	m_unusedClauses.insert(p_clause);
	p_clause->setUnused();
}


void Formula::removeLiteralFromClause(Clause* p_clause, Literal& p_literal) {
}


void Formula::unlinkVariable(Clause* p_clause, Literal* p_literal) {
	Variable* variable = p_literal->var();
	if (p_literal->sign() == SIGN_POSITIVE)
		variable->removePositiveOccurence(p_clause);
	else
		variable->removeNegativeOccurence(p_clause);

	if (!variable->hasPositiveOccurence() && !variable->hasNegativeOccurence())
		removeVariable(variable);
}


/**
 * Move the variable to the unused list.
 * 
 * @param p_variable
 *            the variable to remove
 */
void Formula::removeVariable(Variable* p_variable) {
	m_variables.erase(p_variable);
	m_unusedVariables.insert(p_variable);
	p_variable->setUnused();
}

void Formula::addClause(Clause* p_clause, std::list<Literal>& p_literals) {
}


void Formula::addLiteralToClause(Clause* p_clause, Literal& p_literal) {
}
