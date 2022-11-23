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
#include <algorithm>
#include <list>
#include <ranges>
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
	cout << "                   146" << endl;
	cout << "                   183" << endl;
	cout << "                   215" << endl;
	cout << "                   262" << endl;
	cout << "                   343" << endl;
	cout << "                   355" << endl;
	cout << "                   386" << endl;
	cout << "                   391" << endl;
	cout << "                   448" << endl;
	cout << "                   461" << endl;
	cout << "                   477" << endl;
	cout << "                   512" << endl;
	cout << "                   557" << endl;
	cout << "                   598" << endl;
	cout << "                   637" << endl;
	cout << "                   669" << endl;
	cout << "                   717" << endl;
	cout << "                   725" << endl;
	cout << "                   752" << endl;
	cout << "                   818" << endl;
	cout << "                   895" << endl;
	cout << "                   923" << endl;
	cout << "                   932" << endl;
	cout << "                   941" << endl;
	cout << "                   965" << endl;
	cout << "                   994" << endl;
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
		else {
			auto literals = valuation.getLiterals();
			// Only retain positive literals from the valuation
			auto solution = literals | std::ranges::views::filter([](sat::Literal l) {
				return l.isPositive();
			});
			// Convert to vector so we can sort the result...
			auto sortedSolution = std::vector(std::ranges::begin(solution), std::ranges::end(solution));
			std::ranges::sort(sortedSolution);

			// Show the solution
			cout << "Solution trouvée :" << endl;
			for (unsigned int line=0; line<9; line++) {
				cout << "  | ";
				for (unsigned int column=0; column<9; column++)
					cout << sortedSolution.at(line*9 + column).id() - (line+1)*100 - (column+1)*10 << " ";
				cout << "|" << endl;
			}
		}
		cout << stats  << endl;
		cout << chrono << endl;
	}

	// Clean the logging system
	if (log_cleanup())
		cerr << "Log cleanup failed." << endl;

	return EXIT_SUCCESS;
}
