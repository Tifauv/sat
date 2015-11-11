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

#include "literal.h"
#include "sat.h"

#define OP_REMOVE_CLAUSE              1
#define OP_REMOVE_LITERAL_FROM_CLAUSE 2

typedef unsigned int Operation;
typedef struct tEtape tEtape;

// Etape
struct tEtape {
  Operation     op;     // Opération réalisée: OP_REMOVE_CLAUSE ou OP_REMOVE_LITERAL_FROM_CLAUSE
  unsigned int  indCls; // Indice de la clause concernée
  Literal*      vars;   // Tableau de littéraux
  int           size;   // Taille du tableau
  tEtape       *suiv;   // Ptr sur suivant
};

// Historique
typedef struct {
  tEtape *deb; // Début de liste
} tHist;


// Crée un historique
tHist *hist_new();

// Libère un historique
int hist_free(tHist **pHist);

// Teste si l'historique est vide
int hist_void(tHist *pHist);

// Ajoute une étape 1 en tête
int hist_add_cls(tHist *pHist, tClause *pCls);

// Ajoute une étape 2 en tête
int hist_add_var(tHist *pHist, int pIndCls, Literal pVar);

// Supprime la première étape
int hist_rm(tHist *pHist);

// Renvoie le code de première opération
Operation hist_get_code(tHist *pHist);

// Renvoie l'indice de la première opération
int hist_get_cls(tHist *pHist);

// Renvoie le tableau de variables de la première opération
Literal* hist_get_vars(tHist *pHist);

// Renvoie la taille du dernier tableau ajouté
int hist_get_size(tHist *pHist);

// Ré-effectue les modifications de l'historique
int hist_redo(tHist *pHist, tGraphe **pGraph);

#endif // HISTORY_H
