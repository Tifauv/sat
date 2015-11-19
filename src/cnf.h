/* Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki
   Copyright (C) 2015 Olivier Serve

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
*/
#ifndef CNF_H
#define CNF_H

#include <list>
#include "sat.h"

using namespace std;

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
int cnf_exists_literal(Literal p_literal, std::list<Literal>& p_literals);


/**
 * Parses a clause line from a cnf file.
 * 
 */
std::list<Literal>* cnf_parse_clause(char* p_line, size_t p_size);


/**
 * Loads a SAT problem from a CNF file.
 * 
 * @param p_filename
 *            the name of the file to load
 * 
 * @return NULL if p_filename is NULL,
 *         the formula loaded otherwise
 */
void cnf_load_problem(char* p_filename, tGraphe& p_formula);


/**
 * Loads a SAT solution from a SAT file.
 * 
 * @param p_filename
 *            the name of the file to load
 * 
 * @return NULL if p_filename is NULL,
 *         the interpretation loaded otherwise
 */
std::list<Literal>* cnf_load_solution(char* p_filename);


/**
 * Check whether a given solution is a valid interpretation of a formula.
 * 
 * @param p_formula
 *            the formula
 * @param p_solution
 *            the solution to check
 * 
 * @return true if the solution satisfies teh formula,
 *         false otherwise
 */
bool cnf_check_solution(tGraphe& p_formula, std::list<Literal>* p_solution);

#endif // CNF_H
