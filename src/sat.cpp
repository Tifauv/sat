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

#include <string>
#include <stdlib.h>
#include <log4c.h>
#include "utils.h"
#include "log.h"


// Crée un graphe vide --------------------------------------------------------
tGraphe* sat_new() {
	tGraphe* formula;
	formula = (tGraphe*) malloc(sizeof(tGraphe));
	formula->clauses = NULL;
	formula->vars = NULL;
	return formula;
}


// Libère le graphe
void sat_free(tGraphe* p_formula) {
	// Parameters check
	if (isNull(p_formula))
		return;
  
	// Suppression des clauses une à une
	tClause* clause = p_formula->clauses;
	while (clause) {
		sat_sub_clause(*p_formula, clause->indCls);
		clause = p_formula->clauses;
	}

	// Normalement, quand toutes les variables sont supprimées
	// toutes les variables aussi: on peut donc supprimer p_formula
	free(p_formula);
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Formula deleted.");
}
 

// Renvoie le pointeur sur une variable, NULL si elle n'existe pas ------------
tVar* sat_ex_var(tGraphe& p_formula, LiteralId p_literalId) {
	tVar* lVar;
	int trouve = 0;

	// Recherche
	lVar = p_formula.vars;
	while (notNull(lVar) && (trouve == 0)) {
		if (lVar->indVar == p_literalId)
			trouve = 1;
		else
			lVar = lVar->suiv;
	}

	if (trouve)
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u found.", p_literalId);
	else {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u not found.", p_literalId);
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

	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link clause %u <-> literal %sx%u created.", p_clause->indCls, (p_literalSign < 0 ? "¬" : ""), p_litteral->indVar);
}


// Ajoute une variable au graphe ----------------------------------------------
tVar* sat_add_var(tGraphe& p_formula, LiteralId p_literalId) {
	// Création de var
	tVar* var = (tVar*) malloc(sizeof(tVar));

	// Initialisation de var
	var->indVar = p_literalId;
	var->clsPos = NULL;
	var->clsNeg = NULL;
	var->suiv = NULL;
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u created.", p_literalId);

	// Chaînage
	var->suiv = p_formula.vars;
	p_formula.vars = var;
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u added.", p_literalId);

	return var;
}


// Ajoute une variable à une clause
int sat_add_var_to_cls(tGraphe& p_formula, ClauseId p_clauseId, Literal p_literal) {
	// I) Recherche de la clause...
	tClause* clause = p_formula.clauses;
	while (clause && (clause->indCls != p_clauseId))
		clause = clause->suiv;
	
	if (isNull(clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u not found.", p_clauseId);
		std::list<Literal> literals = { p_literal };
		sat_add_clause(p_formula, p_clauseId, literals);
		return 1;
	}

	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u found.", clause->indCls);
	// Teste si la variable existe
	tVar* lVar = sat_ex_var(p_formula, sat_literal_id(p_literal));
	if (isNull(lVar)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u not found, adding it.", sat_literal_id(p_literal));
		lVar = sat_add_var(p_formula, sat_literal_id(p_literal));
	}

	// Lien entre la variable et la clause
	sat_lnk_clsVar(clause, lVar, sat_literal_sign(p_literal));
	return 0;
}


// Ajoute une clause à un graphe ----------------------------------------------
int sat_add_clause(tGraphe& p_formula, ClauseId p_clauseId, std::list<Literal>& p_literals) {
	// Parameters check
	if (p_literals.empty()) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "Attempting to add an empty clause.");
		return 1;
	}
	
	// Création de la clause
	tClause* clause = (tClause*) malloc(sizeof(tClause));
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u created.", p_clauseId);

	
	// Màj de l'indice
	clause->indCls = p_clauseId;
	clause->suiv = NULL;
	clause->vars = NULL;
	
	// Parcours des variables de la clause
	for (auto literal = p_literals.cbegin(); literal != p_literals.cend(); ++literal) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Linking clause %u <-> literal %sx%u...", p_clauseId, (*literal < 0 ? "¬" : ""), sat_literal_id(*literal));
		
		// Si la variable n'existe pas, l'ajouter.
		tVar* lVar = sat_ex_var(p_formula, sat_literal_id(*literal));
		if (isNull(lVar))
			lVar = sat_add_var(p_formula, sat_literal_id(*literal));
		
		// Linkage Clause <--> Variable    
		sat_lnk_clsVar(clause, lVar, sat_literal_sign(*literal));
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u initialized.", p_clauseId);
	
	// Chaînage des clauses
	clause->suiv = p_formula.clauses;
	p_formula.clauses = clause;
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u added.", p_clauseId);

	return 0;
}


