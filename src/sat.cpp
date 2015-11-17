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
#include "sat.h"

#include <stdio.h>
#include <string.h>
#include "utils.h"


// Crée un graphe vide --------------------------------------------------------
tGraphe* sat_new() {
	tGraphe* formula;
	formula = (tGraphe*) malloc(sizeof(tGraphe));
	formula->clauses = NULL;
	formula->vars = NULL;
	return formula;
}


// Libère le graphe
void sat_free(tGraphe** p_formula) {
	// Parameters check
	if (isNull(*p_formula))
		return;
  
	// Suppression des clauses une à une
	tClause* clause = (*p_formula)->clauses;
	while (clause) {
		sat_sub_clause((*p_formula), clause->indCls);
		clause = (*p_formula)->clauses;
	}

	// Normalement, quand toutes les variables sont supprimées
	// toutes les variables aussi: on peut donc supprimer p_formula
	free(*p_formula);
	(*p_formula) = NULL;
}
 

// Renvoie le pointeur sur une variable, NULL si elle n'existe pas ------------
tVar* sat_ex_var(tGraphe *p_formula, LiteralId p_literalId) {
	tVar* lVar;
	int trouve = 0;

	// Recherche
	lVar = p_formula->vars;
	while (notNull(lVar) && (trouve == 0)) {
		if (lVar->indVar == p_literalId)
			trouve = 1;
		else
			lVar = lVar->suiv;
	}

	if (trouve)
		fprintf(stderr, "   Littéral x%u trouvé.\n", p_literalId);
	else {
		fprintf(stderr, "   Littéral x%u non trouvé.\n", p_literalId);
		lVar = NULL;
	}
	return lVar;
}


/* Crée un lien entre une variable et une clause dans laquelle elle apparait en tenant compte du signe
   et un lien entre ces mêmes clause et variable */
void sat_lnk_clsVar(tClause* p_clause, tVar* p_litteral, int p_literalSign) {
	tPtVarSgn *ps; // Pointeur vers un pointeur dans clsPos ou clsNeg
	tPtVar    *pv; // Pointeur vers une variable 

	//I) Lien Var --> Clause
	// Création de la cellule
	ps = (tPtVarSgn*) malloc(sizeof(tPtVarSgn));

	// Lien de la variable vers la clause
	ps->clause = p_clause;

	if (p_literalSign == 1) {
		ps->suiv = p_litteral->clsPos;
		p_litteral->clsPos = ps;
	}
	else {
		ps->suiv = p_litteral->clsNeg;
		p_litteral->clsNeg = ps;
	}

	//II) Lien Clause --> Var
	// Création de la cellule
	pv = (tPtVar*) malloc(sizeof(tPtVar));

	// Lien de la clause vers la variable
	pv->var = p_litteral;

	// Chaînage
	pv->suiv = p_clause->vars;
	p_clause->vars = pv;

	fprintf(stderr, "   Lien clause %u <-> littéral %sx%u créé.\n", p_clause->indCls, (p_literalSign < 0 ? "¬" : ""), p_litteral->indVar);
}


// Ajoute une variable au graphe ----------------------------------------------
tVar* sat_add_var(tGraphe* p_formula, LiteralId p_literalId) {
	// Création de var
	tVar* var = (tVar*) malloc(sizeof(tVar));

	// Initialisation de var
	var->indVar = p_literalId;
	var->clsPos = NULL;
	var->clsNeg = NULL;
	var->suiv = NULL;
	fprintf(stderr, "   Littéral x%u créé.\n", p_literalId);

	// Chaînage
	var->suiv = p_formula->vars;
	p_formula->vars = var;
	fprintf(stderr, "   Littéral x%u chaîné.\n", p_literalId);

	return var;
}


