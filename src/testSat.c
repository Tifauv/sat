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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log4c.h>

#include "sat.h"
#include "solver.h"
#include "utils.h"
#include "log.h"


/**
 * Displays the usage message.
 */
void usage() {
	printf("Usage: testSat <cnf_file>\n");
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
tGraphe* load_cnf_file(char* p_filename) {
	log4c_category_t* loader = log4c_category_get(LOG_CATEGORY_LOADER_CNF);

	log4c_category_log(loader, LOG4C_PRIORITY_DEBUG, "Loading problem from CNF file '%s'...", p_filename);

	// Ouverture du fichier
	FILE* fic = fopen(p_filename, "r");
	if (fic == NULL) {
		log4c_category_log(loader, LOG4C_PRIORITY_ERROR, "Could not open file '%s'.", p_filename);
		return NULL;
	}
	log4c_category_log(loader, LOG4C_PRIORITY_DEBUG, "File '%s' opened.", p_filename);
	
	// Initialisation de p_formula
	tGraphe* p_formula = sat_new();

	// Initialisation de str
	char* str = malloc(LN_SIZE);
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
		log4c_category_log(loader, LOG4C_PRIORITY_DEBUG, "Line #%02d: |%s|", lineNo, str);
    
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
		int size = 0;
		int* tab = sat_mk_tabVar(str, &size);

		if (sat_add_clause(p_formula, clauseId, tab, size))
			log4c_category_log(loader, LOG4C_PRIORITY_INFO, "Clause for line %u was not added.", lineNo);
		else
			++clauseId;
		free(tab);
	}
	fclose(fic);
	free(str);
	log4c_category_log(loader, LOG4C_PRIORITY_DEBUG, "File '%s' closed.", p_filename);
	log4c_category_log(loader, LOG4C_PRIORITY_INFO, "Problem loaded from CNF file '%s'.", p_filename);

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
		printf("Log4c initialization failed, aborting.\n");
		exit(2);
	}

	char* nom_fic = malloc(32);
	strcpy(nom_fic, p_argv[1]);
	tGraphe* formula = load_cnf_file(nom_fic);
	free(nom_fic);
	sat_see(formula);

	printf("\n\n> Résolution:\n");
	tIntr* interpretation = alg_solve(&formula);
	intr_see(interpretation);

	printf("\n\n> Libération de la mémoire:\n");
	intr_free(&interpretation);
	sat_free(&formula);

	printf("\n\n> Terminé\n\n");

	// Clean the logging system
	if (log4c_fini())
		printf("log4c cleanup failed.");

	return 0;
}
