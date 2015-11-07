/* En-têtes de la libINTR -- Librairie de gestion d'interprétations
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
#ifndef LIBINTR_H
#define LIBINTR_H

typedef struct tLitt tLitt;

// Littéral
struct tLitt {
  int    litt;  // Valeur dans la liste
  tLitt *suiv; // Ptr sur le suivant
};

// Interprétation
typedef struct {
  int insatisfiable; // Si 1, insatisfiable; sinon satisfiable
  tLitt *deb; // ptr sur début de liste
  tLitt *fin; // ptr sur fin de liste
} tIntr;


// Affiche le n° de version de la lib et +
void intr_vers();

// Crée une liste
tIntr *intr_mk();

// Libère une liste
int intr_free(tIntr **pIntr);

// Teste si une liste est vide
int intr_is_void(tIntr *pIntr);

// Ajoute un élément en tête
int intr_add(tIntr *pIntr, int n);

// Supprime le dernier élément
int intr_rm(tIntr **pIntr);

// Renvoie le premier élément
int intr_get_first(tIntr *pIntr);

// Teste si l est insatisfiable
int intr_is_insatisfiable(tIntr *pIntr);

// Positionne l à insatisfiable
int intr_set_insatisfiable(tIntr **pIntr);

// Affiche une interprétation
void intr_see(tIntr *pIntr);

#endif
