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
#include <log4c.h>
#include <chrono>

#include "log.h"
#include "CnfLoader.h"
#include "HistoryBasedDpllSolver.h"
#include "IterativeDpllSolver.h"
#include "Valuation.h"
#include "VariablePolarityLiteralSelector.h"
#include "MostUsedVariableSelector.h"
#include "MostUsedPolaritySelector.h"
#include "BacktrackCounterListener.h"
#include "LoggingListener.h"

using Clock = std::chrono::high_resolution_clock;


/**
 * Displays the usage message.
 * 
 * @param p_command
 *            the command name as given to argv[0]
 */
void usage(char* p_command) {
	std::cout << "Usage: " << p_command << " <cnf_file>" << std::endl;
	std::cout << "    <cnf_file>  a CNF problem" << std::endl;
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
		std::cerr << "Log4c initialization failed, aborting." << std::endl;
		exit(-2);
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
		
		/* Build the listeners */
		sat::solver::listeners::BacktrackCounterListener backtrackListener;
		sat::solver::listeners::LoggingListener loggingListener;
		
		/* Build the solver */
		//sat::solver::HistoryBasedDpllSolver solver(formula, literalSelector);
		sat::solver::IterativeDpllSolver solver(formula, literalSelector);
		solver.addListener(backtrackListener);
		solver.addListener(loggingListener);
		
		/* Solve the problem */
		auto start = Clock::now();
		sat::solver::Valuation& valuation = solver.solve();
		auto end = Clock::now();
		
		/* Output the solution */
		std::cout << "c Solution to cnf file " << cnfFilename << std::endl;
		std::cout << "c Backtracked " << backtrackListener.counter() << " times" << std::endl;
		std::cout << "c Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
		valuation.print();
	}

	// Clean the logging system
	if (log4c_fini())
		std::cerr << "log4c cleanup failed." << std::endl;

	return 0;
}
