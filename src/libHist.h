/* EN-t�tes de la libHIST -- Librairie de gestion d'historiques de graphes SAT
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
#ifndef LIBHIST_H
#define LIBHIST_H

#include "libSat.h"

typedef struct tEtape tEtape;

// Etape
struct tEtape {
  int           op;     // Op�ration r�alis�e: 1->sub_cls 2->sub_var_in_cls
  unsigned int  indCls; // Indice de la clause concern�e
  int          *vars;   // Tableau de litt�raux
  int           size;   // Taille du tableau
  tEtape       *suiv;   // Ptr sur suivant
};

// Historique
typedef struct {
  tEtape *deb; // D�but de liste
} tHist;


// Affiche le n� de version de la lib et +
void hist_vers();

// Cr�e un historique
tHist *hist_mk();

// Lib�re un historique
int hist_free(tHist **pHist);

// Teste si l'historique est vide
int hist_void(tHist *pHist);

// Ajoute une �tape 1 en t�te
int hist_add_cls(tHist *pHist, tClause *pCls);

// Ajoute une �tape 2 en t�te
int hist_add_var(tHist *pHist, int pIndCls, int pVar);

// Supprime la premi�re �tape
int hist_rm(tHist *pHist);

// Renvoie le code de premi�re op�ration
int hist_get_code(tHist *pHist);

// Renvoie l'indice de la premi�re op�ration
int hist_get_cls(tHist *pHist);

// Renvoie le tableau de variables de la premi�re op�ration
int *hist_get_vars(tHist *pHist);

// Renvoie la taille du dernier tableau ajout�
int hist_get_size(tHist *pHist);

// R�-effectue les modifications de l'historique
int hist_redo(tHist *pHist, tGraphe **pGraph);

#endif