// Ajoute une variable à une clause
int sat_add_var_to_cls(tGraphe* p_formula, ClauseId p_clauseId, Literal p_literal) {
	if (isNull(p_formula)) {
		fprintf(stderr, " Ooops: Le pointeur de graphe est NULL.\n");
		return -1;
	}

	// I) Recherche de la clause...
	tClause* clause = p_formula->clauses;
	while (clause && (clause->indCls != p_clauseId))
		clause = clause->suiv;
	
	if (isNull(clause)) {
		fprintf(stderr, "  Clause %u non trouvée.\n", p_clauseId);
		std::list<Literal> literals = { p_literal };
		sat_add_clause(p_formula, p_clauseId, literals);
		return 1;
	}

	fprintf(stderr, "  Clause %u trouvée.\n", clause->indCls);
	// Teste si la variable existe
	tVar* lVar = sat_ex_var(p_formula, sat_literal_id(p_literal));
	if (isNull(lVar)) {
		fprintf(stderr, "   Littéral x%u non trouvé: ajout.\n", sat_literal_id(p_literal));
		lVar = sat_add_var(p_formula, sat_literal_id(p_literal));
	}

	// Lien entre la variable et la clause
	sat_lnk_clsVar(clause, lVar, sat_literal_sign(p_literal));
	return 0;
}


// Ajoute une clause à un graphe ----------------------------------------------
int sat_add_clause(tGraphe* p_formula, ClauseId p_clauseId, std::list<Literal>& p_literals) {
	// Parameters check
	if (p_literals.empty()) {
		fprintf(stderr, "  Waouu: Tentative d'ajout de la clause vide.\n         Continue...\n");
		return 1;
	}
	
	// Création de la clause
	tClause* clause = (tClause*) malloc(sizeof(tClause));
	fprintf(stderr, "  Clause %u créée.\n", p_clauseId);
	
	// Màj de l'indice
	clause->indCls = p_clauseId;
	clause->suiv = NULL;
	clause->vars = NULL;
	
	// Parcours des variables de la clause
	for (auto literal = p_literals.cbegin(); literal != p_literals.cend(); ++literal) {
		fprintf(stderr, "  Lien clause %u <-> littéral x%d en cours...\n", p_clauseId, *literal);
		
		// Si la variable n'existe pas, l'ajouter.
		tVar* lVar = sat_ex_var(p_formula, sat_literal_id(*literal));
		if (isNull(lVar))
			lVar = sat_add_var(p_formula, sat_literal_id(*literal));
		
		// Linkage Clause <--> Variable    
		sat_lnk_clsVar(clause, lVar, sat_literal_sign(*literal));
	}
	fprintf(stderr, "  Clause %u initialisée.\n", p_clauseId);
	
	// Chaînage des clauses
	clause->suiv = p_formula->clauses;
	p_formula->clauses = clause;
	fprintf(stderr, "  Clause %u chaînée.\n", p_clauseId);
	
	return 0;
}


// Renvoie le signe d'une variable dans une clause ----------------------------
int sat_get_sign(tVar* p_litteral, ClauseId p_clauseId) {
	// Parameters check
	if (isNull(p_litteral)) {
		fprintf(stderr, "Ooops: le littéral est vide.\n");
		return 1;
	}

	// Recherche parmis les clauses "positives"
	tPtVarSgn* ps = p_litteral->clsPos;
	while (ps) {
		if (ps->clause->indCls == p_clauseId)
			return SIGN_POSITIVE;
		ps = ps->suiv;
	}

	// Si pas trouvé, recherche parmis les "négatifs"
	ps = p_litteral->clsNeg;
	while (ps) {
		if (ps->clause->indCls == p_clauseId)
			return SIGN_NEGATIVE;
		ps = ps->suiv;
	}

	// Cas qui ne devrait pas arriver, mais on ne sait jamais...
	return 0;
}


