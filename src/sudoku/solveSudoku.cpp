/*  Copyright 2022 Olivier Serve
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
#include "SudokuLoader.h"
#include "IterativeDpllSolver.h"
#include "Valuation.h"
#include "VariablePolarityLiteralSelector.h"
#include "MostUsedVariableSelector.h"
#include "MostUsedPolaritySelector.h"
#include "PolarityCachingSelector.h"
#include "StatisticsListener.h"
#include "ChronoListener.h"

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
	cout << "Usage: " << p_command << " <grid_file>" << endl;
	cout << "    <grid_file>  A sudoku grid file is a text file that lists all the known values." << endl;
	cout << "                 Each value is on its own line, formated as a triplet <value, line, column>." << endl;
	cout << "                 For example, the following grid :" << endl;
	cout << "                   | 1     6       3   |" << endl;
	cout << "                   | 5         2       |" << endl;
	cout << "                   |       3 5     6 1 |" << endl;
	cout << "                   |       8   1 7     |" << endl;
	cout << "                   | 2       7       8 |" << endl;
	cout << "                   |     7     9       |" << endl;
	cout << "                   | 7 5     2         |" << endl;
	cout << "                   | 8               5 |" << endl;
	cout << "                   |   3 2 1   5     4 |" << endl;
	cout << "                 must be entered like the following file (order is not important):" << endl;
	cout << "                   111" << endl;
	cout << "                   614" << endl;
	cout << "                   318" << endl;
	cout << "                   521" << endl;
	cout << "                   226" << endl;
	cout << "                   334" << endl;
	cout << "                   535" << endl;
	cout << "                   638" << endl;
	cout << "                   139" << endl;
	cout << "                   844" << endl;
	cout << "                   146" << endl;
	cout << "                   747" << endl;
	cout << "                   251" << endl;
	cout << "                   755" << endl;
	cout << "                   859" << endl;
	cout << "                   763" << endl;
	cout << "                   966" << endl;
	cout << "                   771" << endl;
	cout << "                   572" << endl;
	cout << "                   275" << endl;
	cout << "                   881" << endl;
	cout << "                   589" << endl;
	cout << "                   392" << endl;
	cout << "                   293" << endl;
	cout << "                   194" << endl;
	cout << "                   596" << endl;
	cout << "                   499" << endl;
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
		char* gridFilename = p_argv[1];

		// Load the formula
		sat::Formula formula;
		sudoku::SudokuLoader loader;
		loader.loadProblem(gridFilename, formula);
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
		sat::solver::listeners::StatisticsListener stats;
		sat::solver::listeners::ChronoListener chrono;
		solver.addListener(stats);
		solver.addListener(chrono);
		//solver.addListener(cachingPolaritySelector);

		/* Solve the problem */
		sat::solver::Valuation& valuation = solver.solve();

		/* Output the solution */
		if (valuation.isUnsatisfiable()) {
			cout << "Il n'y a pas de solution à cette grille" << endl;
		}


		cout << "c Solution to sudoku problem " << gridFilename << endl;
		cout << "c " << stats  << endl;
		cout << "c " << chrono << endl;
		cout << valuation;
	}

	// Clean the logging system
	if (log_cleanup())
		cerr << "Log cleanup failed." << endl;

	return EXIT_SUCCESS;
}
