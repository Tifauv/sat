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
#include "interpretation.h"

#include <iostream>
#include <log4c.h>
#include "utils.h"
#include "log.h"


// CONSTRUCTORS
/**
 * Creates an empty satisfiable interpretation.
 */
Interpretation::Interpretation() :
m_unsatisfiable(false) {
	log4c_category_debug(log_interpretation(), "New interpretation created.");
}


// DESTRUCTORS
/**
 * Deletes the interpretation.
 */
Interpretation::~Interpretation() {
	log4c_category_debug(log_interpretation(), "Interpretation deleted.");
}


// GETTERS
/**
 * Tells whether this interpretation is satisfiable.
 */
bool Interpretation::isSatisfiable() const {
	return !m_unsatisfiable;
}


/**
 * Tells whether this interpretation is unsatisfiable.
 */
bool Interpretation::isUnsatisfiable() const {
	return m_unsatisfiable;
}


// SETTERS
/**
 * Sets the interpretation satisfiable.
 */
void Interpretation::setSatisfiable() {
	m_unsatisfiable = false;
	log4c_category_debug(log_interpretation(), "The interpretation is set satisfiable.");
}


/**
 * Sets the interpretation unsatisfiable.
 */
void Interpretation::setUnsatisfiable() {
	m_unsatisfiable = true;
	log4c_category_debug(log_interpretation(), "The interpretation is set unsatisfiable.");
}


// METHODS
/**
 * Appends a literal.
 * 
 * @param p_literal
 *            the literal to add
 */
void Interpretation::push(Literal p_literal) {
	m_literals.push_back(p_literal);
	log4c_category_info(log_interpretation(), "Literal %sx%u added to the interpretation.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
	
	// Reset the satisfiability status
	if (isUnsatisfiable()) {
		setSatisfiable();
		log4c_category_info(log_interpretation(), "The interpretation was unsatisfiable, and has now been set satisfiable.");
	}
}


/**
 * Removes the last literal.
 */
void Interpretation::pop() {
	if (m_literals.empty())
		return;
	
	Literal literal = m_literals.back();
	m_literals.pop_back();
	log4c_category_info(log_interpretation(), "Literal %sx%u removed from the interpretation.", (literal.isNegative() ? "¬" : ""), literal.id());
}


/**
 * Logs the interpretation.
 */
void Interpretation::log() {
	if (!log4c_category_is_info_enabled(log_interpretation()))
		return;

	if (isUnsatisfiable()) {
		log4c_category_info(log_interpretation(), "The interpretation is unsatisfiable.");
		return;
	}

	// Otherwise, print the elements
	std::string line = "Interpretation:";
	for (auto it = m_literals.cbegin(); it != m_literals.cend(); ++it) {
		const Literal literal = *it;
		line.append("  ").append(literal.isNegative() ? "¬" : "").append("x").append(std::to_string(literal.id()));
	}

	log4c_category_info(log_interpretation(), line.data());
}


/**
 * Prints the interpretation to stdout.
 */
void Interpretation::print() {
	if (isUnsatisfiable()) {
		std::cout << "s UNSATISFIABLE" << std::endl;
		return;
	}
	
	// Otherwise, print the elements
	std::cout << "s SATISFIABLE" << std::endl;
	std::cout << "v";
	for (auto it = m_literals.cbegin(); it != m_literals.cend(); ++it) {
		const Literal literal = *it;
		std::cout << " " << (int)(literal.sign() * literal.id());
	}
	std::cout << " 0" << std::endl;
}
