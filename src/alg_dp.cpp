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

#include <stdlib.h>
#include <log4c.h>

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
Interpretation* alg_solve(tGraphe** p_formula) {
	// Parameters check
	if (isNull(p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The formula pointer is NULL.");
		return NULL;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Starting the DPLL algorithm.");
	return dp_main(p_formula, new Interpretation());
}


/**
 * Main loop of the Davis-Putnam algorithm.
 * 
 * @param p_formula
 *            the SAT formula to solve
 * @param p_interpretation
 *            the current interpretation
 * 
 * @return the interpretation found,
 *         of NULL if p_formula or p_interpretation is NULL
 */
Interpretation* dp_main(tGraphe** p_formula, Interpretation* p_interpretation) {
	/* Check parameters */
	if (isNull(*p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The formula pointer is NULL.");
		return NULL;
	}

	if (isNull(p_interpretation)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The interpretation pointer is NULL.");
		return NULL;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Current state:");
	sat_see(*p_formula);
	p_interpretation->print();

	/*
	 * Stop case: if there is no clause to process, we are done.
	 * Return the current interpretation.
	 */
	if (isNull((*p_formula)->clauses))
		return p_interpretation;

	/*
	 * Choose the reduction literal.
	 * This is the crucial step, performance-wise.
	 */
	Literal chosen_literal = dp_choose_literal(*p_formula);

	/*
	 * First reduction
	 */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "First reduction attempt...");
	History* history = new History();
	dp_reduce(p_formula, chosen_literal, history);
	Interpretation* partialInterpretation = dp_test_sat(p_formula, chosen_literal, p_interpretation);

	/*
	 * The interpretation is satisfiable: we are done.
	 * We can return the current interpretation.
	 */
	if (partialInterpretation->isSatisfiable()) {
		delete history;
		return p_interpretation;
	}
	delete partialInterpretation;
	
	/*
	 * The interpretation is not satisfiable: we try with the opposite literal.
	 */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Rebuilding the formula before second attempt...");
	history->replay(p_formula);
	sat_see(*p_formula);
	p_interpretation->print();
 
	// Seconde réduction et test du résultat
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Second reduction attempt...");
	dp_reduce(p_formula, -chosen_literal, history);
	partialInterpretation = dp_test_sat(p_formula, -chosen_literal, p_interpretation);

	/* Restoring state before backtracking. */
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Restoring state before backtracking...");
	// Remove the current literal from the interpretation
	p_interpretation->pop();

	// Reconstruction du graphe & destruction de l'historique
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Rebuilding the formula...");
	history->replay(p_formula);
	delete history;

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "Restored state:");
	sat_see(*p_formula);
	p_interpretation->print();
	
	return partialInterpretation;
}


/**
 * Chooses a literal to be used for the reduction phase.
 * 
 * @param p_formula
 *            the SAT formula
 * 
 * @return the literal
 */
Literal dp_choose_literal(tGraphe* p_formula) {
	return sat_get_var_cls_unit(p_formula);
}


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
Interpretation* dp_test_sat(tGraphe** p_formula, Literal p_literal, Interpretation* p_interpretation) {
	// Si insatisfiable...
	if (isNull(*p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "The current interpretation is unsatisfiable.");
		Interpretation* interpretation = new Interpretation();
		interpretation->setUnsatisfiable();
		return interpretation;
	}

	// Ajout du littéral à l'interprétation
	p_interpretation->push(p_literal);
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Added %sx%u to the interpretation.", (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal));
	
	// Si le graphe est vide, l'interprétation est terminée
	if (isNull((*p_formula)->clauses)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "No more clauses.");
		return p_interpretation;
	}

	// Il reste des clauses : ajout du littéral à l'interprétation
	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "There are clauses remaining.");
	return dp_main(p_formula, p_interpretation);
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
 */
void dp_reduce(tGraphe **p_formula, Literal p_literal, History *p_history) {
	if (isNull(*p_formula)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The formula pointer is NULL.");
		return;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Reduction using literal %sx%u...", (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal));
	tClause* clause = (*p_formula)->clauses;
	while (clause) {
		tClause* clause2 = clause->suiv;
		switch (dp_reduce_clause(clause, p_literal, (*p_formula), p_history)) {
		case 3: // Clause vide produite: insatisfiable
			log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "The produced clause is unsatisfiable.");
				
			// Sauvegarde des modifs dans l'historique
			log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Dumping the current formula to the history.");
			clause2 = (*p_formula)->clauses;
			while (clause2) {
				p_history->addClause(clause2); 
				log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Clause %u added to the history.", clause2->indCls);
				clause2 = clause2->suiv;
			}
			
			// Suppression du graphe
			log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Deleting the current formula.");
			sat_free(p_formula);
			
			// Pour sortir du while
			clause = NULL;
			break;
		
		case 1:
			clause = clause2;
			break;
			
		default:
			clause = clause->suiv;
		}
	}
}


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
int dp_reduce_clause(tClause* p_clause, Literal p_literal, tGraphe* p_formula, History* p_history) {
	/* Parameter checking */
	if (isNull(p_clause)) {
		log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "The clause pointer is NULL.");
		return -1;
	}

	log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Reducing clause %u by %sx%u.", p_clause->indCls, (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal));
	sat_see(p_formula);

	tPtVar* literal_iterator = p_clause->vars;
	while (literal_iterator) {
		if (literal_iterator->var->indVar == sat_literal_id(p_literal)) {
			
			/* Test de polarité */
			if (sat_literal_sign(p_literal) == sat_get_sign(literal_iterator->var, p_clause->indCls)) {
				// même polarité: on vire la clause
				log4c_category_log(log_dpll(), LOG4C_PRIORITY_DEBUG, "    %sx%u found in the clause.", (p_literal < 0 ? "¬" : ""), abs(p_literal));

				// Enregistrement de la suppression dans l'historique
				log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Saving clause %u in the history.", p_clause->indCls);
				p_history->addClause(p_clause);

				// Suppression de la clause
				sat_sub_clause(p_formula, p_clause->indCls);
				return 1;
			}
			
			else { // polarité contraire: on vire le littéral de la clause
				fprintf(stderr, "    %sx%d found in the clause.\n", (p_literal >= 0 ? "¬" : ""), abs(p_literal));

				// Enregistrement de la suppression dans l'historique
				log4c_category_log(log_dpll(), LOG4C_PRIORITY_INFO, "Saving literal %sx%u of clause %u in the history.", (p_literal >= 0 ? "¬" : ""), abs(p_literal), p_clause->indCls);
				p_history->addLiteral(p_clause->indCls, literal_iterator->var->indVar * (-sat_literal_sign(p_literal)));

				// Suppression de la chosen_literal de la clause
				// retourne: 2 si la clause est non vide (après traitement)
				//           3 si elle est vide (insatisfiable).
				return sat_sub_var_in_cls(literal_iterator, -sat_literal_sign(p_literal), p_clause, p_formula);
			}
		}
		else {
			fprintf(stderr, "    literal x%d not found in the clause.\n", abs(p_literal));
			literal_iterator = literal_iterator->suiv;
		}
	}
	return 0;
}
