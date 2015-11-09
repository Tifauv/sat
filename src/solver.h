/* Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki
   Copyright (C) 2015 Olivier Serve

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
*/

#ifndef SOLVER_H
#define SOLVER_H

#include "sat.h"
#include "interpretation.h"

/**
 * Common entrypoint for SAT solvers.
 * 
 * @param p_graph
 *            the SAT graph to solve
 * 
 * @return NULL if p_graph is NULL,
 *         or an interpretation (satisfiable or not)
 */
tIntr *alg_solve(tGraphe **p_graph);

#endif // SOLVER_H
