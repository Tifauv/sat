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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define LN_SIZE 1024


// Crée un graphe vide --------------------------------------------------------
tGraphe *sat_new() {
	tGraphe *lGraph;
	lGraph = (tGraphe *) malloc(sizeof(tGraphe));
	lGraph -> clauses = NULL;
	lGraph -> vars = NULL;
	return lGraph;
} // sat_new


// Libère le graphe
void sat_free(tGraphe** p_formula) {
	tClause* clause;

	if (isNull(*p_formula))
		return;
  
	// Il y a des clauses
	clause = (*p_formula)->clauses;

	printf("  [");

	// Suppression des clauses une à une
	while (clause) {
		sat_sub_clause((*p_formula), clause->indCls);
		clause = (*p_formula)->clauses;
		printf("#");
	}

	printf("]\n");
	// Normalement, quand toutes les variables sont supprimées
	// toutes les variables aussi: on peut donc supprimer p_formula
	free(*p_formula);
	(*p_formula) = NULL;
} // sat_free
 

// Charge un fichier dans un graphe vide à l'origine --------------------------
tGraphe *sat_load_file(char *pNom_fic) {
  FILE *fic;
  char *str;
  int *tab;
  int size=0, i=0, lIndCls=1;
  tGraphe *p_formula;

  // Initialisation de p_formula
  p_formula = sat_new();

  // Initialisation de str
  str = malloc(LN_SIZE);
  strcpy(str, "\0");

  // Ouverture du fichier
  fic = fopen(pNom_fic, "r");
  if (fic == NULL) {
    fprintf(stderr, " Ooops: Impossible d'ouvrir le fichier %s.\n", pNom_fic);
    exit(1);
  }
  fprintf(stderr, " Fichier %s ouvert.\n", pNom_fic);

  printf("  [");

  while (!feof(fic)) {
    
    // Compteur de ligne
    i++;

    // lecture de la ligne
    fgets(str, LN_SIZE, fic);
    if (feof(fic)) break;
    printf("#");
    str[strlen(str)-1] = '\0';
    fprintf(stderr, "\n Ligne[%d] = |%s|\n", i, str);
    
    // Ignore comment lines
    if (str[0] == 'c')
      continue;
    
    // Ignore project lines
    if (str[0] == 'p')
      continue;
    
    // Break at '%' lines
    if (str[0] == '%')
      break;

    // Transformation string -> tab
    tab = sat_mk_tabVar(str, &size);

    if (sat_add_clause(p_formula, lIndCls, tab, size)) {
      fprintf(stderr, " Waouu: Erreur non fatale à l'ajout de la clause.\n");
      fprintf(stderr, "        La clause n'a pas été ajoutée.\n");
    }
    else
		++lIndCls;
	free(tab);

  }
  printf("]\n");
  fclose(fic);
  free(str);
  fprintf(stderr, " Fichier %s fermé.\n", pNom_fic);

  return p_formula;
} // sat_load_file


// Renvoie le pointeur sur une variable, NULL si elle n'existe pas ------------
tVar *sat_ex_var(tGraphe *p_formula, unsigned int pIndVar) {
  tVar *lVar;
  int trouve=0;

  // Initialisation de lVar
  lVar = p_formula->vars;

  // Recherche
  while (lVar && (!trouve)) {
    if (lVar->indVar == pIndVar) trouve = 1;
    else lVar = lVar->suiv;
  }

  if (trouve) {
    fprintf(stderr, "   Littéral x%u trouvé.\n", pIndVar);
    return lVar;
  }
  else {
    fprintf(stderr, "   Littéral x%u non trouvé.\n", pIndVar);
    return NULL;
  }
} // sat_ex_var


/* Crée un lien entre une variable et une clause dans laquelle elle apparait en tenant compte du signe
   et un lien entre ces mêmes clause et variable */
