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

#ifndef DAVIS_PUTNAM_H
#define DAVIS_PUTNAM_H

#include "literal.h"
#include "sat.h"
#include "interpretation.h"
#include "history.h"

/**
 * Main loop of the Davis-Putnam algorithm.
 * 
 * @param p_formula
 *            the SAT formula to solve
 * @param p_intr
 *            the current interpretation
 */
void dp_main(tGraphe* p_formula, Interpretation* p_intr); 


/**
 * Selects a literal to be used for the reduction phase.
 * 
 * @param p_formula
 *            the SAT formula
 * 
 * @return the literal
 */
Literal dp_select_literal(tGraphe* p_formula);


/**
 * Reduces all the graph's formulas using a literal.
 * The history is used for backtracking.
 * 
 * @param p_formula
 *            the SAT formula
 * @param p_literal
 *            the literal used to reduce the formula
 * @param p_history
 *            the backtracking history
 * 
 * @return -1 if p_formula is NULL,
 *          0 if the reduction was done to the end,
 *          1 if an unsatisfiable clause was produced
 */
int dp_reduce(tGraphe* p_formula, Literal p_literal, History& p_history);

#endif // DAVIS_PUTNAM_H
