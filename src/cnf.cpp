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
#include "cnf.h"

#include <iostream>
#include <string.h>
#include <log4c.h>
#include <list>

#include "dpllsolver.h"
#include "utils.h"
#include "log.h"

#define LN_SIZE 1024


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
std::list<RawLiteral>* cnf_parse_clause(char* p_line, size_t p_size) {
	// Sauvegarde de la chaîne originale
	char* lineCopy = (char*) malloc(p_size);
	strcpy(lineCopy, p_line);
	
	// I/ Création du tableau
	auto literals = new std::list<RawLiteral>();
	
	// III/ Initialisation du tableau
	char* token = strtok(lineCopy, " ");
	while (token && notNull(literals)) {
		// Parse current token
		RawLiteral literal(atoi(token));
		
		// Prepare next token
		token = strtok(NULL, " ");
		
		// Break for this line if the '0' token has been found
		if (literal.id() == 0)
			break;
		
		// On teste si l'entier n'apparaît pas déjà dans la variable
		int exists = cnf_exists_literal(literal, *literals);
		switch (exists) {
			case 1: // Le token apparaît 2 fois avec le même "signe" -> pas ajouté cette fois
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "  - Literal %sx%u already parsed in that clause, skipped.", (literal.isNegative() ? "¬" : ""), literal.id());
				break;
				
			case -1: // Le token et son contraire apparaîssent -> literals = NULL
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "   - Literal %sx%u already parsed in that clause so it is always true.", (literal.isNegative() ? "¬" : ""), literal.id());
				delete literals;
				literals = NULL;
				break;
				
			default:
				literals->push_back(literal);
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "  - Literal %sx%u parsed.", (literal.isNegative() ? "¬" : ""), literal.id());
		}
	}
	
	free(lineCopy);
	return literals;
}


/**
 * Loads a SAT problem from a CNF file.
 * 
 * @param p_filename
 *            the name of the file to load
 * 
 * @return NULL if p_filename is NULL,
 *         the formula loaded otherwise
 */
void cnf_load_problem(char* p_filename, Formula& p_formula) {
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Loading problem from CNF file '%s'...", p_filename);

	// Ouverture du fichier
	FILE* fic = fopen(p_filename, "r");
	if (fic == NULL) {
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_ERROR, "Could not open file '%s'.", p_filename);
		return;
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' opened.", p_filename);

	// Initialisation de str
	char* str = (char*) malloc(LN_SIZE);
	strcpy(str, "\0");

	unsigned int lineNo = 0;
	unsigned int clauseId = 1;
	while (!feof(fic)) {
		// Compteur de ligne
		lineNo++;

		// lecture de la ligne
		fgets(str, LN_SIZE, fic);
		if (feof(fic))
			break;
		
		str[strlen(str)-1] = '\0';
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Line #%02d: |%s|", lineNo, str);
    
		// Ignore comment lines
		if (str[0] == 'c')
			continue;
    
		// Ignore project lines
		if (str[0] == 'p')
			continue;
    
		// Break at '%' lines
		if (str[0] == '%')
			break;

		// Transformation string -> tab
		std::list<RawLiteral>* literals = cnf_parse_clause(str, LN_SIZE);

		p_formula.createClause(clauseId, *literals);
		++clauseId;
		delete literals;
	}
	fclose(fic);
	free(str);
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' closed.", p_filename);
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_INFO, "Problem loaded from CNF file '%s'.", p_filename);
}


/**
 * Loads a SAT solution from a SAT file.
 * 
 * @param p_filename
 *            the name of the file to load
 * 
 * @return NULL if p_filename is NULL,
 *         the interpretation loaded otherwise
 */
std::list<RawLiteral>* cnf_load_solution(char* p_filename) {
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Loading solution from SAT file '%s'...", p_filename);

	// Ouverture du fichier
	FILE* fic = fopen(p_filename, "r");
	if (fic == NULL) {
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_ERROR, "Could not open file '%s'.", p_filename);
		return NULL;
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' opened.", p_filename);
	
	// Initialisation de formula
	std::list<RawLiteral>* solution = NULL;

	// Initialisation de str
	char* str = (char*) malloc(LN_SIZE);
	strcpy(str, "\0");

	unsigned int lineNo = 0;
	while (!feof(fic)) {
		// lecture de la ligne
		fgets(str, LN_SIZE, fic);
		if (feof(fic))
			break;
		
		str[strlen(str)-1] = '\0';
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Line #%02d: |%s|", lineNo, str);
    
		// Ignore comment lines
		if (str[0] == 'c')
			continue;
    
		// Ignore project lines
		if (str[0] == 'p')
			continue;
    
		// Solution
		if (str[0] == 'v') {
			solution = cnf_parse_clause(str+2, LN_SIZE); // +2 to skip the two first characters: "v "
			break;
		}
	}
	fclose(fic);
	free(str);
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' closed.", p_filename);
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_INFO, "Solution loaded from SAT file '%s'.", p_filename);

	return solution;
}