// Renvoie le signe d'une variable dans une clause ----------------------------
int sat_get_sign(tVar* p_litteral, ClauseId p_clauseId) {
	// Parameters check
	if (isNull(p_litteral)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The literal is empty.");
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
void sat_log(tGraphe& p_formula) {
	// Initialisation de pc
	tClause* pc = p_formula.clauses;
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clauses = {");

	// Parcours des clauses
	while (pc) {
		// Numéro de la clause
		std::string line = "   " + std::to_string(pc->indCls) + ": ";

		// Affichage de la clause
		int deb = 1;
		tPtVar* pv = pc->vars;
		while (pv) {
			int sign = sat_get_sign(pv->var, pc->indCls);

			line.append(deb != 1 ? " v " : "").append(sign == -1 ? "¬x" : "x").append(std::to_string(pv->var->indVar));
			if (deb == 1)
				deb = 0;

			// Variable suivante
			pv = pv->suiv;
		}
     
		// Clause suivante
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, line.data());
		pc = pc->suiv;
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "}");

	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literals = {");
	tVar* litt_iter = p_formula.vars;
	while (litt_iter) {
		// Numéro du litéral
		std::string line = "   x" + std::to_string(litt_iter->indVar) + " [+ ";
		
		// Affichage des clauses "positives"
		int deb = 1;
		tPtVarSgn* clause_iter = litt_iter->clsPos;
		while (clause_iter) {
			line.append(deb != 1 ? " " : "").append(std::to_string(clause_iter->clause->indCls));
			if (deb == 1)
				deb = 0;
			
			clause_iter = clause_iter->suiv;
		}

		line.append("]  [- ");

		// Affichage des clauses "négatives"
		deb = 1;
		clause_iter = litt_iter->clsNeg;
		while (clause_iter) {
			line.append(deb != 1 ? " " : "").append(std::to_string(clause_iter->clause->indCls));
			if (deb == 1)
				deb = 0;
			
			clause_iter = clause_iter->suiv;
		}
		
		// Littéral suivant
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, line.append("]").data());
		litt_iter = litt_iter->suiv;
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "}");
}


// Supprime une variable -------------------------------------------
int sat_sub_var(tGraphe& p_formula, LiteralId p_literalId) {
	if (isNull(p_formula.vars)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The literals list of the formula is empty, cannot remove one.");
		return 2;
	}

	// Teste si c'est la première variable
	tVar* lVar = p_formula.vars;
	if (lVar->indVar == p_literalId) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Removing literal x%u.", lVar->indVar);
		// Chaînage préc <-> suiv
		p_formula.vars = lVar->suiv;
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
		if (isNull(lVar2)) {
			log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "The literal x%u does not exist in the formula.", p_literalId);
			return 3;
		}

		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Removing literal x%u.", lVar2->indVar);
		lVar->suiv = lVar2->suiv;
		free(lVar2);
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Literal x%u deleted.", p_literalId);

	return 0;
}


// Supprime une variable dans une clause
int sat_sub_var_in_cls(Literal p_literal, tClause* p_clause, tGraphe& p_formula) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The clause pointer is NULL.");
		return -1;
	}

	// Premier à supprimer 
	if (p_clause->vars->var->indVar == sat_literal_id(p_literal)) {
		// Défait les liens et tout et tout
		sat_unlnk_cls_var(p_formula, p_clause);

		// Codes de retour
		if (isNull(p_clause->vars)) {
			log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "The reduced clause is empty.");
			return 3;
		}

		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "The reduced clause is not empty.");
		return 2;
	}
	
	// Recherche du pointeur sur la variable
	tPtVar* lPtVar = p_clause->vars;
	while (lPtVar->suiv) {
		if (lPtVar->suiv->var->indVar == sat_literal_id(p_literal)) {
			tPtVarSgn* lVarSgn;

			// Suppr lien Var-> cls
			switch (sat_literal_sign(p_literal)) {
			case SIGN_POSITIVE:
				lVarSgn = lPtVar->suiv->var->clsPos;
				break;
			
			case SIGN_NEGATIVE:
				lVarSgn = lPtVar->suiv->var->clsNeg;
				break;
			
			default:
				log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The requested sign (%d) is not valid. Should be either 1 (positive) or -1 (negative).", sat_literal_sign(p_literal));
				return 1;
			}

			// II/ Suppression du lien Variable -> Clause -------------------------
			if (isNull(lVarSgn)) {
				log4c_category_log(log_formula(), LOG4C_PRIORITY_WARN, "The literal %sx%u is unused.", (sat_literal_sign(p_literal) == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar);
				return 2;
			}

			if (lVarSgn->clause == p_clause) { // teste si la première clause correspond
				// Chaînage de la précédente avec la suivante
				if (sat_literal_sign(p_literal) == SIGN_POSITIVE) // Chaînage des positifs
					lPtVar->suiv->var->clsPos = lVarSgn->suiv;
				else
					lPtVar->suiv->var->clsNeg = lVarSgn->suiv;
				// Libération de la cellule de la variable
				free(lVarSgn);
				log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal %sx%u -> clause %u removed.", (sat_literal_sign(p_literal) == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar, p_clause->indCls);
			}
			else { // la clause n'est pas la première
				lVarSgn = sat_get_ptr_varSgn(lVarSgn, p_clause);
				if (notNull(lVarSgn)) { // cellule trouvée
					tPtVarSgn* toDelete = lVarSgn->suiv;
					// Lien préc <-> suiv
					lVarSgn->suiv = lVarSgn->suiv->suiv;
					// Libération de la cellule
					free(toDelete);
					log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal %sx%u -> clause %u removed.", (sat_literal_sign(p_literal) == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar, p_clause->indCls);
				}
				else {
					log4c_category_log(log_formula(), LOG4C_PRIORITY_WARN, "No link literal %sx%u -> clause %u.", (sat_literal_sign(p_literal) == SIGN_NEGATIVE ? "¬" : ""), lPtVar->suiv->var->indVar, p_clause->indCls);
					return 3;
				}
			}

			// Sur-lien cls -> var suiv
			tPtVar* lPtVar2 = lPtVar->suiv;
			lPtVar->suiv = lPtVar->suiv->suiv;
			log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link clause %u -> literal x%u removed.", p_clause->indCls, lPtVar2->var->indVar);

			// Si la variable est vide, la virer
			if (isNull(lPtVar2->var->clsPos) && isNull(lPtVar2->var->clsNeg))
				sat_sub_var(p_formula, lPtVar2->var->indVar);

			// Suppression de la cellule
			free(lPtVar2);

			// Codes de retour
			if (isNull(p_clause->vars)) {
				log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "The reduced clause is empty.");
				return 3;
			}
			else {
				log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "The reduced clause is not empty.");
				return 2;
			}
		}
		else
			lPtVar = lPtVar->suiv;
	}

	log4c_category_log(log_formula(), LOG4C_PRIORITY_WARN, "The clause was not modified.");
	return 4;
}


