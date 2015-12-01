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
#include "clause.h"

#include <log4c.h>
#include "utils.h"
#include "log.h"


// CONSTRUCTORS
/**
 *
 */
Clause::Clause(Id p_id) :
FormulaObject(p_id) {
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u created.", id());
}


// DESTRUCTORS
/**
 *
 */
Clause::~Clause() {
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u deleted.", id());
}


// METHODS
void Clause::addLiteral(Literal p_literal) {
	m_literals.push_back(p_literal);
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u added to clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), id());
}


void Clause::removeLiteral(const Literal& p_literal) {
	m_literals.remove(p_literal);
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal %sx%u removed from clause %u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id(), id());
}


/**
 * A clause becomes unsatisfiable if it has no literal.
 */
bool Clause::isUnsatisfiable() const {
	return m_literals.empty();
}


bool Clause::isUnary() const {
	return m_literals.size() == 1;
}


Literal Clause::firstLiteral() const {
	return *m_literals.cbegin();
}


std::list<Literal>::const_iterator Clause::beginLiteral() const {
	return m_literals.cbegin();
}


std::list<Literal>::const_iterator Clause::endLiteral() const {
	return m_literals.cend();
}
