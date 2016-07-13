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
#include <log4c.h>
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
	log4c_category_debug(log_valuation, "New valuation created.");
}


// DESTRUCTORS
/**
 * Deletes the valuation.
 */
Valuation::~Valuation() {
	log4c_category_debug(log_valuation, "Valuation deleted.");
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
	log4c_category_debug(log_valuation, "The valuation is set satisfiable.");
}


/**
 * Sets the valuation unsatisfiable.
 */
void Valuation::setUnsatisfiable() {
	m_unsatisfiable = true;
	log4c_category_debug(log_valuation, "The valuation is set unsatisfiable.");
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
	log4c_category_info(log_valuation, "Literal %sx%u added to the valuation.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	
	// Reset the satisfiability status
	if (isUnsatisfiable()) {
		setSatisfiable();
		log4c_category_info(log_valuation, "The valuation was unsatisfiable, and has now been set satisfiable.");
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
	log4c_category_info(log_valuation, "Literal %sx%u removed from the valuation.", (literal.isNegative() ? "¬" : ""), literal.id());
}


/**
 * Logs the valuation.
 */
void Valuation::log() {
	if (!log4c_category_is_info_enabled(log_valuation))
		return;

	if (isUnsatisfiable()) {
		log4c_category_info(log_valuation, "The valuation is unsatisfiable.");
		return;
	}

	// Otherwise, print the elements
	std::string line = "Valuation:";
	for (const auto& literal : m_literals)
		line.append("  ").append(literal.isNegative() ? "¬" : "").append("x").append(std::to_string(literal.id()));

	log4c_category_info(log_valuation, line.data());
}


/**
 * Prints the valuation to stdout.
 */
void Valuation::print() {
	if (isUnsatisfiable()) {
		std::cout << "s UNSATISFIABLE" << std::endl;
		return;
	}
	
	// Otherwise, print the elements
	std::cout << "s SATISFIABLE" << std::endl;
	std::cout << "v";
	for (const auto& literal : m_literals)
		std::cout << " " << (int)(literal.sign() * literal.id());
	std::cout << " 0" << std::endl;
}

} // namespace sat::solver
} // namespace sat