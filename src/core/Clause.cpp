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
#include "Clause.h"

#include <log4c.h>
#include "Literal.h"
#include "utils.h"
#include "log.h"


// CONSTRUCTORS
/**
 * Initializes a clause.
 *
 * @param p_id
 *            the clause identifier.
 */
Clause::Clause(Id p_id) :
FormulaObject(p_id) {
	log4c_category_debug(log_formula(), "Clause %u created.", id());
}


// DESTRUCTORS
/**
 * Cleans the memory used by the clause.
 */
Clause::~Clause() {
	log4c_category_debug(log_formula(), "Clause %u deleted.", id());
}


// METHODS
/**
 * Adds a literal to the clause.
 *
 * @param p_literal
 *            the literal to add
 */
void Clause::addLiteral(Literal p_literal) {
	m_literals.push_back(p_literal);
	log4c_category_debug(log_formula(), "Literal %sx%u added to clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), id());
}


/**
 * Removes a literal from the clause.
 *
 * @param p_literal
 *            a reference to the literal to remove.
 */
void Clause::removeLiteral(const Literal& p_literal) {
	m_literals.remove(p_literal);
	log4c_category_debug(log_formula(), "Literal %sx%u removed from clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), id());
}


/**
 * Tells whether the clause is unsatisfiable.
 * This is the case when it has no more literal.
 *
 * @return true if the clause is unsatisfiable (no more literal),
 *         false otherwise
 */
bool Clause::isUnsatisfiable() const {
	return m_literals.empty();
}


/**
 * Tells whether the clause is unary (has only one literal).
 *
 * @return true if the clause has exactly one literal,
 *         false otherwise.
 */
bool Clause::isUnary() const {
	return m_literals.size() == 1;
}


/**
 * Gives the first literal of the clause.
 *
 * @return the first literal of the clause,
 *         or a literal pointing to the nullptr variable if the clause is empty
 */
Literal Clause::firstLiteral() const {
	return *m_literals.cbegin();
}


/**
 * Gives an iterator on the first literal of the clause.
 *
 * @return a 'begin' iterator of the literals
 */
std::list<Literal>::const_iterator Clause::beginLiteral() const {
	return m_literals.cbegin();
}


/**
 * Gives an iterator of the end of the literals list.
 *
 * @return an 'end' iterator of the literals
 */
std::list<Literal>::const_iterator Clause::endLiteral() const {
	return m_literals.cend();
}