// Renvoie le pointeur sur la cellule de la clause pointant sur la clause
tPtVarSgn* sat_get_ptr_varSgn(tPtVarSgn* p_signedLiteral, tClause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The clause is NULL.");
		return NULL;
	}

	if (isNull(p_signedLiteral)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "No link between the literal and a clause.");
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
int sat_unlnk_varSgn_cls(tVar* p_literal, int p_literalSign, tClause* p_clause) {
	// I/ Tests d'application --------------------------------------------
	// Teste le signe et initialise la recherche
	tPtVarSgn* lVarSgn;
	switch (p_literalSign) {
	case 1:
		lVarSgn = p_literal->clsPos;
		break;

	case -1:
		lVarSgn = p_literal->clsNeg;
		break;

	default:
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The requested sign (%d) is not valid. Should be either 1 (positive) or -1 (negative).", p_literalSign);
		return 1;
	}

	// II/ Suppression du lien Variable -> Clause -------------------------
	if (isNull(lVarSgn)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_WARN, "The literal %sx%u is unused.", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), p_literal->indVar);
		return 2;
	}

	if (lVarSgn->clause->indCls == p_clause->indCls) { // teste si la première clause correspond
		// Chaînage de la précédente avec la suivante
		if (p_literalSign == 1) // Chaînage des positifs
			p_literal->clsPos = lVarSgn->suiv;
		else
			p_literal->clsNeg = lVarSgn->suiv;
		// Libération de la cellule de la variable
		free(lVarSgn);
		lVarSgn = NULL;
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal %sx%u -> clause %u removed.", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), p_literal->indVar, p_clause->indCls);
	}
	else { // la clause n'est pas la première
		lVarSgn = sat_get_ptr_varSgn(lVarSgn, p_clause);
		if (notNull(lVarSgn)) {
			// Lien préc <-> suiv
			tPtVarSgn* toDelete = lVarSgn->suiv;
			lVarSgn->suiv = lVarSgn->suiv->suiv;
			// Libération de la cellule
			free(toDelete);
			log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal %sx%u -> clause %u removed.", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), p_literal->indVar, p_clause->indCls);
		}
		else {
			log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "No link literal %sx%u -> clause %u.", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), p_literal->indVar, p_clause->indCls);
			return 3;
		}
	}

	// III/ Suppression du lien Clause -> Variable ---------------------
	tPtVar* lPtVar = p_clause->vars;
	if (lPtVar->var == p_literal) {
		p_clause->vars = lPtVar->suiv;
		free(lPtVar);
		lPtVar = NULL;
	}
	else {
		// Search the parent of the literal pointer to remove
		while (notNull(lPtVar->suiv) && lPtVar->suiv->var != p_literal)
			lPtVar = lPtVar->suiv;
		
		// If the literal pointer has been found
		if (notNull(lPtVar->suiv)) {
			lPtVar->suiv = lPtVar->suiv->suiv;
			free(lPtVar);
			lPtVar = NULL;
		}
	}
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal %sx%u -> clause %u deleted.", (p_literalSign == SIGN_NEGATIVE ? "¬" : ""), p_literal->indVar, p_clause->indCls);
	return 0;
}


