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
#include "Formula.h"

#include <algorithm>
#include <functional>
#include "Clause.h"
#include "Variable.h"
#include "Literal.h"
#include "utils.h"
#include "log.h"


namespace sat {


// METHODS
/**
 * Creates a clause in the formula.
 * The needed variables are created if needed.
 *
 * @param p_clauseId
 *            the identifier of the clause
 * @param p_literals
 *            the raw literals
 */
void Formula::createClause(Id p_clauseId, const unique_ptr<vector<RawLiteral>>& p_literals) {
	// Create the clause & add it to the list
	auto clause = make_shared<Clause>(p_clauseId);
	m_clauses.insert(clause);
	log_debug(log_formula, "Clause %u added.", p_clauseId);
	
	// Link the new clause with its literals
	for (const auto& literal : *p_literals) {
		// Find the variable for the literal
		auto variable = findOrCreateVariable(literal.id());

		// Link clause -> variable
		clause->addLiteral(Literal(variable, literal.sign()));

		// Link variable -> clause
		variable->addOccurence(clause, literal.sign());
	}
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
shared_ptr<Variable> Formula::findOrCreateVariable(Id p_variableId) {
	shared_ptr<Variable> variable = nullptr;

	// Search the variable
	auto match_id([p_variableId](const shared_ptr<Variable>& variable) { return variable->id() == p_variableId; } );
	auto iterator = find_if(m_variables.cbegin(), m_variables.cend(), cref(match_id));

	// If the variable was found, select it
	if (iterator != m_variables.cend()) {
		variable = *iterator;
		log_debug(log_formula, "Variable x%u found.", variable->id());
	}
	// Otherwise, create & add it
	else {
		variable = make_shared<Variable>(p_variableId);
		m_variables.insert(variable);
		log_debug(log_formula, "Variable x%u added.", variable->id());
	}
	
	return variable;
}


/**
 * Retrieves a unit literal.
 *
 * @return the literal found,
 *         or a literal pointing to a nullptr variable if there is no unary clause
 */
Literal Formula::findUnitLiteral() const {
	// Search an unary clause
	auto isUnary([](const shared_ptr<Clause>& clause) { return clause->isUnary(); } );
	auto iterator = find_if(m_clauses.cbegin(), m_clauses.cend(), cref(isUnary));

	// If a clause has been found, retrieve its literal
	if (iterator != m_clauses.cend()) {
		auto clause = *iterator;
		Literal unitLiteral = clause->firstLiteral();
		log_debug(log_formula, "Unit literal %sx%u found in clause %u.", (unitLiteral.isNegative() ? "¬" : ""), unitLiteral. id(), clause->id());
		return unitLiteral;
	}

	// No unary clause found
	return Literal(nullptr, SIGN_POSITIVE);
}


/**
 * Removes a link from a variable to a clause.
 *
 * @param p_clause
 *            the targetted clause
 * @param p_literal
 *            the literal to remove
 */
void Formula::unlinkVariable(const shared_ptr<Clause>& p_clause, const Literal& p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_formula, "Cannot remove a literal from a NULL clause.");
		return;
	}

	// Remove the clause from the variable occurences
	auto variable = p_literal.var();
	if (p_literal.isPositive())
		variable->removePositiveOccurence(p_clause);
	else
		variable->removeNegativeOccurence(p_clause);

	// Auto-remove unused variables
	if (!variable->hasPositiveOccurence() && !variable->hasNegativeOccurence()) {
		log_info(log_formula, "Variable x%u is not used anymore.", variable->id());
		removeVariable(variable);
	}
}


/**
 * Move the variable to the unused list.
 * 
 * @param p_variable
 *            the variable to remove
 */
void Formula::removeVariable(const shared_ptr<Variable>& p_variable) {
	p_variable->setUnused();
	m_unusedVariables.insert(p_variable);
	m_variables.erase(p_variable);
	log_info(log_formula, "Variable x%u has been removed.", p_variable->id());
}


/**
 * Move a variable to the current list.
 * 
 * @param p_variable
 *            the variable to add
 */
void Formula::addVariable(const shared_ptr<Variable>& p_variable) {
	m_variables.insert(p_variable);
	m_unusedVariables.erase(p_variable);
	p_variable->setUsed();
	log_info(log_formula, "Variable x%u has been added.", p_variable->id());
}


/**
 * Move a clause from the unused to current list.
 * 
 * @param p_clause
 *            the clause
 */
