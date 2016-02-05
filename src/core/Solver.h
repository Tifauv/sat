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
#ifndef SOLVER_H
#define SOLVER_H

#include "Interpretation.h"


/**
 * Interface for SAT solvers.
 */
class Solver {
public:
	/**
	 * Getter for the internal interpretation.
	 * 
	 * @return the current interpretation at the moment of the call
	 */
	virtual const Interpretation& getInterpretation() const = 0;


	/**
	 * Common entrypoint for SAT solvers.
	 * 
	 * @return an interpretation (satisfiable or not)
	 */
	virtual Interpretation& solve() = 0;
};

#endif // SOLVER_H
