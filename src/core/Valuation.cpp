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
#include "Valuation.h"

#include <assert.h>
#include <iostream>
#include "utils.h"
#include "log.h"


namespace sat {
namespace solver {


// CONSTRUCTORS
/**
 * Creates an empty satisfiable valuation.
 */
Valuation::Valuation() :
m_unsatisfiable(false) {
}


// GETTERS
/**
 * Tells whether this valuation is satisfiable.
 */
bool Valuation::isSatisfiable() const {
	return !m_unsatisfiable;
}


/**
 * Tells whether this valuation is unsatisfiable.
 */
bool Valuation::isUnsatisfiable() const {
	return m_unsatisfiable;
}


// SETTERS
/**
 * Sets the valuation satisfiable.
 */
void Valuation::setSatisfiable() {
	m_unsatisfiable = false;
	log_debug(log_valuation, "The valuation is set satisfiable.");
}


/**
 * Sets the valuation unsatisfiable.
 */
void Valuation::setUnsatisfiable() {
	m_unsatisfiable = true;
	log_debug(log_valuation, "The valuation is set unsatisfiable.");
}


// METHODS
/**
 * Appends a literal.
 * 
 * @param p_literal
 *            the literal to add
 */
void Valuation::push(Literal p_literal) {
	m_literals.push_back(p_literal);
	log_info(log_valuation, "Literal %sx%u added to the valuation.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	
	// Reset the satisfiability status
	if (isUnsatisfiable()) {
		setSatisfiable();
		log_info(log_valuation, "The valuation was unsatisfiable, and has now been set satisfiable.");
	}
}


/**
 * Gives the last literal.
 * This is the one that would be removed by a call to {@link #pop()}.
 *
 * @return the last literal
 */
Literal Valuation::top() const {
	assert(!m_literals.empty());

	return m_literals.back();
}


/**
 * Removes the last literal.
 */
void Valuation::pop() {
	if (m_literals.empty())
		return;

	Literal literal = m_literals.back();
	m_literals.pop_back();
	log_info(log_valuation, "Literal %sx%u removed from the valuation.", (literal.isNegative() ? "¬" : ""), literal.id());
}


/**
 * Logs the valuation.
 */
void Valuation::log() {
	if (!log_is_info_enabled(log_valuation))
		return;

	if (isUnsatisfiable()) {
		log_info(log_valuation, "The valuation is unsatisfiable.");
		return;
	}

	// Otherwise, print the elements
	string line = "Valuation:";
	for (const auto& literal : m_literals)
		line.append("  ").append(literal.isNegative() ? "¬" : "").append("x").append(to_string(literal.id()));

	log_info(log_valuation, line.data());
}


/**
 * Prints the valuation to the given stream.
 */
ostream& operator<<(ostream& p_outStream, const Valuation& p_valuation) {
	if (p_valuation.isUnsatisfiable()) {
		p_outStream << "s UNSATISFIABLE" << endl;
		return p_outStream;
	}

	// Otherwise, print the elements
	p_outStream << "s SATISFIABLE" << endl;
	p_outStream << "v";
	for (const auto& literal : p_valuation.m_literals)
		p_outStream << " " << (int)(literal.sign() * literal.id());
	p_outStream << " 0" << endl;
	return p_outStream;
}

} // namespace sat::solver
} // namespace sat
