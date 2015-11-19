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

#include "cnf.h"
#include "solver.h"

using namespace std;

/**
 * Displays the usage message.
 */
void usage() {
	cout << "Usage: solveSat <cnf_file>" << endl;
	cout << "    <cnf_file>  a CNF problem" << endl;
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
		exit(-1);
	}

	// Initialize the logging system
	if (log4c_init()) {
		cerr << "Log4c initialization failed, aborting." << endl;
		exit(-2);
	}

	// Create the formula
	tGraphe* formula = sat_new();

	// Load the CNF problem file
	char* nom_fic = (char*) malloc(32);
	strcpy(nom_fic, p_argv[1]);
	cnf_load_problem(nom_fic, *formula);
	cout << "c Solution to cnf file " << nom_fic << endl;
	free(nom_fic);
	sat_log(*formula);

	Interpretation* interpretation = alg_solve(*formula);
	interpretation->print();

	delete interpretation;
	sat_free(formula);
	formula = NULL;

	// Clean the logging system
	if (log4c_fini())
		cerr << "log4c cleanup failed." << endl;

	return 0;
}
