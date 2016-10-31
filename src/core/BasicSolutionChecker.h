/*  Copyright 2016 Olivier Serve
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
#ifndef BASIC_SOLUTION_CHECKER_H
#define BASIC_SOLUTION_CHECKER_H

#include "SolutionChecker.h"
#include "RawLiteral.h"

using namespace std;

namespace sat {

class Formula;

namespace checker {


/**
 * Interface for SAT solution checkers.
 */
class BasicSolutionChecker : public SolutionChecker {
public:
	/**
	 * Constructor.
	 * 
	 * @param p_formula
	 *            the formula
	 */
	explicit BasicSolutionChecker(Formula& p_formula);


	/**
	 * Check whether a given solution is a valid valuation of a formula.
	 * 
	 * @param p_solution
	 *            the solution to check
	 * 
	 * @return true if the solution satisfies the formula,
	 *         false otherwise
	 */
	bool checkSolution(vector<RawLiteral>& p_solution) override;


protected:
	/**
	 * Overloaded implementation that takes a RawLiteral instead of a Literal.
	 * 
	 * @param p_rawLiteral
	 *            the raw literal used to reduce the formula
	 * 
	 * @return true if the reduction is satisfiable
	 *         false if it is unsatisfiable
	 */
	bool reduce(const RawLiteral& p_rawLiteral);


private:
	Formula& m_formula;
};

} // namespace sat::checker
} // namespace sat

#endif // BASICSOLUTIONCHECKER_H
