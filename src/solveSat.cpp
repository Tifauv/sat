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

#include <iostream>
#include <string.h>
#include <log4c.h>
#include <list>

#include "sat.h"
#include "solver.h"
#include "utils.h"
#include "log.h"

using namespace std;

/**
 * Displays the usage message.
 */
void usage() {
	cout << "Usage: solveSat <cnf_file>" << endl;
}


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
// Test si la variable actuelle du tableau n'a pas déjà été trouvée
int cnf_exists_literal(Literal p_literal, std::list<Literal>& p_literals) {
	for (auto literal = p_literals.cbegin(); literal != p_literals.cend(); ++literal)
		if ( sat_literal_id(p_literal) == sat_literal_id(*literal) )
			return sat_literal_sign(p_literal * *literal);
	return 0;
}


/**
 * Parses a clause line from a cnf file.
 * 
 */
std::list<Literal>* cnf_parse_clause(char* p_line, size_t p_size) {
	// Sauvegarde de la chaîne originale
	char* lineCopy = (char*) malloc(p_size);
	strcpy(lineCopy, p_line);
	
	// I/ Création du tableau
	auto literals = new std::list<Literal>();
	
	// III/ Initialisation du tableau
	char* token = strtok(lineCopy, " ");
	while (token && notNull(literals)) {
		// Parse current token
		Literal literal = atoi(token);
		
		// Prepare next token
		token = strtok(NULL, " ");
		
		// Break for this line if the '0' token has been found
		if (literal == 0)
			break;
		
		// On teste si l'entier n'apparaît pas déjà dans la variable
		int exists = cnf_exists_literal(literal, *literals);
		switch (exists) {
			case 1: // Le token apparaît 2 fois avec le même "signe" -> pas ajouté cette fois
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "  - Literal %sx%u already parsed in that clause, skipped.", (literal < 0 ? "¬" : ""), sat_literal_id(literal));
				break;
				
			case -1: // Le token et son contraire apparaîssent -> literals = NULL
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "   - Literal %sx%u already parsed in that clause so it is always true.", (literal < 0 ? "¬" : ""), sat_literal_id(literal));
				delete literals;
				literals = NULL;
				break;
				
			default:
				literals->push_back(literal);
				log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "  - Literal %sx%u parsed.", (literal < 0 ? "¬" : ""), sat_literal_id(literal));
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
tGraphe* cnf_load_file(char* p_filename) {
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "Loading problem from CNF file '%s'...", p_filename);

	// Ouverture du fichier
	FILE* fic = fopen(p_filename, "r");
	if (fic == NULL) {
		log4c_category_log(log_cnf(), LOG4C_PRIORITY_ERROR, "Could not open file '%s'.", p_filename);
		return NULL;
	}
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' opened.", p_filename);
	
	// Initialisation de p_formula
	tGraphe* p_formula = sat_new();

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
		std::list<Literal>* literals = cnf_parse_clause(str, LN_SIZE);

		if (sat_add_clause(p_formula, clauseId, *literals))
			log4c_category_log(log_cnf(), LOG4C_PRIORITY_INFO, "Clause for line %u was not added.", lineNo);
		else
			++clauseId;
		delete literals;
	}
	fclose(fic);
	free(str);
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_DEBUG, "File '%s' closed.", p_filename);
	log4c_category_log(log_cnf(), LOG4C_PRIORITY_INFO, "Problem loaded from CNF file '%s'.", p_filename);

	return p_formula;
}


/**
 * Main function.
 * 
 * @param p_argc
 *            the count of arguments in p_argv
 * @param p_argv
 *            the array of command-line arguments
 * 
 * @return 2 if the log4c initialization fails,
 *         1 if no argument was given,
 *         0 otherwise
 */
int main(int p_argc, char* p_argv[]) {
	// No argument: display usage and exit
	if (p_argc == 1) {
		usage();
		exit(1);
	}

	// Initialize the logging system
	if (log4c_init()) {
		cerr << "Log4c initialization failed, aborting." << endl;
		exit(2);
	}

	char* nom_fic = (char*) malloc(32);
	strcpy(nom_fic, p_argv[1]);
	tGraphe* formula = cnf_load_file(nom_fic);
	cout << "c Solution to cnf file " << nom_fic << endl;
	free(nom_fic);
	sat_see(formula);

	Interpretation* interpretation = alg_solve(formula);
	interpretation->print();

	delete interpretation;
	sat_free(formula);
	formula = NULL;

	// Clean the logging system
	if (log4c_fini())
		cerr << "log4c cleanup failed." << endl;

	return 0;
}