void sat_lnk_clsVar(tClause *pCls, tVar *p_litteral, int pSignVar) {
  tPtVarSgn *ps; // Pointeur vers un pointeur dans clsPos ou clsNeg
  tPtVar    *pv; // Pointeur vers une variable 

  //I) Lien Var --> Clause
  
  // Création de la cellule
  ps = (tPtVarSgn *) malloc(sizeof(tPtVarSgn));

  // Lien de la variable vers la clause
  ps->clause = pCls;

  if (pSignVar == 1) {
    ps->suiv = p_litteral->clsPos;
    p_litteral->clsPos = ps;
  }
  else {
    ps->suiv = p_litteral->clsNeg;
    p_litteral->clsNeg = ps;
  }

  //II) Lien Clause --> Var
  // Création de la cellule
  pv = (tPtVar *) malloc(sizeof(tPtVar));

  // Lien de la clause vers la variable
  pv->var = p_litteral;

  // Chaînage
  pv->suiv = pCls->vars;
  pCls->vars = pv;

  fprintf(stderr, "   Lien clause %u <-> littéral %sx%u créé.\n", pCls->indCls, (pSignVar < 0 ? "¬" : ""), p_litteral->indVar);
} // sat_lnk_varCls


// Test si la variable actuelle du tableau n'a pas déjà été trouvée
int sat_test_prev_var(int *pTabVar, int p_litteral, int pSize) {
  int i;

  for (i=0; i<pSize; ++i) {
    if ( abs(p_litteral) == abs(pTabVar[i]) ) {
      fprintf(stderr, "   Littéral x%u déjà en position %d (%d).\n", abs(p_litteral), i, pTabVar[i]);
      return sat_sign(p_litteral * pTabVar[i]);
    }
  }
  return 0;
} // sat_test_prev_var

// Construit un tableau de variables ------------------------------------------
int *sat_mk_tabVar(char *pStr, int *pSize) {
	int* tab = NULL;
	char* token = NULL;
	int nb_tok=0;
	char *str2, *str1;

	// Sauvegarde de la chaîne originale
	str1 = malloc(LN_SIZE);
	strcpy(str1, pStr);
	str2 = malloc(LN_SIZE);
	strcpy(str2, pStr);

	// I/ Nombre de tokens dans la chaîne
	token = strtok(str1, " ");
	while (token) {
		nb_tok++;
		token = strtok(NULL, " ");
	}
	fprintf(stderr, "  %d tokens trouvés:\n", nb_tok);

	if (nb_tok == 0) {
		free(str2);
		free(str1);
		return NULL;
	}
  
	*pSize = nb_tok;

	// II/ Création du tableau
	tab = (int *) calloc(nb_tok, sizeof(int));

	// III/ Initialisation du tableau
	token = strtok(str2, " ");
	nb_tok = 0;
	while (token) {
		// On teste si l'entier n'apparaît pas déjà dans la variable
		int rslt = sat_test_prev_var(tab, atoi(token), nb_tok);
		if (rslt == 1)  { // Le token apparaît 2 fois avec le même "signe" -> pas ajouté cette fois
			fprintf(stderr, "  Waouu: Le littéral %s a déjà été ajouté.\n", token);
			--*pSize;
		}
		else if (rslt == -1) { // Le token et son contraire apparaîssent -> tab = NIL, pSize = 0;
			fprintf(stderr, "  Waouu: Le littéral %s et son contraire apparaîssent: tableau vide.\n", token);
			free(tab);
			free(str2);
			free(str1);
			*pSize = 0;
			return NULL;
		}
		else {
			tab[nb_tok] = atoi(token);
			fprintf(stderr, "  Ajout de |%d| au rang %d.\n", tab[nb_tok], nb_tok);
			nb_tok++;
		}
		token = strtok(NULL, " ");
	}

	free(str2);
	free(str1);
	return tab;
} // sat_mk_tabVar


// Renvoie le signe d'un entier -----------------------------------------------
int sat_sign(int pNbe) {
  if (pNbe <0) return -1;
  else return 1;
} // sat_sign
 

