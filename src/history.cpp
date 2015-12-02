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
#include "history.h"

#include <log4c.h>
#include "formula.h"
#include "clause.h"
#include "utils.h"
#include "log.h"



/**
 * Creates a new history.
 */
History::History() {
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "New history created.");
}


/**
 * Frees the memory used by an history.
 */
History::~History() {
	clear();
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "History deleted.");
}


/**
 * Adds an operation of type Operation::AddClause as last step of the history.
 *
 * @param p_clause
 *            the clause to save
 */
void History::addClause(Clause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The clause to add is NULL.");
		return;
	}

	// Create the new step
	Step* newStep = new Step(Operation::AddClause, p_clause);

	// Add the new step
	m_steps.push_front(newStep);
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Clause %u added to the history.", p_clause->id());
}


/**
 * Adds an operation of type Operation::AddLiteralToClause as last step of the history.
 *
 * @param p_clause
 *            the clause
 * @param p_literal
 *            the literal to save
 */
void History::addLiteral(Clause* p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The clause to add is NULL.");
		return;
	}

	// Create the new step
	Step* newStep = new Step(Operation::AddLiteralToClause, p_clause, p_literal);
	
	// Add the new step
	m_steps.push_back(newStep);
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u of clause %u added to the history.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), p_clause->id());
}


/**
 * Replays the modifications stored in the history.
 *
 * @param p_formula
 *            the formula in which to replay the operations
 */
void History::replay(Formula& p_formula) {
	// Replaying...
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Replaying the history...");
	for (auto it = m_steps.begin(); it != m_steps.end(); it = m_steps.erase(it)) {
		Clause* clause  = (*it)->clause();
		Literal literal = (*it)->literal();

		switch ((*it)->operation()) {
			case Operation::AddClause:
				p_formula.addClause(clause);
				log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Added clause %u", clause->id());
				break;

			case Operation::AddLiteralToClause:
				p_formula.addLiteralToClause(clause, literal);
				log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Added %sx%u to clause %u", (literal.isNegative() ? "¬" : ""), literal.id(), clause->id());
				break;

			default:
				log4c_category_log(log_history(), LOG4C_PRIORITY_WARN, "An unknown operation code (%u) has been found in the history, skipping.", (*it)->operation());
		}

		// Delete the step
		delete *it;
	}

	// Result
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "History replayed.");
}


/**
 * Removes and frees the last step of the history.
 */
void History::clear() {
	for (auto it = m_steps.begin(); it != m_steps.end(); it = m_steps.erase(it))
		delete *it;
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "History cleared.");
}


History::Step::Step(History::Operation p_operation, Clause* p_clause) :
m_operation(p_operation),
m_clause(p_clause),
m_literal() {
}


History::Step::Step(History::Operation p_operation, Clause* p_clause, Literal p_literal) :
m_operation(p_operation),
m_clause(p_clause),
m_literal(p_literal) {
}


History::Operation History::Step::operation() const {
	return m_operation;
}


Clause* History::Step::clause() const {
	return m_clause;
}


Literal History::Step::literal() const {
	return m_literal;
}
