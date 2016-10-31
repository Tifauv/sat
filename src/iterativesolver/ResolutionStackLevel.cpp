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
#include "ResolutionStackLevel.h"


#include <assert.h>
#include <log4c.h>
#include "log.h"

namespace sat {
namespace solver {

// METHODS
/**
 * Appends a literal.
 *
 * @param p_literal
 *            the literal
 */
void ResolutionStackLevel::pushLiteral(Literal p_literal) {
	m_literals.push_back(p_literal);
	log4c_category_info(log_history, "Literal %sx%u added to the current resolution level.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
}


/**
 * Gives the first literal.
 * This is the literal that was selected by a decision.
 *
 * @return the last literal
 */
Literal ResolutionStackLevel::firstLiteral() const {
	assert(!m_literals.empty());

	return m_literals.front();
}


/**
 * Returns the literals in this level.
 *
 * @return the current list of literals
 */
const list<Literal> ResolutionStackLevel::literals() const {
	return m_literals;
}


/**
 * Save a removed clause in the history.
 *
 * @param p_clause
 *            the clause that was removed from the formula
 */
void ResolutionStackLevel::saveRemovedClause(shared_ptr<Clause> p_clause) {
	m_history.addClause(p_clause);
}


/**
 * Save a literal removed from a clause in the history.
 *
 * @param p_clause
 *            the clause that was modified
 * @param p_literal
 *            the literal removed from that clause
 */
void ResolutionStackLevel::saveRemovedLiteralFromClause(shared_ptr<Clause> p_clause, Literal p_literal) {
	m_history.addLiteral(p_clause, p_literal);
}


/**
 * Replays the current history upon a given formula.
 *
 * @param p_formula
 *            the formula in which to replay the history
 */
void ResolutionStackLevel::replay(Formula &p_formula) {
	m_history.replay(p_formula);
}

} // namespace sat::solver
} // namespace sat