// Enlève les liens entre une clause et sa première variable
int sat_unlnk_cls_var(tGraphe& p_formula, tClause* p_clause) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The clause is NULL.");
		return 1;
	}

	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Unlinking clause %u and its first literal...", p_clause->indCls);
	// Vérification de la liste des variables de la clause
	if (isNull(p_clause->vars)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_WARN, "The clause %u is empty.", p_clause->indCls);
		return 2;
	}

	// Initialisation de lVar
	tVar* lVar = p_clause->vars->var;

	// Test de validité de lVar
	if (isNull(lVar)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The literal is NULL.");
		return 3;
	}

	// Appels de sat_unlnk_varSgn_cls
	if (sat_unlnk_varSgn_cls(lVar, 1, p_clause) == 0)
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal x%u <-> clause %u removed.", lVar->indVar, p_clause->indCls);
	else if (sat_unlnk_varSgn_cls(lVar, -1, p_clause) == 0)
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Link literal ¬x%u <-> clause %u removed.", lVar->indVar, p_clause->indCls);
	else
		log4c_category_log(log_formula(), LOG4C_PRIORITY_WARN, "Cannot find a link literal x%u -> clause %u.", lVar->indVar, p_clause->indCls);

	// Après destruction des liens, on teste si on peut supprimer la variable
	if (isNull(lVar->clsPos) && isNull(lVar->clsNeg))
		sat_sub_var(p_formula, lVar->indVar);
  
	return 0;
}


// Enlève les liens entre une clause et toutes ses variables
int sat_sub_clause(tGraphe& p_formula, ClauseId p_clauseId) {
	// Initialisation
	tClause* cls = p_formula.clauses;
	if (isNull(cls)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "The formula has no clause.");
		return 2;
	}
  
	// Si c'est la première clause ...
	log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Removing clause %u...", p_clauseId);
	tPtVar* ptVars;
	if (cls->indCls == p_clauseId) {
		// Destruction des liens
		ptVars = cls->vars;
		while (notNull(ptVars)) {
			sat_unlnk_cls_var(p_formula, cls);
			ptVars = cls->vars;
		}
		// On re-chaîne
		p_formula.clauses = cls->suiv;
		// On libère cls
		free(cls);
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u deleted.", p_clauseId);
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
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "Clause %u deleted.", p_clauseId);
	}
	
	return 0;
}


// Renvoie le litéral de la première clause unitaire trouvée
//  0 si non trouvée
Literal sat_find_literal_from_unary_clause(tGraphe& p_formula) {
	tClause* clause = p_formula.clauses;
	while (notNull(clause)) {
		if (notNull(clause->vars) && isNull(clause->vars->suiv)) {
			int sign = sat_get_sign(clause->vars->var, clause->indCls);
			log4c_category_log(log_formula(), LOG4C_PRIORITY_INFO, "The clause %u is unary.", clause->indCls);
			return sign * clause->vars->var->indVar;
		}
		
		log4c_category_log(log_formula(), LOG4C_PRIORITY_DEBUG, "The clause %u is not unary.", clause->indCls);
		clause = clause->suiv;
	}
	
	return 0;
}


// Renvoie le premier litéral trouvé
//  0 si non trouvée
Literal sat_get_first_literal(tGraphe& p_formula) {
	if (isNull(p_formula.vars)) {
		log4c_category_log(log_formula(), LOG4C_PRIORITY_ERROR, "No more literals in the formula.");
		return 0;
	}

	// Recherche...
	int sign;
	tVar* first_literal = p_formula.vars;
	if (isNull(first_literal->clsPos)) {
		if (isNull(first_literal->clsNeg))
			return 0;
		
		sign = SIGN_NEGATIVE;
	}
	else
		sign = SIGN_POSITIVE;

	return sign * first_literal->indVar;
}
