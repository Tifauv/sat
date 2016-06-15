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
#include "SolverStack.h"


namespace sat {
namespace solver {

// CONSTRUCTORS
SolverStack::SolverStack() {
}


// DESTRUCTORS
SolverStack::~SolverStack() {

}


// LEVEL MANAGEMENT
void SolverStack::nextLevel() {
	m_histories.push(new History());
}


void SolverStack::popLevel() {
	History* history = m_histories.top();
	m_histories.pop();
	delete history;
}


std::stack<History*>::size_type SolverStack::currentLevel() const {
	return m_histories.size();
}


// CURRENT LEVEL OPERATIONS
void SolverStack::addClause(Clause* p_clause) {
	m_histories.top()->addClause(p_clause);
}


void SolverStack::addLiteral(Clause* p_clause, Literal p_literal) {
	m_histories.top()->addLiteral(p_clause, p_literal);
}


void SolverStack::replay(Formula& p_formula) const {
	m_histories.top()->replay(p_formula);
}

} // namespace sat::solver
} // namespace sat
