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

#include "solver.h"
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
 * 
 * @return the interpretation found,
 *         of NULL if p_formula or p_intr is NULL
 */
tIntr* dp_main(tGraphe** p_formula, tIntr* p_intr); 


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @param p_formula
 *            the SAT formula
 * 
 * @return the literal
 */
int dp_choose_literal(tGraphe* p_formula);


/**
 * Renvoie l'interprétation du graphe
 * 
 * @param p_formula
 *            the SAT formula
 * @param p_literal
 *            the reduction literal
 * @param p_interpretation
 *            the current interpretation
 * 
 * @return 
 */
tIntr* dp_test_sat(tGraphe** p_formula, Literal p_literal, tIntr* p_interpretation);


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
 */
void dp_reduce(tGraphe** p_formula, Literal p_literal, History* p_history);


/**
 * Reduces the clause of a formula using a literal.
 * The history is used for backtracking.
 * 
 * @param p_clause
 *            the clause to reduce
 * @param p_literal
 *            the literal used to reduce the clause
 * @param p_formula
 *            the formula containing the clause
 * @param p_history
 *            the backtracking history
 * 
 * @return  3 if an empty clause has been produced,
 *          2 if the clause was modified but is not empty,
 *          1 if the clause was removed from the formula,
 *          0 nothing happened,
 *         -1 if the given clause is NULL,
 */
int dp_reduce_clause(tClause* p_clause, Literal p_literal, tGraphe* p_formula, History* p_history);

#endif // DAVIS_PUTNAM_H
