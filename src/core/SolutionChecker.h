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
#ifndef SOLUTION_CHECKER_H
#define SOLUTION_CHECKER_H

#include <vector>

using namespace std;


namespace sat {

class RawLiteral;

namespace checker {


/**
 * Interface for SAT solution checkers.
 */
class SolutionChecker {
public:
	/**
	 * Check whether a given solution is a valid valuation of a formula.
	 * 
	 * @param p_solution
	 *            the solution to check
	 * 
	 * @return true if the solution satisfies the formula,
	 *         false otherwise
	 */
	virtual bool checkSolution(vector<RawLiteral>& p_solution) = 0;
};

} // namespace sat::checker
} // namespace sat

#endif // SOLUTIONCHECKER_H
