/* EN-têtes de la libHIST -- Librairie de gestion d'historiques de graphes SAT
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
#ifndef HISTORY_H
#define HISTORY_H

#include "sat.h"

typedef struct tEtape tEtape;

// Etape
struct tEtape {
  int           op;     // Opération réalisée: 1->sub_cls 2->sub_var_in_cls
  unsigned int  indCls; // Indice de la clause concernée
  int          *vars;   // Tableau de littéraux
  int           size;   // Taille du tableau
  tEtape       *suiv;   // Ptr sur suivant
};

// Historique
typedef struct {
  tEtape *deb; // Début de liste
} tHist;


// Affiche le n° de version de la lib et +
void hist_vers();

// Crée un historique
tHist *hist_mk();

// Libère un historique
int hist_free(tHist **pHist);

// Teste si l'historique est vide
int hist_void(tHist *pHist);

// Ajoute une étape 1 en tête
int hist_add_cls(tHist *pHist, tClause *pCls);

// Ajoute une étape 2 en tête
int hist_add_var(tHist *pHist, int pIndCls, int pVar);

// Supprime la première étape
int hist_rm(tHist *pHist);

// Renvoie le code de première opération
int hist_get_code(tHist *pHist);

// Renvoie l'indice de la première opération
int hist_get_cls(tHist *pHist);

// Renvoie le tableau de variables de la première opération
int *hist_get_vars(tHist *pHist);

// Renvoie la taille du dernier tableau ajouté
int hist_get_size(tHist *pHist);

// Ré-effectue les modifications de l'historique
int hist_redo(tHist *pHist, tGraphe **pGraph);

#endif // HISTORY_H