// Affiche les clauses d'un graphe --------------------------------------------
void sat_see(tGraphe* p_formula) {
	// Parameters check
	if (isNull(p_formula)) {
		fprintf(stderr, "  Waouu: Le pointeur de graphe est NULL.\n");
		return;
	}

	// Initialisation de pc
	tClause* pc = p_formula->clauses;
	fprintf(stderr, "  Clauses = {\n");

	// Parcours des clauses
	while (pc) {
		// Numéro de la clause
		fprintf(stderr, "   %u: ", pc->indCls);

		// Affichage de la clause
		int deb = 1;
		tPtVar* pv = pc->vars;
		while (pv) {
			int sign = sat_get_sign(pv->var, pc->indCls);

			fprintf(stderr, "%s%sx%u ", (deb != 1 ? "v " : ""), (sign == -1 ? "¬" : ""), pv->var->indVar);
			if (deb == 1)
				deb = 0;

			// Variable suivante
			pv = pv->suiv;
		}
     
		// Clause suivante
		fprintf(stderr, "\n");
		pc = pc->suiv;
	}
	fprintf(stderr, "  }\n");
	
	
	fprintf(stderr, "  Littéraux = {\n");
	tVar* litt_iter = p_formula->vars;
	while (litt_iter) {
		// Numéro de la clause
		fprintf(stderr, "   x%u [+ ", litt_iter->indVar);
		
		// Affichage des clauses "positives"
		int deb = 1;
		tPtVarSgn* clause_iter = litt_iter->clsPos;
		while (clause_iter) {
			fprintf(stderr, "%s%u", (deb != 1 ? " " : ""), clause_iter->clause->indCls);
			if (deb == 1)
				deb = 0;
			
			clause_iter = clause_iter->suiv;
		}

		fprintf(stderr, "]  [- ");
		
		// Affichage des clauses "négatives"
		deb = 1;
		clause_iter = litt_iter->clsNeg;
		while (clause_iter) {
			fprintf(stderr, "%s%u", (deb != 1 ? " " : ""), clause_iter->clause->indCls);
			if (deb == 1)
				deb = 0;
			
			clause_iter = clause_iter->suiv;
		}
		
		// Littéral suivant
		fprintf(stderr, "]\n");
		litt_iter = litt_iter->suiv;
	}
	fprintf(stderr, "  }\n");
}


// Supprime une variable -------------------------------------------
int sat_sub_var(tGraphe* p_formula, LiteralId p_literalId) {
	// Parameters check
	if (isNull(p_formula)) {
		fprintf(stderr, " Ooops: le graphe est vide, impossible de supprimer un littéral.\n");
		return -1;
	}

	if (isNull(p_formula->vars)) {
		fprintf(stderr, " Ooops: la liste des littéraux est vide, impossible d'en supprimer un.\n");
		return 2;
	}

	// Teste si c'est la première variable
	tVar* lVar = p_formula->vars;
	if (lVar->indVar == p_literalId) {
		fprintf(stderr, "  Suppression du littéral x%u.\n", lVar->indVar);
		// Chaînage préc <-> suiv
		p_formula->vars = lVar->suiv;
		// Libération de la variable
		free(lVar);
	}
	else {
		// Recherche parmis les suivantes
		while (lVar->suiv && (lVar->suiv->indVar != p_literalId)) 
			lVar = lVar->suiv;

		// lVar2: élément à supprimer
		tVar* lVar2 = lVar->suiv;

		// Vérification de lVar2
		if (!lVar2) {
			fprintf(stderr, "  Ooops: Le littéral x%u n'existe pas dans le graphe.\n", p_literalId);
			return 3;
		}

		fprintf(stderr, "  Suppression du littéral x%u.\n", lVar2->indVar);
		lVar->suiv = lVar2->suiv;
		free(lVar2);
	}
	fprintf(stderr,  "  Littéral x%u supprimé.\n", p_literalId);
	
	return 0;
}


