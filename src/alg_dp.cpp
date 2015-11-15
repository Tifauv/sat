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

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


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
		fprintf(stderr, " Ooops: Le pointeur de formule est NULL.\n");
		return NULL;
	}

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
		fprintf(stderr, " Ooops: Le pointeur de formule est NULL.\n");
		return NULL;
	}

	if (isNull(p_interpretation)) {
		fprintf(stderr, " Ooops: Le pointeur de l'interprétation est NULL.\n");
		return NULL;
	}

	fprintf(stderr, "\n\n Davis-Putnam...\n");
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
	fprintf(stderr, " Première tentative de réduction...\n");
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
	fprintf(stderr, " Reconstruction des formules pour seconde tentative...\n");
	history->replay(p_formula);
	sat_see(*p_formula);
	p_interpretation->print();
 
	// Seconde réduction et test du résultat
	fprintf(stderr, " Seconde tentative de réduction...\n");
	dp_reduce(p_formula, -chosen_literal, history);
	partialInterpretation = dp_test_sat(p_formula, -chosen_literal, p_interpretation);

	/* Restoring state before backtracking. */
	fprintf(stderr, " Restoring state before backtracking...\n");
	// Remove the current literal from the interpretation
	p_interpretation->pop();

	// Reconstruction du graphe & destruction de l'historique
	fprintf(stderr, "  Rebuilding the formula...\n");
	history->replay(p_formula);
	delete history;

	fprintf(stderr, "  Rebuilt data:\n");
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
	fprintf(stderr, "\n  Test de satisfiabilité: ");
	// Si insatisfiable...
	if (isNull(*p_formula)) {
		fprintf(stderr, "Insatisfiable\n");
		Interpretation* interpretation = new Interpretation();
		interpretation->setUnsatisfiable();
		return interpretation;
	}

	// Ajout du littéral à l'interprétation
	fprintf(stderr, "  Ajout de %sx%u à l'interprétation.\n", (p_literal < 0 ? "¬" : ""), sat_literal_id(p_literal));
	p_interpretation->push(p_literal);

	// Si le graphe est vide, l'interprétation est terminée
	if (isNull((*p_formula)->clauses)) {
		fprintf(stderr, "Plus de clauses\n");
		return p_interpretation;
	}

	// Il reste des clauses : ajout du littéral à l'interprétation
	fprintf(stderr, "Il reste des clauses\n");
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
		fprintf(stderr, "  Waouu: Le pointeur de graphe est NULL.\n");
		return;
	}

	fprintf(stderr, "  Réduction par %sx%d...\n", (p_literal < 0 ? "¬" : ""), abs(p_literal));
	tClause* clause = (*p_formula)->clauses;
	while (clause) {
		tClause* clause2 = clause->suiv;
		switch (dp_reduce_clause(clause, p_literal, (*p_formula), p_history)) {
		case 3: // Clause vide produite: insatisfiable
			fprintf(stderr, "  Insatisfiable.\n");
				
			// Sauvegarde des modifs dans l'historique
			fprintf(stderr, "  Sauvegarde du graphe actuel dans l'historique...\n");
			clause2 = (*p_formula)->clauses;
			while (clause2) {
				p_history->addClause(clause2); 
				fprintf(stderr, "   Clause n°%u mise dans l'historique.\n", clause2->indCls);
				clause2 = clause2->suiv;
			}
			
			// Suppression du graphe
			fprintf(stderr, "  Suppression du graphe...\n");
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
	tPtVar* literal_iterator;

	/* Parameter checking */
	if (isNull(p_clause)) {
		fprintf(stderr, " Ooops: Le pointeur de clause est NULL.\n");
		return -1;
	}

	fprintf(stderr, "\n   Réduction de la clause n°%u par %sx%d.\n", p_clause->indCls, (p_literal < 0 ? "¬" : ""), abs(p_literal));
	sat_see(p_formula);
	
	literal_iterator = p_clause->vars;
	while (literal_iterator) {
		if (literal_iterator->var->indVar == sat_literal_id(p_literal)) {
			
			/* Test de polarité */
			if (sat_sign(p_literal) == sat_get_sign(literal_iterator->var, p_clause->indCls)) {
				// même polarité: on vire la clause
				fprintf(stderr, "    %sx%d found in the clause.\n", (p_literal < 0 ? "¬" : ""), abs(p_literal));

				// Enregistrement de la suppression dans l'historique
				fprintf(stderr, "    Sauvegarde de la clause n°%u dans l'historique.\n", p_clause->indCls);
				p_history->addClause(p_clause);

				// Suppression de la clause
				sat_sub_clause(p_formula, p_clause->indCls);
				return 1;
			}
			
			else { // polarité contraire: on vire le littéral de la clause
				fprintf(stderr, "    %sx%d found in the clause.\n", (p_literal >= 0 ? "¬" : ""), abs(p_literal));

				// Enregistrement de la suppression dans l'historique
				fprintf(stderr, "    Sauvegarde du littéral %sx%d pour la clause n°%u dans l'historique.\n", (p_literal >= 0 ? "¬" : ""), abs(p_literal), p_clause->indCls);
				p_history->addLiteral(p_clause->indCls, literal_iterator->var->indVar * (-sat_sign(p_literal)));

				// Suppression de la chosen_literal de la clause
				// retourne: 2 si la clause est non vide (après traitement)
				//           3 si elle est vide (insatisfiable).
				return sat_sub_var_in_cls(literal_iterator, -sat_sign(p_literal), p_clause, p_formula);
			}
		}
		else {
			fprintf(stderr, "    literal x%d not found in the clause.\n", abs(p_literal));
			literal_iterator = literal_iterator->suiv;
		}
	}
	return 0;
}