// Ajoute une variable au graphe ----------------------------------------------
tVar *sat_add_var(tGraphe *p_formula, int pIndVar) {
  tVar *var;

  // Création de var
  var = (tVar *) malloc(sizeof(tVar));

  // Initialisation de var
  var->indVar = abs(pIndVar);
  var->clsPos = NULL;
  var->clsNeg = NULL;
  var->suiv = NULL;
  fprintf(stderr, "   Littéral x%d créé.\n", pIndVar);

  // Chaînage
  var->suiv = p_formula->vars;
  p_formula->vars = var;
  fprintf(stderr, "   Littéral x%d chaîné.\n", pIndVar);

  return var;
} // sat_add_var


// Ajoute une variable à une clause
int sat_add_var_to_cls(tGraphe *p_formula, unsigned int p_clauseId, int p_litteral) {
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
		int* literals = malloc(sizeof(int));
		literals[0] = p_litteral;
		sat_add_clause(p_formula, p_clauseId, literals, 1);
		free(literals);
		return 1;
	}

	fprintf(stderr, "  Clause %u trouvée.\n", clause->indCls);
	// Teste si la variable existe
	tVar* lVar = sat_ex_var(p_formula, abs(p_litteral));
	if (isNull(lVar)) {
		fprintf(stderr, "   Littéral x%d non trouvé: ajout.\n", abs(p_litteral));
		lVar = sat_add_var(p_formula, abs(p_litteral));
	}

	// Lien entre la variable et la clause
	sat_lnk_clsVar(clause, lVar, sat_sign(p_litteral));
	return 0;
} // sat_add_var_to_cls


// Ajoute une clause à un graphe ----------------------------------------------
int sat_add_clause(tGraphe *p_formula, unsigned int p_clauseId, int *pTabVar, int pSize) {
  tClause *clause;
  int *lTabVar;
  int i;

  // Teste si on essaye d'ajouter une clause vide (stupide)
  if (pSize == 0) {
    fprintf(stderr, "  Waouu: Tentative d'ajout de la clause vide.\n         Continue...\n");
    return 1;
  }

  // Construction et Ajout de la clause

  // Création de la clause
  clause = (tClause *)malloc(sizeof(tClause));

  fprintf(stderr, "  Clause %u créée.\n", p_clauseId);

  // Màj de l'indice
  clause->indCls = p_clauseId;
  clause->suiv = NULL;
  clause->vars = NULL;

  // Chaînage des variables
  lTabVar = pTabVar;
  // Parcours des variables de la clause
  for (i=0; i<pSize; i++) {
	tVar *lVar;

    fprintf(stderr, "  Lien clause %u <-> littéral x%d en cours...\n", p_clauseId, lTabVar[i]);

    // Si la variable n'existe pas, l'ajouter.
    if ( !(lVar = sat_ex_var(p_formula, abs(lTabVar[i]))) )
      lVar = sat_add_var(p_formula, lTabVar[i]);
    // Linkage Clause <--> Variable    
    sat_lnk_clsVar(clause, lVar, sat_sign(lTabVar[i]));
    
  }
  fprintf(stderr, "  Clause %u initialisée.\n", p_clauseId);

  // Chaînage des clauses
  clause->suiv = p_formula->clauses;
  p_formula->clauses = clause;

  fprintf(stderr, "  Clause %u chaînée.\n", p_clauseId);

  return 0;
} // sat_add_clause


