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
#include "cnf.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <log4c.h>

#include "dpllsolver.h"
#include "utils.h"
#include "log.h"


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
int cnf_exists_literal(RawLiteral& p_literal, std::list<RawLiteral>& p_literals) {
	for (auto it = p_literals.cbegin(); it != p_literals.cend(); ++it) {
		RawLiteral literal = *it;
		if ( p_literal.id() == literal.id() )
			return p_literal.sign() * literal.sign();
	}
	return 0;
}


/**
 * Parses a clause line from a cnf file.
 * 
 */
std::list<RawLiteral>* cnf_parse_clause(std::string p_line) {
	// I/ Création du tableau
	auto literals = new std::list<RawLiteral>();
	
	// III/ Initialisation du tableau
	std::istringstream source(p_line);
	int token;
	while ((source >> token) && (notNull(literals))) {
		// If the '0' token is found, this is the end of the clause.
		// NOTE this is not conformant to teh CNF format specification because some other
		// clause might follow. But current test files do not use that feature.
		if (token == 0)
			break;

		// Parse current token
		RawLiteral literal(token);
		
		// On teste si l'entier n'apparaît pas déjà dans la variable
		switch (cnf_exists_literal(literal, *literals)) {
			case 1: // Le token apparaît 2 fois avec le même "signe" -> pas ajouté cette fois
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "  - Literal %sx%u already parsed in that clause, skipped.", (literal.isNegative() ? "¬" : ""), literal.id());
				break;
				
			case -1: // Le token et son contraire apparaîssent -> literals = nullptr
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "   - Literal %sx%u already parsed in that clause so it is always true.", (literal.isNegative() ? "¬" : ""), literal.id());
				delete literals;
				literals = nullptr;
				break;
				
			default:
				literals->push_back(literal);
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "  - Literal %sx%u parsed.", (literal.isNegative() ? "¬" : ""), literal.id());
		}
	}
	
	return literals;
}


/**
 * Loads a SAT problem from a CNF file.
 * 
 * @param p_filename
 *            the name of the file to load
 * @param p_formula
 *            the formula to initialize
 */
void cnf_load_problem(char* p_filename, Formula& p_formula) {
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Loading problem from CNF file '%s'...", p_filename);

	// Ouverture du fichier
	std::ifstream file(p_filename);
	if (!file.is_open()) {
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_ERROR, "Could not open file '%s'.", p_filename);
		return;
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' opened.", p_filename);

	// Initializations
	std::string line;
	unsigned int lineNo = 0;
	unsigned int clauseId = 1;

	while (std::getline(file, line)) {
		lineNo++;
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Line #%02d: |%s|", lineNo, line.c_str());
    
		// Ignore comment lines
		if (line[0] == 'c')
			continue;
    
		// Ignore project lines
		if (line[0] == 'p')
			continue;
    
		// Break at '%' lines
		if (line[0] == '%')
			break;

		// Transformation string -> tab
		std::list<RawLiteral>* literals = cnf_parse_clause(line);
		p_formula.createClause(clauseId, *literals);
		++clauseId;
		delete literals;
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_INFO, "Problem loaded from CNF file '%s'.", p_filename);
}


/**
 * Loads a SAT solution from a SAT file.
 * 
 * @param p_filename
 *            the name of the file to load
 * 
 * @return nullptr if p_filename is nullptr,
 *         the interpretation loaded otherwise
 */
std::list<RawLiteral>* cnf_load_solution(char* p_filename) {
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Loading solution from SAT file '%s'...", p_filename);

	// Ouverture du fichier
	std::ifstream file(p_filename);
	if (!file.is_open()) {
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_ERROR, "Could not open file '%s'.", p_filename);
		return nullptr;
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' opened.", p_filename);
	
	// Initializations
	std::list<RawLiteral>* solution = nullptr;
	std::string line;
	unsigned int lineNo = 0;

	while (std::getline(file, line)) {
		lineNo++;
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Line #%02d: |%s|", lineNo, line.c_str());
    
		// Ignore comment lines
		if (line[0] == 'c')
			continue;
    
		// Ignore project lines
		if (line[0] == 'p')
			continue;
    
		// Solution
		if (line[0] == 'v') {
			solution = cnf_parse_clause(line.erase(0, 2)); // +2 to skip the two first characters: "v "
			break;
		}
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_INFO, "Solution loaded from SAT file '%s'.", p_filename);

	return solution;
}
