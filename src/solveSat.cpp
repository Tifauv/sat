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
#include <iostream>
#include <log4c.h>

#include "CnfLoader.h"
#include "DpllSolver.h"
#include "Interpretation.h"
#include "VariablePolarityLiteralSelector.h"
#include "FirstVariableSelector.h"
#include "PositiveFirstPolaritySelector.h"

using namespace std;


/**
 * Displays the usage message.
 * 
 * @param p_command
 *            the command name as given to argv[0]
 */
void usage(char* p_command) {
	cout << "Usage: " << p_command << " <cnf_file>" << endl;
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
 * @return -2 if the log4c initialization fails,
 *         -1 if no argument was given,
 *          0 otherwise
 */
int main(int p_argc, char* p_argv[]) {
	// No argument: display usage and exit
	if (p_argc != 2) {
		usage(p_argv[0]);
		exit(-1);
	}

	// Initialize the logging system
	if (log4c_init()) {
		cerr << "Log4c initialization failed, aborting." << endl;
		exit(-2);
	}
	
	{
		// File to load
		char* cnfFilename = p_argv[1];

		// Load the formula
		Formula formula;
		CnfLoader loader;
		loader.loadProblem(cnfFilename, formula);
		cout << "c Solution to cnf file " << cnfFilename << endl;
		formula.log();

		// Build the solver
		FirstVariableSelector variableSelector;
		PositiveFirstPolaritySelector polaritySelector;
		VariablePolarityLiteralSelector literalSelector(variableSelector, polaritySelector);

		// Solve the problem and display its interpretation
		DpllSolver solver(literalSelector);
		Interpretation* interpretation = solver.solve(formula);
		interpretation->print();
		delete interpretation;
	}

	// Clean the logging system
	if (log4c_fini())
		cerr << "log4c cleanup failed." << endl;

	return 0;
}
