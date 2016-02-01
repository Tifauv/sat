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
#ifndef CNFLOADER_H
#define CNFLOADER_H

#include <vector>
#include "Formula.h"


class CnfLoader {
public:
	/**
	* Loads a SAT problem from a CNF file.
	* 
	* @param p_filename
	*            the name of the file to load
	* @param p_formula
	*            the formula to initialize
	*/
	void loadProblem(char* p_filename, Formula& p_formula);


	/**
	* Loads a SAT solution from a SAT file.
	* 
	* @param p_filename
	*            the name of the file to load
	* 
	* @return nullptr if p_filename is nullptr,
	*         the interpretation loaded otherwise
	*/
	std::vector<RawLiteral>* loadSolution(char* p_filename);


protected:
	/**
	 * Parses a clause line from a cnf file as a list of raw literals.
	 * 
	 * @param p_line
	 *            the line to parse
	 * 
	 * @return the list of raw literals
	 */
	std::vector<RawLiteral>* parseClause(std::string p_line);


	/**
	 * Searches a literal in a list of literals.
	 * 
	 * @param p_literal
	 *            the literal to search
	 * @param p_literals
	 *            the list of literals
	 * 
	 * @return -1 if -p_literal appears in p_literals
	 *          0 if p_literal does not appear in p_literals
	 *          1 if p_literal appears in p_literals
	 */
	int existsLiteral(RawLiteral& p_literal, std::vector<RawLiteral>& p_literals);
};

#endif // CNFLOADER_H
