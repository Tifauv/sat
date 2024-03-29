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
#include "History.h"

#include "RemoveClauseStep.h"
#include "RemoveLiteralFromClauseStep.h"
#include "Formula.h"
#include "Clause.h"
#include "utils.h"
#include "log.h"


namespace sat {
namespace history {


/**
 * Adds a step for a "remove clause" operation as last step of the history.
 *
 * @param p_clause
 *            the clause to save
 */
void History::addClause(const std::shared_ptr<Clause>& p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_history, "The clause to add is NULL.");
		return;
	}

	// Add the new step
	m_steps.push( std::make_unique<RemoveClauseStep>(p_clause) );
	log_info(log_history, "Clause %u added to the history.", p_clause->id());
}


/**
 * Adds a step for a "remove literal from clause" operation as last step of the history.
 *
 * @param p_clause
 *            the clause
 * @param p_literal
 *            the literal to save
 */
void History::addLiteral(const std::shared_ptr<Clause>& p_clause, Literal p_literal) {
	// Parameters check
	if (isNull(p_clause)) {
		log_error(log_history, "The clause to add is NULL.");
		return;
	}

	// Add the new step
	m_steps.push( std::make_unique<RemoveLiteralFromClauseStep>(p_clause, p_literal) );
	log_info(log_history, "Literal %sx%u of clause %u added to the history.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), p_clause->id());
}


/**
 * Replays the modifications stored in the history.
 *
 * @param p_formula
 *            the formula in which to replay the operations
 */
void History::replay(Formula& p_formula) {
	// Replaying...
	log_debug(log_history, "Replaying the history...");
	while (!m_steps.empty()) {
		m_steps.top()->undo(p_formula);
		m_steps.pop();
	}

	// Result
	log_info(log_history, "History replayed.");
}

} // namespace sat::history
} // namespace sat
