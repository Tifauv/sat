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

#include "log.h"
#include "CnfLoader.h"
#include "IterativeDpllSolver.h"
#include "Valuation.h"
#include "VariablePolarityLiteralSelector.h"
#include "MostUsedVariableSelector.h"
#include "MostUsedPolaritySelector.h"
#include "PolarityCachingSelector.h"
#include "StatisticsListener.h"
#include "ChronoListener.h"
#include "LoggingListener.h"

using namespace std;


/* Exit codes */
#define EXIT_SUCCESS      0
#define EXIT_LOG_FAILURE -1


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
 * @return -1 if the log initialization fails,
 *          0 otherwise
 */
int main(int p_argc, char* p_argv[]) {
	// No argument: display usage and exit
	if (p_argc != 2) {
		usage(p_argv[0]);
		exit(EXIT_SUCCESS);
	}

	// Initialize the logging system
	if (log_setup()) {
		cerr << "Log initialization failed, aborting." << endl;
		exit(EXIT_LOG_FAILURE);
	}
	
	{
		// File to load
		char* cnfFilename = p_argv[1];

		// Load the formula
		sat::Formula formula;
		sat::CnfLoader loader;
		loader.loadProblem(cnfFilename, formula);
		formula.log();

		/* Build the literal selection strategy */
		sat::solver::selectors::MostUsedVariableSelector variableSelector;
		sat::solver::selectors::MostUsedPolaritySelector polaritySelector;
		sat::solver::VariablePolarityLiteralSelector literalSelector(variableSelector, polaritySelector);
		//sat::solver::selectors::PolarityCachingSelector cachingPolaritySelector(polaritySelector);
		//sat::solver::VariablePolarityLiteralSelector literalSelector(variableSelector, cachingPolaritySelector);
		
		/* Build the solver */
		sat::solver::IterativeDpllSolver solver(formula, literalSelector);

		/* Build and add the listeners */
		//sat::solver::listeners::LoggingListener logging;
		sat::solver::listeners::StatisticsListener stats;
		sat::solver::listeners::ChronoListener chrono;
		//solver.addListener(logging);
		solver.addListener(stats);
		solver.addListener(chrono);
		//solver.addListener(cachingPolaritySelector);
		
		/* Solve the problem */
		const sat::solver::Valuation& valuation = solver.solve();
		
		/* Output the solution */
		cout << "c Solution to cnf file " << cnfFilename << endl;
		cout << "c " << stats  << endl;
		cout << "c " << chrono << endl;
		cout << valuation;
	}

	// Clean the logging system
	if (log_cleanup())
		cerr << "Log cleanup failed." << endl;

	return EXIT_SUCCESS;
}