// Supprime une variable dans une clause
int sat_sub_var_in_cls(tPtVar* pPtVar, int p_literalSign, tClause* p_clause, tGraphe* p_formula) {
	// Parameters check
	if (isNull(p_clause)) {
		fprintf(stderr, "     Ooops: Le pointeur de clause est NULL.\n");
		return -1;
	}

	if (isNull(p_formula)) {
		fprintf(stderr, "     Ooops: Le pointeur de Graphe est NULL.\n");
		return -2;
	}

	// Premier à supprimer 
	if (p_clause->vars == pPtVar) {
		// Défait les liens et tout et tout
		sat_unlnk_cls_var(p_formula, p_clause);

		// Codes de retour
		if (isNull(p_clause->vars)) {
			fprintf(stderr, "      La clause vide a été produite.\n");
			return 3;
		}

		fprintf(stderr, "       La clause modifiée est non vide.\n");
		return 2;
	}
	
	// Recherche du pointeur sur la variable
	tPtVar* lPtVar = p_clause->vars;
	while (lPtVar->suiv) {
		if (lPtVar->suiv == pPtVar) {
			tPtVarSgn* lVarSgn;

			// Suppr lien Var-> cls
			switch (p_literalSign) {
			case SIGN_POSITIVE:
				lVarSgn = lPtVar->suiv->var->clsPos;
				break;
			
			case SIGN_NEGATIVE:
				lVarSgn = lPtVar->suiv->var->clsNeg;
				break;
			
			default:
				fprintf(stderr, "      Ooops: Le signe demandé (%d) est incorrect: 1 -> positif, -1 -> négatif\n", p_literalSign);
				return 1;
			}

			// II/ Suppression du lien Variable -> Clause -------------------------
			if (isNull(lVarSgn)) {
				fprintf(stderr, "      Waouu: Le littéral %sx%u n'est pas utilisé.\n", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar);
				return 2;
			}

			if (lVarSgn->clause == p_clause) { // teste si la première clause correspond
				// Chaînage de la précédente avec la suivante
				if (p_literalSign == SIGN_POSITIVE) // Chaînage des positifs
					lPtVar->suiv->var->clsPos = lVarSgn->suiv;
				else
					lPtVar->suiv->var->clsNeg = lVarSgn->suiv;
				// Libération de la cellule de la variable
				free(lVarSgn);
				fprintf(stderr, "      Lien littéral %sx%u -> clause %u détruit.\n", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar, p_clause->indCls);
			}
			else { // la clause n'est pas la première
				lVarSgn = sat_get_ptr_varSgn(lVarSgn, p_clause);
				if (notNull(lVarSgn)) { // cellule trouvée
					tPtVarSgn* toDelete = lVarSgn->suiv;
					// Lien préc <-> suiv
					lVarSgn->suiv = lVarSgn->suiv->suiv;
					// Libération de la cellule
					free(toDelete);
					fprintf(stderr, "      Lien littéral %sx%u -> clause %u détruit.\n", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar, p_clause->indCls);
				}
				else {
					fprintf(stderr, "      Waouu: Pas de lien littéral %sx%u -> clause %u trouvé.\n", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar, p_clause->indCls);
					return 3;
				}
			}

			// Sur-lien cls -> var suiv
			tPtVar* lPtVar2 = lPtVar->suiv;
			lPtVar->suiv = lPtVar->suiv->suiv;
			fprintf(stderr, "      Lien clause %u -> littéral x%u détruit.\n", p_clause->indCls, lPtVar2->var->indVar);

			// Si la variable est vide, la virer
			if (isNull(lPtVar2->var->clsPos) && isNull(lPtVar2->var->clsNeg))
				sat_sub_var(p_formula, lPtVar2->var->indVar);

			// Suppression de la cellule
			free(lPtVar2);

			// Codes de retour
			if (isNull(p_clause->vars)) {
				fprintf(stderr, "      La clause vide a été produite.\n");
				return 3;
			}
			else {
				fprintf(stderr, "      La clause modifiée est non vide.\n");
				return 2;
			}
		}
		else
			lPtVar = lPtVar->suiv;
	}

	fprintf(stderr, "      Waouu: La clause n'a pas été modifiée.\n");
	return 4;
}


// Renvoie le pointeur sur la cellule de la clause pointant sur la clause
tPtVarSgn* sat_get_ptr_varSgn(tPtVarSgn* p_signedLiteral, tClause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		fprintf(stderr, "    Ooops: La clause n'existe pas.\n");
		return NULL;
	}

	if (isNull(p_signedLiteral)) {
		fprintf(stderr, "    Ooops: Pas de lien entre le littéral et une clause");
		return NULL;
	}

	// Recherche
	tPtVarSgn* lVarSgn = p_signedLiteral;
	while (notNull(lVarSgn->suiv) && (lVarSgn->suiv->clause->indCls != p_clause->indCls))
		lVarSgn = lVarSgn->suiv;

	// Si pas de résultat trouvé, retourne NULL, sinon le pointeur voulu
	if (isNull(lVarSgn->suiv))
		return NULL;
	return lVarSgn;
}


