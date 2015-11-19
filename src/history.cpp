/* Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki
   Copyright (C) 2015 Olivier Serve

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
#include "history.h"

#include <log4c.h>
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
void History::addClause(tClause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The clause to add is NULL.");
		return;
	}

	// Check the clause is not empty
	if (isNull(p_clause->vars)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The clause to add is empty.");
		return;
	}

	// Create the literals array
	std::list<Literal> literals;
	tPtVar* literalIter = p_clause->vars;
	while (notNull(literalIter)) {
		literals.push_back( literalIter->var->indVar * sat_get_sign(literalIter->var, p_clause->indCls) );
		literalIter = literalIter->suiv;
	}
	
	// Create the new step
	Step* newStep = new Step();
	newStep->operation = Operation::AddClause;
	newStep->clauseId = p_clause->indCls;
	newStep->literals = literals;

	// Add the new step
	m_steps.push_front(newStep);
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Clause %u added to the history.", p_clause->indCls);
}


/**
 * Adds an operation of type Operation::AddLiteralToClause as last step of the history.
 *
 * @param p_clauseId
 *            the id of the clause
 * @param p_literal
 *            the literal to save
 */
void History::addLiteral(ClauseId p_clauseId, Literal p_literal) {
	// Create the literals array
	std::list<Literal> literals(1);
	literals.push_front( p_literal );
	
	// Create the new step
	Step* newStep = new Step();
	newStep->operation = Operation::AddLiteralToClause;
	newStep->clauseId = p_clauseId;
	newStep->literals = literals;
	
	// Add the new step
	m_steps.push_back(newStep);
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u of clause %u added to the history.", (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal), p_clauseId);
}


/**
 * Replays the modifications stored in the history.
 *
 * @param p_formula
 *            the formula in which to replay the operations
 */
void History::replay(tGraphe& p_formula) {
	// Replaying...
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Replaying the history...");
	for (auto it = m_steps.begin(); it != m_steps.end(); it = m_steps.erase(it)) {
		ClauseId clauseId = (*it)->clauseId;
		std::list<Literal> literals = (*it)->literals;

		switch ((*it)->operation) {
			case Operation::AddClause:
				sat_add_clause(&p_formula, clauseId, literals);
				log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Added clause %u", clauseId);
				break;

			case Operation::AddLiteralToClause:
				sat_add_var_to_cls(&p_formula, clauseId, literals.front());
				log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Added %sx%u to clause %u", (literals.front() < 0 ? "¬" : ""), sat_literal_id(literals.front()), clauseId);
				break;

			default:
				log4c_category_log(log_history(), LOG4C_PRIORITY_WARN, "An unknown operation code (%u) has been found in the history, skipping.", (*it)->operation);
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
