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
#ifndef INTERPRETATION_H
#define INTERPRETATION_H

#include "literal.h"
#include <list>


/**
 * A SAT interpretation is a list of literals.
 * Additionnaly, it can be marked unsatisfiable.
 */
class Interpretation {
public:
	/**
	 * Creates an empty satisfiable interpretation.
	 */
	Interpretation();

	/**
	 * Deletes the interpretation.
	 */
	~Interpretation();

	/**
	 * Tells whether this interpretation is satisfiable.
	 */
	bool isSatisfiable() const;

	/**
	 * Tells whether this interpretation is unsatisfiable.
	 */
	bool isUnsatisfiable() const;

	/**
	 * Sets the interpretation satisfiable.
	 */
	void setSatisfiable();

	/**
	 * Sets the interpretation unsatisfiable.
	 */
	void setUnsatisfiable();

	/**
	 * Appends a literal.
	 * 
	 * @param p_literal
	 *            the literal
	 */
	void push(Literal p_literal);

	/**
	 * Removes the last literal.
	 */          
	void pop();

	/**
	 * Logs the interpretation.
	 */
	void log();

	/**
	 * Prints the interpretation to stdout.
	 */
	void print();

private:
	/** The satisfiability flag. */
	bool m_unsatisfiable;
	
	/** The list of literals. */
	std::list<Literal> m_literals;
};

#endif // INTERPRETATION_H
