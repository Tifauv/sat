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
	ifstream file(p_filename);
	if (!file.is_open()) {
		log_error(log_sudoku, "Could not open file '%s'.", p_filename);
		return;
	}
	log_debug(log_sudoku, "File '%s' opened.", p_filename);

	string line;
	while (getline(file, line)) {
		// Transformation string -> tab
		p_formula.createClause(m_clauseId++, parseClause(line));
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
	for (unsigned int value=1; value<=SIZE; value++) {
		// Generate line and column constraints
		for (unsigned int line=1; line<=SIZE; line++) {
			generateLineConstraints(p_formula, value, line);
			generateColumnConstraints(p_formula, value, line);
		}
		for (unsigned int squareLine=1; squareLine<=SQUARE_SIZE; squareLine++)
			for (unsigned int squareColumn=1; squareColumn<=SQUARE_SIZE; squareColumn++)
				generateSquareConstraints(p_formula, value, squareLine, squareColumn);
	}

	// Allowed values
	generateValuesPerCell(p_formula);

	// Disallow two values for the same cell
	generateUniqueValuePerCell(p_formula);
}


void SudokuLoader::generateLineConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_line) {
	log_debug(log_sudoku, "  - line constraints for value %d line %d", p_value, p_line);
	for (unsigned int startCol=1; startCol<=SIZE; startCol++)
		for (unsigned int targetCol=startCol+1; targetCol<=SIZE; targetCol++)
			p_formula.newClause(m_clauseId++)
				.withNegativeLiteral(p_line*100 +  startCol*10 + p_value)
				.withNegativeLiteral(p_line*100 + targetCol*10 + p_value)
				.build();
}


void SudokuLoader::generateColumnConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_column) {
	log_debug(log_sudoku, "  - column constraints for value %d column %d", p_value, p_column);
	for (unsigned int startLine=1; startLine<=SIZE; startLine++)
		for (unsigned int targetLine=startLine+1; targetLine<=SIZE; targetLine++)
			p_formula.newClause(m_clauseId++)
				.withNegativeLiteral( startLine*100 + p_column*10 + p_value)
				.withNegativeLiteral(targetLine*100 + p_column*10 + p_value)
				.build();
}


void SudokuLoader::generateSquareConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_squareLine, unsigned int p_squareColumn) {
	unsigned int squareLineOffset   = SQUARE_SIZE * (p_squareLine   - 1);
	unsigned int squareColumnOffset = SQUARE_SIZE * (p_squareColumn - 1);

	for (unsigned int lineInSquare=1; lineInSquare<=SQUARE_SIZE; lineInSquare++) {
		for (unsigned int columnInSquare=1; columnInSquare<=SQUARE_SIZE; columnInSquare++) {
			for (unsigned int targetLineInSquare=1; targetLineInSquare<=SQUARE_SIZE; targetLineInSquare++) {
				if (lineInSquare == targetLineInSquare)
					continue;

				for (unsigned int targetColumnInSquare=1; targetColumnInSquare<=SQUARE_SIZE; targetColumnInSquare++) {
					if (columnInSquare == targetColumnInSquare)
						continue;

					unsigned int valueLine  =       lineInSquare + squareLineOffset;
					unsigned int targetLine = targetLineInSquare + squareLineOffset;
					unsigned int valueColumn  =       columnInSquare + squareColumnOffset;
					unsigned int targetColumn = targetColumnInSquare + squareColumnOffset;

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
	for (unsigned int line=1; line<=SIZE; line++)
		for (unsigned int column=1; column<=SIZE; column++) {
			auto clause = unique_ptr<vector<sat::RawLiteral>>(new vector<sat::RawLiteral>());
			for (unsigned int value=1; value<=SIZE; value++)
				// Add literal (line, column,  value)
				clause->push_back(sat::RawLiteral(line*100 + column*10 + value));
			p_formula.createClause(m_clauseId++, clause);
		}
}


void SudokuLoader::generateUniqueValuePerCell(sat::Formula& p_formula) {
	for (unsigned int value=1; value<=SIZE; value++)
		for (unsigned int targetValue=value+1; targetValue<=SIZE; targetValue++)
			for (unsigned int line=1; line<=SIZE; line++)
				for (unsigned int column=1; column<=SIZE; column++)
					p_formula.newClause(m_clauseId++)
						.withNegativeLiteral(line*100 + column*10 +       value)
						.withNegativeLiteral(line*100 + column*10 + targetValue)
						.build();
}

/**
 * Parses a clause line from a cnf file.
 *
 */
unique_ptr<vector<sat::RawLiteral>> SudokuLoader::parseClause(string p_line) {
	// I/ Création du tableau
	auto literals = unique_ptr<vector<sat::RawLiteral>>(new vector<sat::RawLiteral>());

	// III/ Initialisation du tableau
	istringstream source(p_line);
	int token;
	while ((source >> token) && (notNull(literals))) {
		// If the '0' token is found, this is the end of the clause.
		// NOTE this is not conformant to the CNF format specification because some other
		// clause might follow. But current test files do not use that feature.
		if (token == 0)
			break;

		// Parse current token
		sat::RawLiteral literal(token);

		// On teste si l'entier n'apparaît pas déjà dans la variable
		switch (existsLiteral(literal, *literals)) {
			case 1: // Le token apparaît 2 fois avec le même "signe" -> pas ajouté cette fois
				log_debug(log_sudoku, "  - Literal %sx%u already parsed in that clause, skipped.", (literal.isNegative() ? "¬" : ""), literal.id());
				break;

			case -1: // Le token et son contraire apparaîssent -> literals = nullptr
				log_debug(log_sudoku, "   - Literal %sx%u already parsed in that clause so it is always true.", (literal.isNegative() ? "¬" : ""), literal.id());
				literals = nullptr;
				break;

			default:
				literals->push_back(literal);
				log_debug(log_sudoku, "  - Literal %sx%u parsed.", (literal.isNegative() ? "¬" : ""), literal.id());
		}
	}

	return literals;
}


/**
 * Searches a literal in a list of literals.
 *
 * @param p_literal
 *            the literal to search
 * @param p_literals
 *            the list of literals
 *
 * @return -1 if -p_literal appears in p_literals
 *          0 if p_literal does not appear in p_literals
 *          1 if p_literal appears in p_literals
 */
int SudokuLoader::existsLiteral(sat::RawLiteral& p_literal, vector<sat::RawLiteral>& p_literals) {
	auto foundLiteral = find_if(p_literals.begin(), p_literals.end(), [p_literal](sat::RawLiteral x){return x.id() == p_literal.id();});

	// Literal found
	if (foundLiteral != p_literals.end()) {
		return p_literal.sign() * (*foundLiteral).sign();
	}

	// Not found
	return 0;
}

} // namespace sudoku
