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

#include "cnf.h"

using namespace std;

/**
 * Displays the usage message.
 */
void usage() {
	cout << "Usage: checkSat <cnf_file> <sat_file>" << endl;
	cout << "    <cnf_file>  a CNF problem" << endl;
	cout << "    <sat_file>  a CNF solution" << endl;
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
	if (p_argc != 3) {
		usage();
		exit(-1);
	}

	// Initialize the logging system
	if (log4c_init()) {
		cout << "Log4c initialization failed, aborting." << endl;
		exit(-2);
	}

	// Create the formula
	tGraphe* formula = sat_new();

	// Load the CNF problem file
	char* cnfFilename = (char*) malloc(32);
	strcpy(cnfFilename, p_argv[1]);
	cnf_load_problem(cnfFilename, *formula);
	free(cnfFilename);
	sat_log(*formula);

	// Load the SAT solution file
	char* satFilename = (char*) malloc(32);
	strcpy(satFilename, p_argv[2]);
	std::list<Literal>* solution = cnf_load_solution(satFilename);
	free(satFilename);

	// Check the solution
	int rc = 0;
	if (cnf_check_solution(*formula, solution)) {
		rc = 0;
		cout << "The solution is valid." << endl;
	}
	else {
		rc = 1;
		cout << "The solution is not valid." << endl;
	}

	delete solution;
	sat_free(formula);
	formula = NULL;

	// Clean the logging system
	if (log4c_fini())
		cerr << "log4c cleanup failed." << endl;

	return rc;
}

