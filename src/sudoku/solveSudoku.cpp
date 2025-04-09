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
	std::cout << "Usage: " << p_command << " <grid_file>" << std::endl;
	std::cout << "    <grid_file>  A sudoku grid file is a text file that lists all the known values." << std::endl;
	std::cout << "                 Each value is on its own line, formated as a triplet <line, column, value>." << std::endl;
	std::cout << "                 For example, the following grid :" << std::endl;
	std::cout << "                   | 1     6       3   |" << std::endl;
	std::cout << "                   | 5         2       |" << std::endl;
	std::cout << "                   |       3 5     6 1 |" << std::endl;
	std::cout << "                   |       8   1 7     |" << std::endl;
	std::cout << "                   | 2       7       8 |" << std::endl;
	std::cout << "                   |     7     9       |" << std::endl;
	std::cout << "                   | 7 5     2         |" << std::endl;
	std::cout << "                   | 8               5 |" << std::endl;
	std::cout << "                   |   3 2 1   5     4 |" << std::endl;
	std::cout << "                 must be entered like the following file (order is not important):" << std::endl;
	std::cout << "                   111" << std::endl;
	std::cout << "                   146" << std::endl;
	std::cout << "                   183" << std::endl;
	std::cout << "                   215" << std::endl;
	std::cout << "                   262" << std::endl;
	std::cout << "                   343" << std::endl;
	std::cout << "                   355" << std::endl;
	std::cout << "                   386" << std::endl;
	std::cout << "                   391" << std::endl;
	std::cout << "                   448" << std::endl;
	std::cout << "                   461" << std::endl;
	std::cout << "                   477" << std::endl;
	std::cout << "                   512" << std::endl;
	std::cout << "                   557" << std::endl;
	std::cout << "                   598" << std::endl;
	std::cout << "                   637" << std::endl;
	std::cout << "                   669" << std::endl;
	std::cout << "                   717" << std::endl;
	std::cout << "                   725" << std::endl;
	std::cout << "                   752" << std::endl;
	std::cout << "                   818" << std::endl;
	std::cout << "                   895" << std::endl;
	std::cout << "                   923" << std::endl;
	std::cout << "                   932" << std::endl;
	std::cout << "                   941" << std::endl;
	std::cout << "                   965" << std::endl;
	std::cout << "                   994" << std::endl;
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
		std::cerr << "Log initialization failed, aborting." << std::endl;
		exit(EXIT_LOG_FAILURE);
	}

	{
		// File to load
		auto gridFilename = p_argv[1];

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
		auto valuation = solver.solve();

		/* Output the solution */
		if (valuation.isUnsatisfiable()) {
			std::cout << "Il n'y a pas de solution à cette grille" << std::endl;
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
			std::cout << "Solution trouvée :" << std::endl;
			for (unsigned int line=0; line<9; line++) {
				std::cout << "  | ";
				for (unsigned int column=0; column<9; column++)
					std::cout << sortedSolution.at(line*9 + column).id() - (line+1)*100 - (column+1)*10 << " ";
				std::cout << "|" << std::endl;
			}
		}
		std::cout << stats  << std::endl;
		std::cout << chrono << std::endl;
	}

	// Clean the logging system
	if (log_cleanup())
		std::cerr << "Log cleanup failed." << std::endl;

	return EXIT_SUCCESS;
}
