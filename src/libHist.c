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
#include "libHist.h"

#include <stdio.h>
#include <stdlib.h>
#include "libSat.h"

#define LIBHIST_VERS "1.0.1"
#define LIBHIST_NAME "libHIST"
#define LIBHIST_AUTH "Olivier Serve & Mickaël Sibelle & Philippe Strelezki"


// Affiche le n° de version de l'unité et +
void hist_vers() {
  printf("\n   %s --- Gestion d'historiques --- v%s\n", LIBHIST_NAME, LIBHIST_VERS);
  printf(" Cette librairie est un logiciel libre. Elle est fournie sans\n");
  printf(" AUCUNE GARANTIE. Consultez le fichier COPYING pour plus de\n");
  printf(" détails sur la licence GPL.\n\n");
  printf(" Auteur(s): %s\n\n", LIBHIST_AUTH);
} // hist_vers



// Crée un historique
tHist *hist_mk() {
  tHist *lHist;

  lHist = (tHist *) malloc(sizeof(tHist));
  lHist->deb = NULL;

  return lHist;
} // hist_mk


// Libère un historique
int hist_free(tHist **pHist) {
  tEtape *lEtape;

  // Vérification de pHist
  if (!(*pHist)) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Libération de l'historique...
  lEtape = (*pHist)->deb;
  while (lEtape) {
    hist_rm(*pHist);
    lEtape = (*pHist)->deb;
  }
  free(*pHist);
  (*pHist) = NULL;

  return 0;
} // hist_free


// Teste si l'historique est vide
int hist_void(tHist *pHist) {

  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  return (pHist->deb == NULL);
} // hist_void

// Ajoute une étape code 1 en tête
int hist_add_cls(tHist *pHist, tClause *pCls) {
  tPtVar *lPtVar;
  tEtape *lEtape;
  int    *lVars;
  int nb_var_in_cls=0, i=0;

  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de pCls
  if (!pCls) {
    fprintf(stderr, " Ooops: Le pointeur de clause est NULL.\n");
    return -2;
  }

  // Vérifie que la clause est non vide
  if (!pCls->vars) {
    fprintf(stderr, " Waouu: La clause à ajouter est vide.\n");
    return -3;
  }

  // Création de lEtape
  lEtape = (tEtape *) malloc(sizeof(tEtape));

  // Mise à jour
  lEtape->op = 1;
  lEtape->indCls = pCls->indCls;

  // Recherche du nombre de variables
  lPtVar = pCls->vars;
  while (lPtVar) {
    nb_var_in_cls++;
    lPtVar = lPtVar->suiv;
  }

  // Création du tableau de variables;
  lVars = (int *) calloc(nb_var_in_cls, sizeof(int));

  // Initialisation du tableau
  lPtVar = pCls->vars;
  while (lPtVar) {
    lVars[i++] = lPtVar->var->indVar * sat_get_sign(lPtVar->var, pCls->indCls);
    lPtVar = lPtVar->suiv;
  }
  // Ajout du tableau
  lEtape->vars = lVars;
  lEtape->size = nb_var_in_cls;

  // Chaînage
  lEtape->suiv = pHist->deb;

  // Ajout en tête
  pHist->deb = lEtape;
  return 0;
} // hist_add_cls


// Ajoute une étape 2 en tête
int hist_add_var(tHist *pHist, int pIndCls, int pVar) {
  tEtape *lEtape;
  int    *lVars;
  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Création de lEtape
  lEtape = (tEtape *) malloc(sizeof(tEtape));

  // Mise à jour
  lEtape->op = 2;
  lEtape->indCls = pIndCls;

  // Création du tableau de variables;
  lVars = (int *) malloc(sizeof(int));
  lVars[0] = pVar;

  // Ajout du tableau
  lEtape->vars = lVars;
  lEtape->size = 1;

  // Chaînage
  lEtape->suiv = pHist->deb;

  // Ajout en tête
  pHist->deb = lEtape;
  return 0;
} // hist_add_var


// Supprime la première étape
int hist_rm(tHist *pHist) {
  tEtape *lEtape;

  // Vérif de pHist;
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de taille
  if (!pHist->deb) {
    fprintf(stderr, " Waouu: L'historique est NULL.\n");
    return -2;
  }

  lEtape = pHist->deb;

  // MàJ du ptr de début de liste
  pHist->deb = lEtape->suiv;

  // On ne libère pas le tableau de variables
  // free(lEtape->vars);

  // On libère lEtape
  free(lEtape);

  return 0;
} // hist_rm


// Renvoie le code de première opération
int hist_get_code(tHist *pHist) {

  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne le code de la dernière opération effectuée
  return pHist->deb->op;
} // hist_get_code


// Renvoie l'indice de la première opération
int hist_get_cls(tHist *pHist) {

  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne l'indice de la dernière clause modifiée
  return pHist->deb->indCls;
} // hist_get_cls


// Renvoie le tableau de variables de la première opération
int *hist_get_vars(tHist *pHist) {

  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return NULL;
  }

  // Vérif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return NULL;
  }

  // Retourne la liste des variables de la dernière clause modifiée
  return pHist->deb->vars;
} // hist_get_vars


// Renvoie la taille du dernier tableau ajouté
int hist_get_size(tHist *pHist) {

  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Vérif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne la taille du tab
  return pHist->deb->size;
} // hist_get_size


// Ré-effectue les modifications de l'historique
int hist_redo(tHist *pHist, tGraphe **pGraph) {
  int var;

  // Vérif de pHist
  if (!pHist) {
    fprintf(stderr, "    Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  fprintf(stderr, "\n");

  if (!(*pGraph)) {
    fprintf(stderr, "    Waouu: Le graphe est vide.\n");
    fprintf(stderr, "    Initialisation...\n");
    (*pGraph) = sat_mk();
  }

  fprintf(stderr, "    Reconstruction...\n");
  while (!hist_void(pHist)) {
    switch (hist_get_code(pHist)) {
    case 1: // Ajouter une clause
      fprintf(stderr, "\n    Ajout de la clause n°%d.\n\n", hist_get_cls(pHist));
      sat_add_clause(*pGraph, hist_get_cls(pHist), hist_get_vars(pHist), hist_get_size(pHist));
      break;
    case 2: // Ajouter une variable à une clause
      fprintf(stderr, "\n    Ajout de ");
      var = (*hist_get_vars(pHist));
      if (var < 0) fprintf(stderr, "¬");
      fprintf(stderr, "X%d à la clause n°%d...\n\n", abs(var), hist_get_cls(pHist));
      sat_add_var_to_cls(*pGraph, hist_get_cls(pHist), var);
      break;
    default:
      fprintf(stderr, "\n     Waouu: Une opération non conforme a été enregistrée dans l'historique.\n");
      fprintf(stderr, "            Continue...\n");
    }
    hist_rm(pHist);
  }
  //  fprintf(stderr, "\n    Graphe reconstruit:\n");
  //  sat_see(*pGraph);
  return 0;
} // hist_redo


// FIN DE libHist
