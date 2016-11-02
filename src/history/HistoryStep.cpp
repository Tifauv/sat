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
#include "HistoryStep.h"

#include "Clause.h"


namespace sat {
namespace history {


// CONSTRUCTORS
HistoryStep::HistoryStep(const shared_ptr<Clause>& p_clause) :
m_clause(p_clause),
m_literal() {
}


HistoryStep::HistoryStep(const shared_ptr<Clause>& p_clause, Literal p_literal) :
m_clause(p_clause),
m_literal(p_literal) {
}


// GETTERS
/**
 * Give the clause recorded in this step.
 * 
 * @return a pointer to the clause
 */
shared_ptr<Clause> HistoryStep::clause() const {
	return m_clause;
}


/**
 * Give the literal recorder in this step.
 * 
 * @return the literal (can be empty)
 */
Literal HistoryStep::literal() const {
	return m_literal;
}

} // namespace sat::history
} // namespace sat
