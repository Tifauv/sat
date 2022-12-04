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
#include "SudokuLoader.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <cmath>
#include "utils.h"
#include "log.h"


namespace sudoku {

// METHODS
/**
 * Loads a Sudoku problem from a CNF file.
 *
 * @param p_filename
 *            the name of the file to load
 * @param p_formula
 *            the formula to initialize
 */
void SudokuLoader::loadProblem(char* p_filename, sat::Formula& p_formula) {
	log_debug(log_sudoku, "Loading sudoku problem from file '%s'...", p_filename);
	m_clauseId = 1;

	generateSudokuConstraints(p_formula);

	// Ouverture du fichier
	std::ifstream file(p_filename);
	if (!file.is_open()) {
		log_error(log_sudoku, "Could not open file '%s'.", p_filename);
		return;
	}
	log_debug(log_sudoku, "File '%s' opened.", p_filename);

	std::string line;
	while (getline(file, line)) {
		auto literal = parseInt(line);
		if (literal.has_value())
			p_formula.newClause(m_clauseId++)
			        .withPositiveLiteral(literal.value())
			        .build();
	}
	log_info(log_sudoku, "Sudoku problem loaded from file '%s'.", p_filename);
}



/**
 * Loads the standard 9x9 Sudoku constraints.
 *
 * @param p_formula
 *            the formula to initialize
 */
void SudokuLoader::generateSudokuConstraints(sat::Formula& p_formula) {
	log_debug(log_sudoku, "Generating sudoku constraints...");

	// General line / column / square constraints
	for (unsigned  value=1; value<=SIZE; value++) {
		// Generate line and column constraints
		for (unsigned line=1; line<=SIZE; line++) {
			generateLineConstraints(p_formula, value, line);
			generateColumnConstraints(p_formula, value, line);
		}
		for (unsigned squareLine=1; squareLine<=SQUARE_SIZE; squareLine++)
			for (unsigned squareColumn=1; squareColumn<=SQUARE_SIZE; squareColumn++)
				generateSquareConstraints(p_formula, value, squareLine, squareColumn);
	}

	// Allowed values
	generateValuesPerCell(p_formula);

	// Disallow two values for the same cell
	generateUniqueValuePerCell(p_formula);
}


void SudokuLoader::generateLineConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_line) {
	log_debug(log_sudoku, "  - line constraints for value %d line %d", p_value, p_line);
	for (unsigned startCol=1; startCol<=SIZE; startCol++)
		for (unsigned targetCol=startCol+1; targetCol<=SIZE; targetCol++)
			p_formula.newClause(m_clauseId++)
				.withNegativeLiteral(p_line*100 +  startCol*10 + p_value)
				.withNegativeLiteral(p_line*100 + targetCol*10 + p_value)
				.build();
}


void SudokuLoader::generateColumnConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_column) {
	log_debug(log_sudoku, "  - column constraints for value %d column %d", p_value, p_column);
	for (unsigned startLine=1; startLine<=SIZE; startLine++)
		for (unsigned targetLine=startLine+1; targetLine<=SIZE; targetLine++)
			p_formula.newClause(m_clauseId++)
				.withNegativeLiteral( startLine*100 + p_column*10 + p_value)
				.withNegativeLiteral(targetLine*100 + p_column*10 + p_value)
				.build();
}


void SudokuLoader::generateSquareConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_squareLine, unsigned int p_squareColumn) {
	auto squareLineOffset   = SQUARE_SIZE * (p_squareLine   - 1);
	auto squareColumnOffset = SQUARE_SIZE * (p_squareColumn - 1);

	for (unsigned lineInSquare=1; lineInSquare<=SQUARE_SIZE; lineInSquare++) {
		for (unsigned columnInSquare=1; columnInSquare<=SQUARE_SIZE; columnInSquare++) {
			for (unsigned targetLineInSquare=1; targetLineInSquare<=SQUARE_SIZE; targetLineInSquare++) {
				if (lineInSquare == targetLineInSquare)
					continue;

				for (unsigned targetColumnInSquare=1; targetColumnInSquare<=SQUARE_SIZE; targetColumnInSquare++) {
					if (columnInSquare == targetColumnInSquare)
						continue;

					auto valueLine  =       lineInSquare + squareLineOffset;
					auto targetLine = targetLineInSquare + squareLineOffset;
					auto valueColumn  =       columnInSquare + squareColumnOffset;
					auto targetColumn = targetColumnInSquare + squareColumnOffset;

					p_formula.newClause(m_clauseId++)
						.withNegativeLiteral( valueLine*100 +  valueColumn*10 + p_value)
						.withNegativeLiteral(targetLine*100 + targetColumn*10 + p_value)
						.build();
				}
			}
		}
	}
}


void SudokuLoader::generateValuesPerCell(sat::Formula& p_formula) {
	for (unsigned line=1; line<=SIZE; line++)
		for (unsigned column=1; column<=SIZE; column++) {
			auto clause = p_formula.newClause(m_clauseId++);
			for (unsigned value=1; value<=SIZE; value++)
				clause.withPositiveLiteral(line*100 + column*10 + value);
			clause.build();
		}
}


void SudokuLoader::generateUniqueValuePerCell(sat::Formula& p_formula) {
	for (unsigned value=1; value<=SIZE; value++)
		for (unsigned targetValue=value+1; targetValue<=SIZE; targetValue++)
			for (unsigned line=1; line<=SIZE; line++)
				for (unsigned column=1; column<=SIZE; column++)
					p_formula.newClause(m_clauseId++)
						.withNegativeLiteral(line*100 + column*10 +       value)
						.withNegativeLiteral(line*100 + column*10 + targetValue)
						.build();
}


std::optional<int> SudokuLoader::parseInt(std::string p_line) {
	std::istringstream source(p_line);
	int token;
	if (source >> token) {
		log_debug(log_sudoku, "  - Literal %sx%u parsed.", (token < 0 ? "¬" : ""), abs(token));
		return token;
	}
	return {};
}

} // namespace sudoku