// Renvoie le signe d'une variable dans une clause ----------------------------
int sat_get_sign(tVar *p_litteral, unsigned int p_clauseId) {
	tPtVarSgn *ps; // Pointeur de parcours des clsPos et clsNeg de tVar

	// Recherche parmis les clauses "positives"
	if (isNull(p_litteral)) {
		fprintf (stderr, "Ooops: le littéral est vide.\n");
		return 1;
	}

	ps = p_litteral->clsPos;
	while (ps) {
		if (ps->clause->indCls == p_clauseId)
			return 1;

		ps = ps->suiv;
	}

	// Si pas trouvé, recherche parmis les "négatifs"
	ps = p_litteral->clsNeg;
	while (ps) {
		if (ps->clause->indCls == p_clauseId)
			return -1;

		ps = ps->suiv;
	}

	// Cas qui ne devrait pas arriver, mais on ne sait jamais...
	return 0;
}


// Affiche les clauses d'un graphe --------------------------------------------
void sat_see(tGraphe* p_formula) {
	// Vérif de p_formula
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
} // sat_see


// Supprime une variable -------------------------------------------
int sat_sub_var(tGraphe *p_formula, unsigned int pIndVar) {
  tVar *lVar, *lVar2;

  // Teste si le graphe est vide
  if (!p_formula) { 
    fprintf(stderr, " Ooops: le graphe est vide, impossible de supprimer un littéral.\n");
    return 1;
  }

  // initialisatin de lVar
  lVar = p_formula->vars;

  // Vérification de lVar
  if (!lVar) {
    fprintf(stderr, " Ooops: la liste des littéraux est vide, impossible d'en supprimer un.\n");
    return 2;
  }

  // Teste si c'est la première variable
  if (lVar->indVar == pIndVar) {
    fprintf(stderr, "  Suppression du littéral x%u.\n", lVar->indVar);
    // Chaînage préc <-> suiv
    p_formula->vars = lVar->suiv;
    // Libération de la variable
    free(lVar);
  }
  else {
    // Recherche parmis les suivantes
    while (lVar->suiv && (lVar->suiv->indVar != pIndVar)) 
      lVar = lVar->suiv;

    // lVar2: élément à supprimer
    lVar2 = lVar->suiv;

    // Vérification de lVar2
    if (!lVar2) {
      fprintf(stderr, "  Ooops: Le littéral x%u n'existe pas dans le graphe.\n", pIndVar);
      return 3;
    }

    fprintf(stderr, "  Suppression du littéral x%u.\n", lVar2->indVar);

    // Chaînage précédant <-> suivant
    lVar->suiv = lVar2->suiv;

    // Libération de la variable
    free(lVar2);
  }
  fprintf(stderr,  "  Littéral x%u supprimé.\n", pIndVar);
  return 0;
} // sat_sub_var


