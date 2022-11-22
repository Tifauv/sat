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
#ifndef SUDOKU_LOADER_H
#define SUDOKU_LOADER_H

#include <memory>
#include <vector>
#include "Formula.h"

using namespace std;

namespace sudoku {

/**
 * Loads a Sudoku solving formula.
 *
 * This class is NOT thread-safe because of the m_clauseId state it maintains.
 * It can however be used multiple times because the m_clauseId field is reset
 * at each {@link #loadSudoku} invocation.
 */
class SudokuLoader {
public:
	/**
	* Loads a Sudoku problem from a CNF file.
	*
	* @param p_filename
	*            the name of the file to load
	* @param p_formula
	*            the formula to initialize
	*/
	void loadProblem(char* p_filename, sat::Formula& p_formula);


protected:
	/**
	 * Loads the standard 9x9 Sudoku constraints:
	 * - one value per line;
	 * - one value per column;
	 * - one value per square;
	 *
	 * @param p_formula
	 *            the formula to initialize
	 */
	void generateSudokuConstraints(sat::Formula& p_formula);

	/**
	 * Generate clauses that enforce the "one value per line" constraint.
	 */
	void generateLineConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_line);

	/**
	 * Generate clauses that enforce the "one value per column" constraint.
	 */
	void generateColumnConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_column);

	/**
	 * Generate clauses that enforce the "one value per square" constraint.
	 */
	void generateSquareConstraints(sat::Formula& p_formula, unsigned int p_value, unsigned int p_squareLine, unsigned int p_squareColumn);

	/**
	 * Generate the allowed values for each cell.
	 */
	void generateValuesPerCell(sat::Formula& p_formula);

	/**
	 * Generate clauses that enforce the "unique value per cell" constraint.
	 */
	void generateUniqueValuePerCell(sat::Formula& p_formula);


	/**
	 * Parses a clause line from a cnf file as a list of raw literals.
	 *
	 * @param p_line
	 *            the line to parse
	 *
	 * @return the list of raw literals
	 */
	unique_ptr<vector<sat::RawLiteral>> parseClause(string p_line);


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
	int existsLiteral(sat::RawLiteral& p_literal, vector<sat::RawLiteral>& p_literals);


private:
	const unsigned int SIZE = 9;
	const unsigned int SQUARES = 3;
	const unsigned int SQUARE_SIZE = SIZE / 3;

	unsigned int m_clauseId = 1;
};

} // namespace sudoku

#endif // SUDOKU_LOADER_H
