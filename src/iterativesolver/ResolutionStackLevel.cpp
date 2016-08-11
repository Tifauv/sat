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

// CONSTRUCTORS
ResolutionStackLevel::ResolutionStackLevel() {
	log4c_category_debug(log_history, "New resolution level created.");
}


// DESTRUCTORS
ResolutionStackLevel::~ResolutionStackLevel() {
	log4c_category_debug(log_history, "Resolution level deleted.");
}


// METHODS
void ResolutionStackLevel::pushLiteral(Literal p_literal) {
	m_literals.push_back(p_literal);
	log4c_category_info(log_history, "Literal %sx%u added to the current resolution level.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
}


Literal ResolutionStackLevel::firstLiteral() const {
	assert(!m_literals.empty());

	return m_literals.front();
}

const std::list<Literal> ResolutionStackLevel::literals() const {
	return m_literals;
}

void ResolutionStackLevel::saveRemovedClause(Clause *p_clause) {
	m_history.addClause(p_clause);
}


void ResolutionStackLevel::saveRemovedLiteralFromClause(Clause *p_clause, Literal p_literal) {
	m_history.addLiteral(p_clause, p_literal);
}

void ResolutionStackLevel::replay(Formula &p_formula) {
	m_history.replay(p_formula);
}

} // namespace sat::solver
} // namespace sat