// Recherche et supprime la cellule de la variable pointant sur la clause
int sat_unlnk_varSgn_cls(tVar* p_litteral, int p_literalSign, tClause* p_clause) {
	// I/ Tests d'application --------------------------------------------
	// Teste le signe et initialise la recherche
	tPtVarSgn* lVarSgn;
	switch (p_literalSign) {
	case 1:
		lVarSgn = p_litteral->clsPos;
		break;
		
	case -1:
		lVarSgn = p_litteral->clsNeg;
		break;
	
	default:
		fprintf(stderr, "   Ooops: Le signe demandé (%d) est incorrect: 1 -> positif, -1 -> négatif\n", p_literalSign);
		return 1;
	}

	// II/ Suppression du lien Variable -> Clause -------------------------
	if (isNull(lVarSgn)) {
		fprintf(stderr, "   Waouu: Le littéral %sx%u n'est pas utilisé.\n", (p_literalSign == -1 ? "¬" : ""), p_litteral->indVar);
		return 2;
	}

	if (lVarSgn->clause->indCls == p_clause->indCls) { // teste si la première clause correspond
		// Chaînage de la précédente avec la suivante
		if (p_literalSign == 1) // Chaînage des positifs
			p_litteral->clsPos = lVarSgn->suiv;
		else
			p_litteral->clsNeg = lVarSgn->suiv;
		// Libération de la cellule de la variable
		free(lVarSgn);
		lVarSgn = NULL;
		fprintf(stderr, "   Lien littéral %sx%u -> clause %u détruit.\n", (p_literalSign == -1 ? "¬" : ""), p_litteral->indVar, p_clause->indCls); 
	}
	else { // la clause n'est pas la première
		lVarSgn = sat_get_ptr_varSgn(lVarSgn, p_clause);
		if (notNull(lVarSgn)) {
			// Lien préc <-> suiv
			tPtVarSgn* toDelete = lVarSgn->suiv;
			lVarSgn->suiv = lVarSgn->suiv->suiv;
			// Libération de la cellule
			free(toDelete);
			fprintf(stderr, "   Lien littéral %sx%u -> clause %u détruit.\n", (p_literalSign == -1 ? "¬" : ""), p_litteral->indVar, p_clause->indCls);
		}
		else {
			fprintf(stderr, "   Waouu: Pas de lien littéral %sx%u -> clause %u trouvé.\n", (p_literalSign == -1 ? "¬" : ""), p_litteral->indVar, p_clause->indCls);
			return 3;
		}
	}

	// III/ Suppression du lien Clause -> Variable ---------------------
	tPtVar* lPtVar = p_clause->vars;
	if (lPtVar->var == p_litteral) {
		p_clause->vars = lPtVar->suiv;
		free(lPtVar);
		lPtVar = NULL;
	}
	else {
		// Search the parent of the literal pointer to remove
		while (notNull(lPtVar->suiv) && lPtVar->suiv->var != p_litteral)
			lPtVar = lPtVar->suiv;
		
		// If the literal pointer has been found
		if (notNull(lPtVar->suiv)) {
			lPtVar->suiv = lPtVar->suiv->suiv;
			free(lPtVar);
			lPtVar = NULL;
		}
	}
	fprintf(stderr, "   Lien littéral %sx%u <- clause %u détruit.\n", (p_literalSign == -1 ? "¬" : ""), p_litteral->indVar, p_clause->indCls);
	return 0;
}


// Enlève les liens entre une clause et sa première variable
int sat_unlnk_cls_var(tGraphe* p_formula, tClause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		fprintf(stderr, "  Ooops: La clause n'existe pas.\n");
		return 1;
	}

	fprintf(stderr, "  Suppression des liens entre la clause %u et son premier littéral...\n", p_clause->indCls);
	// Vérification de la liste des variables de la clause
	if (isNull(p_clause->vars)) {
		fprintf(stderr, "  Waouu: La clause %u ne contient pas de littéraux.\n", p_clause->indCls);
		return 2;
	}

	// Initialisation de lVar
	tVar* lVar = p_clause->vars->var;

	// Test de validité de lVar
	if (isNull(lVar)) {
		fprintf(stderr, "  Ooops: Le littéral est NULL.\n");
		return 3;
	}

	// Appels de sat_unlnk_varSgn_cls
	if (sat_unlnk_varSgn_cls(lVar, 1, p_clause) == 0)
		fprintf(stderr, "  Lien littéral x%u <-> clause %u détruit.\n", lVar->indVar, p_clause->indCls);
	else if (sat_unlnk_varSgn_cls(lVar, -1, p_clause) == 0)
		fprintf(stderr, "  Lien littéral ¬x%u <-> clause %u détruit.\n", lVar->indVar, p_clause->indCls);
	else
		fprintf(stderr, "  Waouu: Impossible de trouver le lien littéral x%u -> clause %u.\n", lVar->indVar, p_clause->indCls);

	// Après destruction des liens, on teste si on peut supprimer la variable
	if (isNull(lVar->clsPos) && isNull(lVar->clsNeg))
		sat_sub_var(p_formula, lVar->indVar);
  
	return 0;
}


