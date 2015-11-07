/* Fonctions de la libHIST -- Librairie de gestion d'historiques de graphes SAT
   Copyright (C) 2002 Olivier Serve, Micka�l Sibelle & Philippe Strelezki

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
#define LIBHIST_AUTH "Olivier Serve & Micka�l Sibelle & Philippe Strelezki"


// Affiche le n� de version de l'unit� et +
void hist_vers() {
  printf("\n   %s --- Gestion d'historiques --- v%s\n", LIBHIST_NAME, LIBHIST_VERS);
  printf(" Cette librairie est un logiciel libre. Elle est fournie sans\n");
  printf(" AUCUNE GARANTIE. Consultez le fichier COPYING pour plus de\n");
  printf(" d�tails sur la licence GPL.\n\n");
  printf(" Auteur(s): %s\n\n", LIBHIST_AUTH);
} // hist_vers



// Cr�e un historique
tHist *hist_mk() {
  tHist *lHist;

  lHist = (tHist *) malloc(sizeof(tHist));
  lHist->deb = NULL;

  return lHist;
} // hist_mk


// Lib�re un historique
int hist_free(tHist **pHist) {
  tEtape *lEtape;

  // V�rification de pHist
  if (!(*pHist)) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Lib�ration de l'historique...
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

// Ajoute une �tape code 1 en t�te
int hist_add_cls(tHist *pHist, tClause *pCls) {
  tPtVar *lPtVar;
  tEtape *lEtape;
  int    *lVars;
  int nb_var_in_cls=0, i=0;

  // V�rif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // V�rif de pCls
  if (!pCls) {
    fprintf(stderr, " Ooops: Le pointeur de clause est NULL.\n");
    return -2;
  }

  // V�rifie que la clause est non vide
  if (!pCls->vars) {
    fprintf(stderr, " Waouu: La clause � ajouter est vide.\n");
    return -3;
  }

  // Cr�ation de lEtape
  lEtape = (tEtape *) malloc(sizeof(tEtape));

  // Mise � jour
  lEtape->op = 1;
  lEtape->indCls = pCls->indCls;

  // Recherche du nombre de variables
  lPtVar = pCls->vars;
  while (lPtVar) {
    nb_var_in_cls++;
    lPtVar = lPtVar->suiv;
  }

  // Cr�ation du tableau de variables;
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

  // Cha�nage
  lEtape->suiv = pHist->deb;

  // Ajout en t�te
  pHist->deb = lEtape;
  return 0;
} // hist_add_cls


// Ajoute une �tape 2 en t�te
int hist_add_var(tHist *pHist, int pIndCls, int pVar) {
  tEtape *lEtape;
  int    *lVars;
  // V�rif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // Cr�ation de lEtape
  lEtape = (tEtape *) malloc(sizeof(tEtape));

  // Mise � jour
  lEtape->op = 2;
  lEtape->indCls = pIndCls;

  // Cr�ation du tableau de variables;
  lVars = (int *) malloc(sizeof(int));
  lVars[0] = pVar;

  // Ajout du tableau
  lEtape->vars = lVars;
  lEtape->size = 1;

  // Cha�nage
  lEtape->suiv = pHist->deb;

  // Ajout en t�te
  pHist->deb = lEtape;
  return 0;
} // hist_add_var


// Supprime la premi�re �tape
int hist_rm(tHist *pHist) {
  tEtape *lEtape;

  // V�rif de pHist;
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // V�rif de taille
  if (!pHist->deb) {
    fprintf(stderr, " Waouu: L'historique est NULL.\n");
    return -2;
  }

  lEtape = pHist->deb;

  // M�J du ptr de d�but de liste
  pHist->deb = lEtape->suiv;

  // On ne lib�re pas le tableau de variables
  // free(lEtape->vars);

  // On lib�re lEtape
  free(lEtape);

  return 0;
} // hist_rm


// Renvoie le code de premi�re op�ration
int hist_get_code(tHist *pHist) {

  // V�rif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // V�rif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne le code de la derni�re op�ration effectu�e
  return pHist->deb->op;
} // hist_get_code


// Renvoie l'indice de la premi�re op�ration
int hist_get_cls(tHist *pHist) {

  // V�rif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // V�rif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne l'indice de la derni�re clause modifi�e
  return pHist->deb->indCls;
} // hist_get_cls


// Renvoie le tableau de variables de la premi�re op�ration
int *hist_get_vars(tHist *pHist) {

  // V�rif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return NULL;
  }

  // V�rif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return NULL;
  }

  // Retourne la liste des variables de la derni�re clause modifi�e
  return pHist->deb->vars;
} // hist_get_vars


// Renvoie la taille du dernier tableau ajout�
int hist_get_size(tHist *pHist) {

  // V�rif de pHist
  if (!pHist) {
    fprintf(stderr, " Ooops: Le pointeur d'historique est NULL.\n");
    return -1;
  }

  // V�rif de hist non vide
  if (!pHist->deb) {
    fprintf(stderr, " Ooops: L'historique est vide.\n");
    return -2;
  }

  // Retourne la taille du tab
  return pHist->deb->size;
} // hist_get_size


// R�-effectue les modifications de l'historique
int hist_redo(tHist *pHist, tGraphe **pGraph) {
  int var;

  // V�rif de pHist
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
      fprintf(stderr, "\n    Ajout de la clause n�%d.\n\n", hist_get_cls(pHist));
      sat_add_clause(*pGraph, hist_get_cls(pHist), hist_get_vars(pHist), hist_get_size(pHist));
      break;
    case 2: // Ajouter une variable � une clause
      fprintf(stderr, "\n    Ajout de ");
      var = (*hist_get_vars(pHist));
      if (var < 0) fprintf(stderr, "�");
      fprintf(stderr, "X%d � la clause n�%d...\n\n", abs(var), hist_get_cls(pHist));
      sat_add_var_to_cls(*pGraph, hist_get_cls(pHist), var);
      break;
    default:
      fprintf(stderr, "\n     Waouu: Une op�ration non conforme a �t� enregistr�e dans l'historique.\n");
      fprintf(stderr, "            Continue...\n");
    }
    hist_rm(pHist);
  }
  //  fprintf(stderr, "\n    Graphe reconstruit:\n");
  //  sat_see(*pGraph);
  return 0;
} // hist_redo


// FIN DE libHist