// Supprime une variable dans une clause
int sat_sub_var_in_cls(tPtVar *pPtVar, int pSign, tClause *pCls, tGraphe *p_formula) {
	tPtVar *lPtVar, *lPtVar2;
	tPtVarSgn *lVarSgn;

	// Vérification de pCls
	if (isNull(pCls)) {
		fprintf(stderr, "     Ooops: Le pointeur de clause est NULL.\n");
		return -1;
	}

	// Vérification de p_formula
	if (isNull(p_formula)) {
		fprintf(stderr, "     Ooops: Le pointeur de Graphe est NULL.\n");
		return -2;
	}

	// Premier à supprimer 
	if (pCls->vars == pPtVar) {
		// Défait les liens et tout et tout
		sat_unlnk_cls_var(p_formula, pCls);

		// Codes de retour
		if (isNull(pCls->vars)) {
			fprintf(stderr, "      La clause vide a été produite.\n");
			return 3;
		}

		fprintf(stderr, "       La clause modifiée est non vide.\n");
		return 2;
	}
  // Recherche du pointeur sur la variable
  lPtVar = pCls->vars;
  while (lPtVar->suiv) {
    if (lPtVar->suiv == pPtVar) {
      // Suppr lien Var-> cls
      // Teste le signe et initialise la recherche
      if ((pSign != 1) && (pSign != -1)) {
        fprintf(stderr, "      Ooops: Le signe demandé (%d) est incorrect: 1 -> positif, -1 -> négatif\n", pSign);
        return 1;
      }
      if (pSign ==  1) lVarSgn = lPtVar->suiv->var->clsPos;
      if (pSign == -1) lVarSgn = lPtVar->suiv->var->clsNeg;

      // II/ Suppression du lien Variable -> Clause -------------------------
      if (!lVarSgn) {
	fprintf(stderr, "      Waouu: Le littéral %sx%u n'est pas utilisé.\n", (pSign == -1 ? "¬" : ""), lPtVar->suiv->var->indVar);
	return 2;
      }

      if (lVarSgn->clause == pCls) { // teste si la première clause correspond
	// Chaînage de la précédente avec la suivante
      if (pSign == 1) // Chaînage des positifs
	lPtVar->suiv->var->clsPos = lVarSgn->suiv;
      else lPtVar->suiv->var->clsNeg = lVarSgn->suiv;
      // Libération de la cellule de la variable
      free(lVarSgn);
      fprintf(stderr, "      Lien littéral %sx%u -> clause %u détruit.\n", (pSign == -1 ? "¬" : ""), lPtVar->suiv->var->indVar, pCls->indCls);
      }
      else { // la clause n'est pas la première
	lVarSgn = sat_get_ptr_varSgn(lVarSgn, pCls);
	if (lVarSgn) { // cellule trouvée
	  // Lien préc <-> suiv
	  lVarSgn->suiv = lVarSgn->suiv->suiv;
	  // Libération de la cellule
	  free(lVarSgn->suiv);
	  fprintf(stderr, "      Lien littéral %sx%u -> clause %u détruit.\n", (pSign == -1 ? "¬" : ""), lPtVar->suiv->var->indVar, pCls->indCls);
	}
	else {
	  fprintf(stderr, "      Waouu: Pas de lien littéral %sx%u -> clause %u trouvé.\n", (pSign == -1 ? "¬" : ""), lPtVar->suiv->var->indVar, pCls->indCls);
	  return 3;
	}
      }

      // Sur-lien cls -> var suiv
      lPtVar2 = lPtVar->suiv;
      lPtVar->suiv = lPtVar->suiv->suiv;
      fprintf(stderr, "      Lien clause %u -> littéral x%u détruit.\n", pCls->indCls, lPtVar2->var->indVar);

      // Si la variable est vide, la virer
      if (!lPtVar2->var->clsPos && !lPtVar2->var->clsNeg)
	sat_sub_var(p_formula, lPtVar2->var->indVar);

      // Suppression de la cellule
      free(lPtVar2);

      // Codes de retour
      if (!pCls->vars) {
	fprintf(stderr, "      La clause vide a été produite.\n");
	return 3;
      }
      else {
	fprintf(stderr, "      La clause modifiée est non vide.\n");
	return 2;
      }
    }
    else lPtVar = lPtVar->suiv;
  }

  fprintf(stderr, "      Waouu: La clause n'a pas été modifiée.\n");
  return 4;

} // sat_sub_var_in_cls


// Renvoie le pointeur sur la cellule de la clause pointant sur la clause
tPtVarSgn *sat_get_ptr_varSgn(tPtVarSgn *p_litteralSgn, tClause *pCls) {
	tPtVarSgn *lVarSgn;

	// Vérifie que la clause n'est pas NULL
	if (isNull(pCls)) {
		fprintf(stderr, "    Ooops: La clause n'existe pas.\n");
		return NULL;
	}

	// Vérification de lVarSgn
	if (isNull(p_litteralSgn)) {
		fprintf(stderr, "    Ooops: Pas de lien entre le littéral et une clause");
		return NULL;
	}

	// Recherche
	lVarSgn = p_litteralSgn;

	while (lVarSgn->suiv && (lVarSgn->suiv->clause->indCls != pCls->indCls))
		lVarSgn = lVarSgn->suiv;

	// Si pas de résultat trouvé, retourne NULL, sinon le pointeur voulu
	if (isNull(lVarSgn->suiv))
		return NULL;
	else
		return lVarSgn;
} // sat_get_ptr_varSgn