// Enlève les liens entre une clause et toutes ses variables
int sat_sub_clause(tGraphe* p_formula, ClauseId p_clauseId) {
	// Parameters check
	if (isNull(p_formula)) {
		fprintf(stderr, " Ooops: Aucune clause à supprimer car le graphe est vide !\n");
		return 1;
	}

	// Initialisation
	tClause* cls = p_formula->clauses;
	if (isNull(cls)) {
		fprintf(stderr, " Ooops: Il n'y a pas de clause dans ce graphe !\n");
		return 2;
	}
  
	// Si c'est la première clause ...
	fprintf(stderr, "\n Suppression de la clause %u...\n", p_clauseId);
	tPtVar* ptVars;
	if (cls->indCls == p_clauseId) {
		// Destruction des liens
		ptVars = cls->vars;
		while (notNull(ptVars)) {
			sat_unlnk_cls_var(p_formula, cls);
			ptVars = cls->vars;
		}
		// On re-chaîne
		p_formula->clauses = cls->suiv;
		// On libère cls
		free(cls);
		fprintf(stderr, " Clause %u détruite.\n", p_clauseId);
	}
	else {
		// Recherche du pointeur sur la clause d'indice p_clauseId
		while (notNull(cls->suiv) && (cls->suiv->indCls != p_clauseId)) 
			cls = cls->suiv;
		tClause* cls2 = cls->suiv;

		// On retire les liens
		ptVars = cls2->vars;
		while (notNull(ptVars)) {
			sat_unlnk_cls_var(p_formula, cls2);
			ptVars = cls2->vars;
		}
		
		// On re-chaîne avec le suivant
		cls->suiv = cls->suiv->suiv;
    
		// On libère cls
		free(cls2);
		fprintf(stderr, " Clause %u détruite.\n", p_clauseId);
	}
	
	return 0;
}


// Renvoie la variable de la première clause unitaire trouvée
//  0 si non trouvée
int sat_get_var_cls_unit(tGraphe* p_formula) {
	// Parameters check
	if (isNull(p_formula)) {
		fprintf(stderr, "  Ooops: Le pointeur de graphe est NULL.\n");
		return 1;
	}

	// Recherche...
	tClause* clause = p_formula->clauses;
	while (clause) {
		if (isNull(clause->vars))
			fprintf(stderr, "  Waouu: La clause %u ne contient pas de littéraux.\n", clause->indCls);
		else if (isNull(clause->vars->suiv)) {
			int rslt = sat_get_sign(clause->vars->var, clause->indCls) * clause->vars->var->indVar;
			fprintf(stderr, "  La clause %u (%sx%u) est unitaire.\n", clause->indCls, (rslt < 0 ? "¬" : ""), sat_literal_id(rslt));
			return rslt;
		}
		else {
			fprintf(stderr, "  La clause %u n'est pas unitaire.\n", clause->indCls);
			clause = clause->suiv;
		}
	}

	// Pas de clause unitaire trouvée
	if (isNull(p_formula->vars)) {
		fprintf(stderr, "  Ooops: Plus de littéraux dans le graphe.\n");
		return 0;
	}
	tVar* lVar = p_formula->vars;
	if (isNull(lVar->clsPos)) {
		if (isNull(lVar->clsNeg)) {
			fprintf(stderr, "  Ooops: Le littéral x%u n'est liée à aucune clause.\n", lVar->indVar);
			return 0;
		}
		else
			return -lVar->indVar;
	}
	else
		return lVar->indVar;
}
