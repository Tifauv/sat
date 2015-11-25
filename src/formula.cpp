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
 * The needed variables are created if needed.
 *
 * @param p_clauseId
 *            the identifier of the clause
 * @param p_literals
 *            the raw literals
 *
 * @return the new clause
 */
Clause* Formula::createClause(Id p_clauseId, std::list<RawLiteral>& p_literals) {
	// Create the clause & add it to the list
	Clause* clause = new Clause(p_clauseId);
	m_clauses.insert(clause);
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u added.", p_clauseId);
	
	// Link the new clause with its literals
	for (auto iterator = p_literals.cbegin(); iterator != p_literals.cend(); ++iterator) {
		RawLiteral literal = *iterator;
		
		// Find the variable for the literal
		Variable* variable = findOrCreateVariable(literal.id());

		// Link clause -> variable
		clause->addLiteral(Literal(variable, literal.sign()));

		// Link variable -> clause
		variable->addOccurence(clause, literal.sign());
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
 * Retrieves the literal from a unary clause.
 *
 * @return the literal found,
 *         or NULL if there is no unary clause
 */
Literal Formula::findLiteralFromUnaryClause() const {
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
	return Literal(NULL, SIGN_POSITIVE);
}


/**
 * Select a literal for the reduction phase.
 *
 * @return the literal selected,
 *         or NULL if there is not anymore
 */
Literal Formula::selectLiteral() const {
	// Parameters check
	if (m_variables.empty()) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "There is no more used literal in the formula.");
		return Literal(NULL, SIGN_POSITIVE);
	}

	Variable* variable = *m_variables.cbegin();
	int sign = SIGN_POSITIVE;
	if (!variable->hasPositiveOccurence())
		sign = SIGN_NEGATIVE;

	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u selected.", (sign == SIGN_NEGATIVE ? "¬" : ""), variable->id());
	return Literal(variable, sign);
}


/**
 * Remove a clause from the list of active ones.
 *
 * @param p_clause
 *            the clause to remove
 */
void Formula::removeClause(Clause* p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Cannot remove a NULL clause.");
		return;
	}

	// Remove all links variable -> clause
	for (auto it = p_clause->beginLiteral(); it != p_clause->endLiteral(); ++it)
		if (p_literal != *it)
			unlinkVariable(p_clause, *it);

	// Move the clause to the unused list
	m_clauses.erase(p_clause);
	m_unusedClauses.insert(p_clause);
	p_clause->setUnused();
	log4c_category_log(log_formula(), LOG4C_PRIORITY_INFO, "Clause %u removed.", p_clause->id());
}


/**
 * Remove a literal from a clause.
 *
 * @param p_clause
 *            the clause
 * @param p_literal
 *            the literal
 */
void Formula::removeLiteralFromClause(Clause* p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Cannot remove a literal from a NULL clause.");
		return;
	}

	// Remove the link clause -> variable
	p_clause->removeLiteral(p_literal);
	
	// Remove the link variable -> clause
	//unlinkVariable(p_clause, p_literal);
	log4c_category_log(log_formula(), LOG4C_PRIORITY_INFO, "Literal %sx%u removed from clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), p_clause->id());
}


/**
 * Removes a link from a variable to a clause.
 *
 * @param p_clause
 *            the targetted clause
 * @param p_literal
 *            the literal to remove
 */
void Formula::unlinkVariable(Clause* p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Cannot remove a literal from a NULL clause.");
		return;
	}

	// Remove the clause from the variable occurences
	Variable* variable = p_literal.var();
	if (p_literal.isPositive())
		variable->removePositiveOccurence(p_clause);
	else
		variable->removeNegativeOccurence(p_clause);

	// Auto-remove unused variables
	if (!variable->hasPositiveOccurence() && !variable->hasNegativeOccurence()) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_INFO, "Variable x%u is not used anymore.", variable->id());
		removeVariable(variable);
	}
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
	log4c_category_log(log_formula(), LOG4C_PRIORITY_INFO, "Variable x%u has been removed.", p_variable->id());
}


/**
 * Move a clause from the unused to current list.
 * 
 * @param p_clause
 *            the clause
 */
void Formula::addClause(Clause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Cannot add a NULL clause.");
		return;
	}

	// Move the clause to the current list
	m_unusedClauses.erase(p_clause);
	m_clauses.insert(p_clause);
	p_clause->setUsed();
	log4c_category_log(log_formula(), LOG4C_PRIORITY_INFO, "Clause %u added.", p_clause->id());
}


/**
 * Add a literal to a clause.
 * 
 * @param p_clause
 *            the clause
 * @param p_literal
 *            the literal to add
 */
void Formula::addLiteralToClause(Clause* p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Cannot add a literal to a NULL clause.");
		return;
	}

	// Link clause -> variable
	p_clause->addLiteral(p_literal);

	// Link variable -> clause
	p_literal.var()->addOccurence(p_clause, p_literal.sign());
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u added to clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), p_clause->id());
}


/**
 * Tells whether there are still clauses to process.
 */
bool Formula::hasMoreClauses() const {
	return !m_clauses.empty();
}


/**
 * 
 */
std::unordered_set<Variable*>::iterator Formula::beginVariable() {
	return m_variables.begin();
}


/**
 * 
 */
std::unordered_set<Variable*>::iterator Formula::endVariable() {
	return m_variables.end();
}


/**
 * Logs the current formula to the formula logger.
 */
void Formula::log() const {
	// Print the clauses
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clauses = {");
	for (auto clauseIt = m_clauses.cbegin(); clauseIt != m_clauses.cend(); ++clauseIt) {
		Clause* clause = *clauseIt;

		// Clause id
		std::string line = "   " + std::to_string(clause->id()) + ": ";

		// Clause's literals
		int deb = 1;
		for (auto literalIt = clause->beginLiteral(); literalIt != clause->endLiteral(); ++literalIt) {
			const Literal literal = *literalIt;

			line.append(deb != 1 ? " v " : "").append(literal.isNegative() ? "¬x" : "x").append(std::to_string(literal.id()));
			if (deb == 1)
				deb = 0;
		}

		// Print the clause
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, line.data());
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "}");

	// Print the variables
	/* TODO Reenable later
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Variables = {");
	for (auto variableIt = m_variables.cbegin(); variableIt != m_variables.cend(); ++variableIt) {
		Variable* variable = *variableIt;

		// Variable id
		std::string line = "   x" + std::to_string(variable->id()) + " +{";

		// Positive occurences
		for (auto occurenceIt = variable->beginPositiveOccurences(); occurenceIt != variable->endPositiveOccurences(); ++occurenceIt) {
			Clause* clause = *occurenceIt;
			line.append(" ").append(std::to_string(clause->id()));
		}

		line.append("}  -{ ");

		// Negative occurences
		for (auto occurenceIt = variable->beginNegativeOccurences(); occurenceIt != variable->endNegativeOccurences(); ++occurenceIt) {
			Clause* clause = *occurenceIt;
			line.append(" ").append(std::to_string(clause->id()));
		}

		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, line.append("}").data());
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "}");
	*/
}