// Recherche et supprime la cellule de la variable pointant sur la clause
int sat_unlnk_varSgn_cls(tVar *p_litteral, int pSign, tClause *pCls) {
	tPtVarSgn* lVarSgn;
	tPtVar* lPtVar;

	// I/ Tests d'application --------------------------------------------

	// Teste le signe et initialise la recherche
	if ((pSign != 1) && (pSign != -1)) {
		fprintf(stderr, "   Ooops: Le signe demandé (%d) est incorrect: 1 -> positif, -1 -> négatif\n", pSign);
		return 1;
	}
	if (pSign ==  1)
		lVarSgn = p_litteral->clsPos;
	if (pSign == -1)
		lVarSgn = p_litteral->clsNeg;

	// II/ Suppression du lien Variable -> Clause -------------------------
	if (isNull(lVarSgn)) {
		fprintf(stderr, "   Waouu: Le littéral %sx%u n'est pas utilisé.\n", (pSign == -1 ? "¬" : ""), p_litteral->indVar);
		return 2;
	}

	if (lVarSgn->clause->indCls == pCls->indCls) { // teste si la première clause correspond
		// Chaînage de la précédente avec la suivante
		if (pSign == 1) // Chaînage des positifs
			p_litteral->clsPos = lVarSgn->suiv;
		else
			p_litteral->clsNeg = lVarSgn->suiv;
		// Libération de la cellule de la variable
		free(lVarSgn);
		lVarSgn = NULL;
		fprintf(stderr, "   Lien littéral %sx%u -> clause %u détruit.\n", (pSign == -1 ? "¬" : ""), p_litteral->indVar, pCls->indCls); 
	}
	else { // la clause n'est pas la première
		lVarSgn = sat_get_ptr_varSgn(lVarSgn, pCls);
		if (notNull(lVarSgn)) {
			// Lien préc <-> suiv
			tPtVarSgn* toDelete = lVarSgn->suiv;
			lVarSgn->suiv = lVarSgn->suiv->suiv;
			// Libération de la cellule
			free(toDelete);
			fprintf(stderr, "   Lien littéral %sx%u -> clause %u détruit.\n", (pSign == -1 ? "¬" : ""), p_litteral->indVar, pCls->indCls);
		}
		else {
			fprintf(stderr, "   Waouu: Pas de lien littéral %sx%u -> clause %u trouvé.\n", (pSign == -1 ? "¬" : ""), p_litteral->indVar, pCls->indCls);
			return 3;
		}
	}

	// III/ Suppression du lien Clause -> Variable ---------------------
	lPtVar = pCls->vars;
	if (lPtVar->var == p_litteral) {
		pCls->vars = lPtVar->suiv;
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
	fprintf(stderr, "   Lien littéral %sx%u <- clause %u détruit.\n", (pSign == -1 ? "¬" : ""), p_litteral->indVar, pCls->indCls);
	return 0;
} // sat_unlnk_varSgn_cls


// Enlève les liens entre une clause et sa première variable
int sat_unlnk_cls_var(tGraphe *p_formula, tClause *pCls) {
	tVar* lVar;

	// Vérification de la clause
	if (isNull(pCls)) {
		fprintf(stderr, "  Ooops: La clause n'existe pas.\n");
		return 1;
	}

	fprintf(stderr, "  Suppression des liens entre la clause %u et son premier littéral...\n", pCls->indCls);
	// Vérification de la liste des variables de la clause
	if (isNull(pCls->vars)) {
		fprintf(stderr, "  Waouu: La clause %u ne contient pas de littéraux.\n", pCls->indCls);
		return 2;
	}

	// Initialisation de lVar
	lVar = pCls->vars->var;

	// Test de validité de lVar
	if (isNull(lVar)) {
		fprintf(stderr, "  Ooops: Le littéral est NULL.\n");
		return 3;
	}

	// Appels de sat_unlnk_varSgn_cls
	else {
		if (sat_unlnk_varSgn_cls(lVar, 1, pCls) == 0)
			fprintf(stderr, "  Lien littéral x%u <-> clause %u détruit.\n", lVar->indVar, pCls->indCls);
		else if (sat_unlnk_varSgn_cls(lVar, -1, pCls) == 0)
			fprintf(stderr, "  Lien littéral ¬x%u <-> clause %u détruit.\n", lVar->indVar, pCls->indCls);
		else
			fprintf(stderr, "  Waouu: Impossible de trouver le lien littéral x%u -> clause %u.\n", lVar->indVar, pCls->indCls);
	}

	// Après destruction des liens, on teste si on peut supprimer la variable
	if (isNull(lVar->clsPos) && isNull(lVar->clsNeg))
		sat_sub_var(p_formula, lVar->indVar);
  
	return 0;
} // sat_unlnk_cls_var


// Enlève les liens entre une clause et toutes ses variables
int sat_sub_clause(tGraphe *p_formula, unsigned int p_clauseId) {
  tPtVar *ptVars;
  tClause *cls, *cls2;

  fprintf(stderr, "\n Suppression de la clause %u...\n", p_clauseId);
  // Teste si le graphe est vide, auquel cas -> quit !
  if (!p_formula) {
    fprintf(stderr, " Ooops: Aucune clause à supprimer car le graphe est vide !\n");
    return 1;
  }

  // Initialisation
  cls = p_formula->clauses;

  // Test de validité
  if (!cls) {
    fprintf(stderr, " Ooops: Il n'y a pas de clause dans ce graphe !\n");
    return 2;
  }
  
  // Si c'est la première clause ...
  if (cls->indCls == p_clauseId) {
    
    // Destruction des liens
    ptVars = cls->vars;
    while (ptVars) {
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
    while (cls->suiv && (cls->suiv->indCls != p_clauseId)) 
      cls = cls->suiv;

    cls2 = cls->suiv;

    // On retire les liens
    ptVars = cls2->vars;
    while (ptVars) {
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
} // sat_sub_clause


// Renvoie la variable de la première clause unitaire trouvée
//  0 si non trouvée
int sat_get_var_cls_unit(tGraphe *p_formula) {
  tClause *clause;
  tVar    *lVar;
  int      rslt;

  // Vérification de p_formula
  if (!p_formula) {
    fprintf(stderr, "  Ooops: Le pointeur de graphe est NULL.\n");
    return 1;
  }

  // Recherche...
  clause = p_formula->clauses;

  while (clause) {
    if (!clause->vars) {
      fprintf(stderr, "  Waouu: La clause %u ne contient pas de littéraux.\n", clause->indCls);
    }
    else if (!clause->vars->suiv) {
      rslt = sat_get_sign(clause->vars->var, clause->indCls) * clause->vars->var->indVar;
	  fprintf(stderr, "  La clause n°%u (%sx%d) est unitaire.\n", clause->indCls, (rslt < 0 ? "¬" : ""), abs(rslt));
      return rslt;
    }
    else {
		fprintf(stderr, "  La clause n°%u n'est pas unitaire.\n", clause->indCls);
      clause = clause->suiv;
    }
  }

  // Pas de clause unitaire trouvée
  if (!p_formula->vars) {
    fprintf(stderr, "  Ooops: Plus de littéraux dans le graphe.\n");
    return 0;
  }
  lVar = p_formula->vars;
  if (!lVar->clsPos) {
    if (!lVar->clsNeg) {
      fprintf(stderr, "  Ooops: Le littéral x%u n'est liée à aucune clause.\n", lVar->indVar);
      return 0;
    }
    else {
      return -lVar->indVar;
    }
  }
else return lVar->indVar;
} // sat_get_var_cls_unit

// FIN DE libSat