void Formula::addClause(const shared_ptr<Clause>& p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_formula, "Cannot add a NULL clause.");
		return;
	}

	// Move the clause to the current list
	m_clauses.insert(p_clause);
	m_unusedClauses.erase(p_clause);
	p_clause->setUsed();

	// Ensure the linked variables are enabled
	for (auto literalIterator = p_clause->beginLiteral(); literalIterator != p_clause->endLiteral(); ++literalIterator) {
		auto variable = (*literalIterator).var();
		
		// Move the variable to the current list if needed
		if (variable->isUnused())
			addVariable(variable);
		
		// Relink the variable with the clause
		variable->addOccurence(p_clause, (*literalIterator).sign());
	}
	
	log_info(log_formula, "Clause %u added.", p_clause->id());
}


/**
 * Add a literal to a clause.
 * 
 * @param p_clause
 *            the clause
 * @param p_literal
 *            the literal to add
 */
void Formula::addLiteralToClause(const shared_ptr<Clause>& p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_formula, "Cannot add a literal to a NULL clause.");
		return;
	}

	// Link clause -> variable
	p_clause->addLiteral(p_literal);

	// Link variable -> clause
	p_literal.var()->addOccurence(p_clause, p_literal.sign());
	log_debug(log_formula, "Literal %sx%u added to clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), p_clause->id());
}


/**
 * 
 */
void Formula::removeClause(const shared_ptr<Clause>& p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_formula, "Cannot remove a NULL clause.");
		return;
	}
	
	log_debug(log_formula, "Removing clause %u.", p_clause->id());
	// Remove all links clause -> variables except the current iterator
	for (auto literalIterator = p_clause->beginLiteral(); literalIterator != p_clause->endLiteral(); ++literalIterator)
		unlinkVariable(p_clause, *literalIterator);
	
	// Move the clause to the unused list
	p_clause->setUnused();
	m_clauses.erase(p_clause);
	m_unusedClauses.insert(p_clause);
	
	log_info(log_formula, "Clause %u removed.", p_clause->id());
}


/**
 * 
 */
void Formula::removeLiteralFromClause(const shared_ptr<Clause>& p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_formula, "Cannot add a literal to a NULL clause.");
		return;
	}
	
	p_clause->removeLiteral(p_literal);
	unlinkVariable(p_clause, p_literal);
	log_info(log_formula, "Literal %sx%u removed from clause %u.", (p_literal.isPositive() ? "¬" : ""), p_literal.id(), p_clause->id());
}


/**
 * Tells whether there are clauses in the formula.
 */
bool Formula::hasClauses() const {
	return !m_clauses.empty();
}


/**
 * Tells whether there are variables in the formula.
 */
bool Formula::hasVariables() const {
	return !m_variables.empty();
}


/**
 * 
 */
unordered_set<shared_ptr<Variable>>::iterator Formula::beginVariable() {
	return m_variables.begin();
}


/**
 * 
 */
unordered_set<shared_ptr<Variable>>::iterator Formula::endVariable() {
	return m_variables.end();
}


/**
 * Logs the current formula to the formula logger.
 */
void Formula::log() const {
	if (!log_is_debug_enabled(log_formula))
		return;

	// Print the clauses
	log_debug(log_formula, "Clauses = {");
	for (const auto& clause : m_clauses) {
		// Clause id
		string line = "   " + to_string(clause->id()) + ": ";

		// Clause's literals
		int deb = 1;
		for (auto literalIt = clause->beginLiteral(); literalIt != clause->endLiteral(); ++literalIt) {
			const Literal literal = *literalIt;

			line.append(deb != 1 ? " v " : "").append(literal.isNegative() ? "¬x" : "x").append(to_string(literal.id()));
			if (deb == 1)
				deb = 0;
		}

		// Print the clause
		log_debug(log_formula, line.data());
	}
	log_debug(log_formula, "}");

	// Print the variables
	log_debug(log_formula, "Variables = {");
	for (const auto& variable : m_variables) {
		// Variable id
		string line = "   x" + to_string(variable->id()) + " \t+{";

		// Positive occurences
		for (auto clause = variable->beginOccurence(SIGN_POSITIVE); clause != variable->endOccurence(SIGN_POSITIVE); ++clause)
			line.append(" ").append(to_string((*clause)->id()));

		line.append(" } \t-{");

		// Negative occurences
		for (auto clause = variable->beginOccurence(SIGN_NEGATIVE); clause != variable->endOccurence(SIGN_NEGATIVE); ++clause)
			line.append(" ").append(to_string((*clause)->id()));

		log_debug(log_formula, line.append(" }").data());
	}
	log_debug(log_formula, "}");
}

} // namespace sat
