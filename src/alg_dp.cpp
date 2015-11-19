/* Fonctions de la libDP -- Algorithme de Davis-Putnam pour problèmes SAT
   Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki

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
#include "alg_dp.h"

#include <log4c.h>
#include "solver.h"
#include "utils.h"
#include "log.h"


/**
 * Starter function of the solver.
 * Implements "solver.h"
 * 
 * @param p_formula
 *            the SAT formula to solve
 * 
 * @return an interpretation (satisfiable or not),
 *         or NULL if p_formula is NULL
 */
Interpretation* alg_solve(tGraphe* p_formula) {
	// Parameters check
	if (isNull(p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The formula pointer is NULL.");
		return NULL;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Starting the DPLL algorithm.");
	Interpretation* interpretation = new Interpretation();
	dp_main(p_formula, interpretation);
	return interpretation;
}


/**
 * Main loop of the Davis-Putnam algorithm.
 * 
 * @param p_formula
 *            the SAT formula to solve
 * @param p_interpretation
 *            the current interpretation
 */
void dp_main(tGraphe* p_formula, Interpretation* p_interpretation) {
	/* Check parameters */
	if (isNull(p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The formula pointer is NULL.");
		return;
	}

	if (isNull(p_interpretation)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The interpretation pointer is NULL.");
		return;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Current state:");
	sat_see(p_formula);
	p_interpretation->log();

	/*
	 * Stop case: if there is no clause left, we are done.
	 */
	if (isNull((p_formula)->clauses)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "No more clauses.");
		return;
	}

	/*
	 * Choose the reduction literal.
	 * This is the crucial step, performance-wise.
	 */
	Literal chosen_literal = dp_select_literal(p_formula);

	/*
	 * First reduction with the chosen literal.
	 */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "First reduction attempt...");
	History history;
	int rc = dp_reduce(p_formula, chosen_literal, history);
	
	/*
	 * The interpretation is satisfiable: we are done.
	 */
	if (rc == 0) {
		// Add the chosen literal to the current interpretation
		p_interpretation->push(chosen_literal);
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Added %sx%u to the interpretation.", (chosen_literal < 0 ? "¬" : ""), sat_literal_id(chosen_literal));

		// Loop again
		dp_main(p_formula, p_interpretation);
		if (p_interpretation->isSatisfiable())
			return;
		else // Remove the current literal from the interpretation
			p_interpretation->pop();
	}

	/*
	 * The interpretation is not satisfiable: we try with the opposite literal.
	 */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "The current interpretation is unsatisfiable.");
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Rebuilding the formula before second attempt...");
	history.replay(p_formula);
	sat_see(p_formula);
	p_interpretation->setSatisfiable();
	p_interpretation->log();

	// Seconde réduction et test du résultat
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Second reduction attempt...");
	rc = dp_reduce(p_formula, -chosen_literal, history);

	/*
	 * The interpretation is satisfiable: we are done. We can return the current interpretation.
	 */
	if (rc == 0) {
		// Add the chosen literal to the current interpretation
		p_interpretation->push(-chosen_literal);
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Added %sx%u to the interpretation.", (chosen_literal > 0 ? "¬" : ""), sat_literal_id(chosen_literal));

		// Loop again
		dp_main(p_formula, p_interpretation);
		if (p_interpretation->isSatisfiable())
			return;
		else // Remove the current literal from the interpretation
			p_interpretation->pop();
	}
	/*
	 * The interpretation is also unsatisfiable with the opposite literal.
	 */
	else
		p_interpretation->setUnsatisfiable();

	/* Restoring state before backtracking. */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Restoring state before backtracking...");

	// Reconstruction du graphe
	history.replay(p_formula);

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Restored state:");
	sat_see(p_formula);
	p_interpretation->log();
}


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @param p_formula
 *            the SAT formula
 * 
 * @return the literal
 */
Literal dp_select_literal(tGraphe* p_formula) {
	// Search a literal from a unitary clause
	Literal chosen_literal = sat_find_literal_from_unary_clause(p_formula);
	if (chosen_literal != 0)
		return chosen_literal;

	// If there is no unitary clause
	return sat_get_first_literal(p_formula);
}


/**
 * Reduces all the graph's formulas using a literal.
 * The history is used for backtracking.
 * 
 * @param p_formula
 *            the SAT graph of formulas
 * @param p_literal
 *            the literal used to reduce the formulas
 * @param p_history
 *            the backtracking history
 *
 * @return -1 if p_formula is NULL,
 *          0 if the reduction was done to the end,
 *          1 if an unsatisfiable clause was produced
 */
int dp_reduce(tGraphe* p_formula, Literal p_literal, History& p_history) {
	if (isNull(p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The formula pointer is NULL.");
		return -1;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Reduction using literal %sx%u...", (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal));
	int rc = 0;

	// Retrieve the literal cell
	tVar* literalPtr = p_formula->vars;
	while (notNull(literalPtr) && literalPtr->indVar != sat_literal_id(p_literal))
		literalPtr = literalPtr->suiv;
	if (literalPtr == NULL)
		return 0;

	// Remove the clauses that contain the same sign as the given literal
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Removing clauses that contain the literal %sx%u...", (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal));
	tPtVarSgn* clausePtr = NULL;
	if (sat_literal_sign(p_literal) == SIGN_POSITIVE)
		clausePtr = literalPtr->clsPos;
	else
		clausePtr = literalPtr->clsNeg;
	while (notNull(clausePtr)) {
		tClause* clause = clausePtr->clause;
		tPtVarSgn* nextClausePtr = clausePtr->suiv;

		// Enregistrement de la suppression dans l'historique
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Saving clause %u in the history.", clause->indCls);
		p_history.addClause(clause);

		// Suppression de la clause
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Removing clause %u.", clause->indCls);
		sat_sub_clause(p_formula, clause->indCls);

		// Next
		clausePtr = nextClausePtr;
	}

	// Retrieve the literal cell
	literalPtr = p_formula->vars;
	while (notNull(literalPtr) && literalPtr->indVar != sat_literal_id(p_literal))
		literalPtr = literalPtr->suiv;
	if (literalPtr == NULL)
		return 0;
	
	// Remove the literal from the clauses that contain the oposite sign
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Removing literal %sx%u from the clauses.", (p_literal > 0 ? "¬" : ""), sat_literal_id(p_literal));
	if (sat_literal_sign(p_literal) == SIGN_NEGATIVE)
		clausePtr = literalPtr->clsPos;
	else
		clausePtr = literalPtr->clsNeg;
	while (notNull(clausePtr)) {
		tClause* clause = clausePtr->clause;
		tPtVarSgn* nextClausePtr = clausePtr->suiv;

		// Record the removal of the literal in the history
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Saving literal %sx%u of clause %u in the history.", (p_literal > 0 ? "¬" : ""), sat_literal_id(p_literal), clause->indCls);
		p_history.addLiteral(clause->indCls, -p_literal);

		// Remove the literal from the clause
		// returns: 2 if the clause is not empty
		//          3 if the clause is empty (unsatisfiable)
		if (sat_sub_var_in_cls(-p_literal, clause, p_formula) == 3) {
			log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "The produced clause is unsatisfiable.");
			rc = 1;
			break;
		}

		// Next
		clausePtr = nextClausePtr;
	}

	return rc;
}
