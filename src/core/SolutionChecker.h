/*  Copyright (c) 2015 Olivier Serve
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
#ifndef SOLUTIONCHECKER_H
#define SOLUTIONCHECKER_H

#include <list>
class RawLiteral;


/**
 * Interface for SAT solution checkers.
 */
class SolutionChecker {
public:
	/**
	 * Check whether a given solution is a valid interpretation of a formula.
	 * 
	 * @param p_solution
	 *            the solution to check
	 * 
	 * @return true if the solution satisfies the formula,
	 *         false otherwise
	 */
	bool checkSolution(std::list<RawLiteral>* p_solution);
};

#endif // SOLUTIONCHECKER_H
