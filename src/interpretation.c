/* Fonctions de la libINTR -- Librairie de gestion d'interprétations
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
#include "interpretation.h"

#include <stdio.h>
#include <stdlib.h>

#define LIBINTR_VERS "1.2"
#define LIBINTR_NAME "libINTR"
#define LIBINTR_AUTH "Olivier Serve & Mickaël Sibelle & Philippe Strelezki"


// Affiche le n° de version de l'unité et +
void intr_vers() {
  printf("\n   %s --- Gestion d'interprétations --- v%s\n", LIBINTR_NAME, LIBINTR_VERS);
  printf(" Cette librairie est un logiciel libre. Elle est fournie sans\n");
  printf(" AUCUNE GARANTIE. Consultez le fichier COPYING pour plus de\n");
  printf(" détails sur la licence GPL.\n\n");
  printf(" Auteur(s): %s\n\n", LIBINTR_AUTH);
} // intr_vers


// Crée une liste
tIntr *intr_mk() {
  tIntr *lIntr;

  // Création de la liste
  lIntr = (tIntr *) malloc(sizeof(tIntr));

  // Initialisation
  lIntr->insatisfiable = 0;
  lIntr->deb = NULL;
  lIntr->fin = NULL;

  return lIntr;
} // intr_mk


// Libère une linterprétation
int intr_free(tIntr **pIntr) {
  tLitt *e;

  // Vérification de l
  if (!(*pIntr)) {
    fprintf(stderr, " Ooops: Le pointeur de liste est NULL.\n");
    return -1;
  }

  // Suppression des éléments
  e = (*pIntr)->deb;
  while (e) {
    intr_rm(pIntr);
    e = (*pIntr)->deb;
  }

  free(*pIntr);
  *pIntr = NULL;
  return 0;
} // intr_free


// Teste si une liste est vide
int intr_is_void(tIntr *pIntr) {

  // Vérification de pIntr
  if (!pIntr) {
    fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
    return -1;
  }

  if (pIntr->insatisfiable) {
    fprintf(stderr, " Insatisfiable.\n");
    return 2;
  }
  else return (!pIntr->deb);
} // intr_is_void


// Ajoute un élément en tête
int intr_add(tIntr *pIntr, int n) {
  tLitt *e;

  // Vérification de pIntr
  if (!pIntr) {
    fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
    return -1;
  }

  // Création de e
  e = (tLitt *) malloc(sizeof(tLitt));

  // Initialisation de e
  e->litt = n;
  e->suiv = NULL;

  if (!pIntr->deb) {
    // Si vide
    pIntr->deb = e;
    pIntr->fin = e;
  }
  else {
  // Ptr de pIntr
  pIntr->fin->suiv = e;
  pIntr->fin = e;
  }

  if (pIntr->insatisfiable == 1) {
    fprintf(stderr, " Waouu: L'interprétation était insatisfiable.\n");
    pIntr->insatisfiable = 0;
    fprintf(stderr, "        Elle est maintenant satisfiable");
    return 1;
  }
  return 0;
} // intr_add


// Supprime le premier élément
int intr_rm(tIntr **pIntr) {
  tLitt *e, *e2;

  // Vérification de l
  if (!(*pIntr)) {
    fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
    return -1;
  }

  // Vérification de la taille
  if (!(*pIntr)->deb) {
    fprintf(stderr, " Waouu: L'interprétation est vide: rien à supprimer.\n");
    return -2;
  }
 
  if ((*pIntr)->deb == (*pIntr)->fin) {
    e = (*pIntr)->deb;
    (*pIntr)->deb = NULL;
    (*pIntr)->fin = NULL;
    free(e);
  }
  else {
    e = (*pIntr)->fin;
    e2 = (*pIntr)->deb;
    while (e2->suiv != e)
      e2 = e2->suiv;
    (*pIntr)->fin = e2;
    free(e);
  }
  return 0;
} // intr_rm


// Renvoie le premier élément
int intr_get_first(tIntr *pIntr) {

  // Vérification de pIntr
  if (!pIntr) {
    fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
    return 0;
  }
  return pIntr->deb->litt;
} // intr_get_first


// Teste si l est insatisfiable
int intr_is_insatisfiable(tIntr *pIntr) {

  // Vérification de l
  if (!pIntr) {
    fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
    return -1;
  }

  return pIntr->insatisfiable;
} // intr_is_insatisfiable


// Positionne l à insatisfiable
int intr_set_insatisfiable(tIntr **pIntr) {/* passer en double pointeurs */

  // Vérification de pIntr
  if (!(*pIntr)) {
    fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
    return -1;
  }

  (*pIntr)->insatisfiable = 1;
  return 0;
} // intr_set_insatisfiable


// Affiche une interprétation
void intr_see(tIntr *pIntr) {
  tLitt *lLitt;

  // Vérification de pIntr;
  if (!pIntr) {
    fprintf(stderr, "  Ooops: Le pointeur d'interprétation est NULL.\n");
    exit(1);
  }

  // Teste si insatisfiable
  if (pIntr->insatisfiable == 1) printf("  L'interprétation est insatisfiable.\n");
  else {
    // Parcours de la liste des littéraux...
    printf("  Interprétation = (");
    lLitt = pIntr->deb;
    while (lLitt) {
      printf(" ");
      if (lLitt->litt < 0) printf("¬");
      printf("X%d", abs(lLitt->litt));
      lLitt = lLitt->suiv;
    }
    printf(" )\n");
  }

} // intr_see

// FIN DE libList
