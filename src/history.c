/* Fonctions de la libHIST -- Librairie de gestion d'historiques de graphes SAT
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
#include "history.h"

#include <stdio.h>
#include <stdlib.h>


/**
 * Creates a new history.
 */
tHist *hist_new() {
	tHist *history;

	history = (tHist *) malloc(sizeof(tHist));
	history->deb = NULL;

	return history;
} // hist_new


// Libère un historique
int hist_free(tHist **p_history) {
  tEtape *etape;

  // Vérification de p_history
  if (!(*p_history)) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Libération de l'historique...
  etape = (*p_history)->deb;
  while (etape) {
    hist_rm(*p_history);
    etape = (*p_history)->deb;
  }
  free(*p_history);
  (*p_history) = NULL;

  return 0;
} // hist_free


// Teste si l'historique est vide
int hist_void(tHist *p_history) {

  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  return (p_history->deb == NULL);
} // hist_void

// Ajoute une étape code 1 en tête
int hist_add_cls(tHist *p_history, tClause *p_clause) {
  // Parameters check
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  if (!p_clause) {
    fprintf(stderr, " Ooops: Le pointeur de clause est NULL.\n");
    return -2;
  }

  // Vérifie que la clause est non vide
  if (!p_clause->vars) {
    fprintf(stderr, " Waouu: La clause à ajouter est vide.\n");
    return -3;
  }

  // Création de etape
  tEtape* etape = (tEtape *) malloc(sizeof(tEtape));

  // Mise à jour
  etape->op = OP_REMOVE_CLAUSE;
  etape->indCls = p_clause->indCls;

  // Recherche du nombre de variables
  int nb_var_in_cls = 0;
  tPtVar* lPtVar = p_clause->vars;
  while (lPtVar) {
    nb_var_in_cls++;
    lPtVar = lPtVar->suiv;
  }

  // Création du tableau de variables;
  Literal* literals = (int *) calloc(nb_var_in_cls, sizeof(int));

  // Initialisation du tableau
  int i = 0;
  lPtVar = p_clause->vars;
  while (lPtVar) {
    literals[i++] = lPtVar->var->indVar * sat_get_sign(lPtVar->var, p_clause->indCls);
    lPtVar = lPtVar->suiv;
  }
  // Ajout du tableau
  etape->vars = literals;
  etape->size = nb_var_in_cls;

  // Chaînage
  etape->suiv = p_history->deb;

  // Ajout en tête
  p_history->deb = etape;
  return 0;
} // hist_add_cls


// Ajoute une étape 2 en tête
int hist_add_var(tHist *p_history, int p_clauseId, Literal p_literal) {
  // Parameters check
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Création de etape
  tEtape* etape = (tEtape *) malloc(sizeof(tEtape));

  // Mise à jour
  etape->op = OP_REMOVE_LITERAL_FROM_CLAUSE;
  etape->indCls = p_clauseId;

  // Création du tableau de variables;
  Literal* literals = (int *) malloc(sizeof(int));
  literals[0] = p_literal;

  // Ajout du tableau
  etape->vars = literals;
  etape->size = 1;

  // Chaînage
  etape->suiv = p_history->deb;

  // Ajout en tête
  p_history->deb = etape;
  return 0;
} // hist_add_var


// Supprime la première étape
int hist_rm(tHist *p_history) {
  tEtape *etape;

  // Vérif de p_history;
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de taille
  if (!p_history->deb) {
    fprintf(stderr, " Waouu: L'historique est NULL.\n");
    return -2;
  }

  etape = p_history->deb;

  // MàJ du ptr de début de liste
  p_history->deb = etape->suiv;

  // On libère le tableau de variables
  free(etape->vars);

  // On libère etape
  free(etape);

  return 0;
} // hist_rm


// Renvoie le code de première opération
Operation hist_get_code(tHist *p_history) {

  // Vérif de p_history
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de hist non vide
  if (!p_history->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne le code de la dernière opération effectuée
  return p_history->deb->op;
} // hist_get_code


// Renvoie l'indice de la première opération
int hist_get_cls(tHist *p_history) {

  // Vérif de p_history
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de hist non vide
  if (!p_history->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne l'indice de la dernière clause modifiée
  return p_history->deb->indCls;
} // hist_get_cls


// Renvoie le tableau de variables de la première opération
int *hist_get_vars(tHist *p_history) {

  // Vérif de p_history
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return NULL;
  }

  // Vérif de hist non vide
  if (!p_history->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return NULL;
  }

  // Retourne la liste des variables de la dernière clause modifiée
  return p_history->deb->vars;
} // hist_get_vars


// Renvoie la taille du dernier tableau ajouté
int hist_get_size(tHist *p_history) {

  // Vérif de p_history
  if (!p_history) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de hist non vide
  if (!p_history->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne la taille du tab
  return p_history->deb->size;
} // hist_get_size


// Ré-effectue les modifications de l'historique
int hist_redo(tHist *p_history, tGraphe **pGraph) {
  int var;

  // Vérif de p_history
  if (!p_history) {
    fprintf(stderr, "    Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  fprintf(stderr, "\n");

  if (!(*pGraph)) {
    fprintf(stderr, "    Waouu: Le graphe est vide.\n");
    fprintf(stderr, "    Initialisation...\n");
    (*pGraph) = sat_new();
  }

  fprintf(stderr, "    Reconstruction...\n");
  while (!hist_void(p_history)) {
    switch (hist_get_code(p_history)) {
    case OP_REMOVE_CLAUSE: // Ajouter une clause
      fprintf(stderr, "\n    Ajout de la clause n°%d.\n\n", hist_get_cls(p_history));
      sat_add_clause(*pGraph, hist_get_cls(p_history), hist_get_vars(p_history), hist_get_size(p_history));
      break;
    case OP_REMOVE_LITERAL_FROM_CLAUSE: // Ajouter une variable à une clause
      fprintf(stderr, "\n    Ajout de ");
      var = (*hist_get_vars(p_history));
      if (var < 0) fprintf(stderr, "¬");
      fprintf(stderr, "X%d à la clause n°%d...\n\n", abs(var), hist_get_cls(p_history));
      sat_add_var_to_cls(*pGraph, hist_get_cls(p_history), var);
      break;
    default:
      fprintf(stderr, "\n     Waouu: Une opération non conforme a été enregistrée dans l'historique.\n");
      fprintf(stderr, "            Continue...\n");
    }
    hist_rm(p_history);
  }
  //  fprintf(stderr, "\n    Graphe reconstruit:\n");
  //  sat_see(*pGraph);
  return 0;
} // hist_redo


// FIN DE libHist
