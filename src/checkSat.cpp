/*  Copyright 2015 Olivier Serve
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
#include <iostream>
#include <vector>
#include <log4c.h>

#include "CnfLoader.h"
#include "BasicSolutionChecker.h"



/**
 * Displays the usage message.
 * 
 * @param p_command
 *            the command name as given to argv[0]
 */
void usage(char* p_command) {
	std::cout << "Usage: " << p_command << " <cnf_file> <sat_file>" << std::endl;
	std::cout << "    <cnf_file>  a CNF problem" << std::endl;
	std::cout << "    <sat_file>  a CNF solution" << std::endl;
}


/**
 * Main function.
 * 
 * @param p_argc
 *            the count of arguments in p_argv
 * @param p_argv
 *            the array of command-line arguments
 * 
 * @return -2 if the log4c initialization fails,
 *         -1 if no argument was given,
 *          0 otherwise
 */
int main(int p_argc, char* p_argv[]) {
	// No argument: display usage and exit
	if (p_argc != 3) {
		usage(p_argv[0]);
		exit(-1);
	}

	// Initialize the logging system
	if (log4c_init()) {
		std::cout << "Log4c initialization failed, aborting." << std::endl;
		exit(-2);
	}

	int rc = 0;
	{
		// Files to load
		char* cnfFilename = p_argv[1];
		char* satFilename = p_argv[2];
		
		// Load the formula
		Formula formula;
		CnfLoader loader;
		loader.loadProblem(cnfFilename, formula);
		formula.log();

		// Load the SAT solution file
		std::vector<RawLiteral>* solution = loader.loadSolution(satFilename);

		// Check the solution
		BasicSolutionChecker checker(formula);
		if (checker.checkSolution(solution)) {
			rc = 0;
			std::cout << "The solution is valid." << std::endl;
		}
		else {
			rc = 1;
			std::cout << "The solution is not valid." << std::endl;
		}

		delete solution;
	}

	// Clean the logging system
	if (log4c_fini())
		std::cerr << "log4c cleanup failed." << std::endl;

	return rc;
}

