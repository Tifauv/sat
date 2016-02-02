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
#include <chrono>

#include "CnfLoader.h"
#include "HistoryBasedDpllSolver.h"
#include "Interpretation.h"
#include "VariablePolarityLiteralSelector.h"
#include "FirstVariableSelector.h"
#include "MostUsedVariableSelector.h"
#include "LeastUsedVariableSelector.h"
#include "PositiveFirstPolaritySelector.h"
#include "MostUsedPolaritySelector.h"
#include "LeastUsedPolaritySelector.h"
#include "BacktrackCounterListener.h"

using namespace std;
typedef chrono::high_resolution_clock Clock;


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
		formula.log();

		/* Build the literal selection strategy */
		//FirstVariableSelector variableSelector;
		MostUsedVariableSelector variableSelector;
		//LeastUsedVariableSelector variableSelector;
		//PositiveFirstPolaritySelector polaritySelector;
		MostUsedPolaritySelector polaritySelector;
		//LeastUsedPolaritySelector polaritySelector;
		VariablePolarityLiteralSelector literalSelector(variableSelector, polaritySelector);
		
		/* Build the listeners */
		BacktrackCounterListener backtrackListener;
		
		/* Build the solver */
		HistoryBasedDpllSolver solver(formula, literalSelector);
		solver.registerListener(backtrackListener);
		
		/* Solve the problem */
		auto start = Clock::now();
		Interpretation& interpretation = solver.solve();
		auto end = Clock::now();
		
		/* Output the solution */
		cout << "c Solution to cnf file " << cnfFilename << endl;
		cout << "c Backtracked " << backtrackListener.counter() << " times" << endl;
		cout << "c Took " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds" << endl;
		interpretation.print();
	}

	// Clean the logging system
	if (log4c_fini())
		cerr << "log4c cleanup failed." << endl;

	return 0;
}
