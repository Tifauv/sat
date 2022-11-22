/*  Copyright 2022 Olivier Serve
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
#include "ClauseBuilder.h"

#include "Formula.h"


namespace sat {


// CONSTRUCTORS
/**
 * Initializes a clause builder.
 *
 * @param p_formula
 *            the formula to create the clause for
 */
ClauseBuilder::ClauseBuilder(Formula& p_formula) :
m_formula(p_formula),
m_literals(nullptr) {
}


// METHODS
/**
 * Prepares this builder for a new clause.
 *
 * @param p_clauseId
 *            the id of the new clause
 */
ClauseBuilder& ClauseBuilder::reset(unsigned int p_clauseId) {
	m_clauseId = p_clauseId;
	m_literals = unique_ptr<vector<RawLiteral>>(new vector<RawLiteral>());
	return *this;
}


/**
 * Adds a positive literal to the clause.
 */
ClauseBuilder& ClauseBuilder::withPositiveLiteral(unsigned int p_variable) {
	 m_literals->push_back(sat::RawLiteral(p_variable));
	return *this;
}

/**
 * Adds a negative literal to the clause.
 */
ClauseBuilder& ClauseBuilder::withNegativeLiteral(unsigned int p_variable) {
	 m_literals->push_back(-sat::RawLiteral(p_variable));
	return *this;
}

/**
 * Builds the clause and add it to the formula.
 */
Formula& ClauseBuilder::build() {
	m_formula.createClause(m_clauseId, m_literals);
	return m_formula;
}

} // namespace sat
