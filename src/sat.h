/* En-têtes de la libSAT -- Librairie de gestion de graphe SAT
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
#ifndef SAT_H
#define SAT_H

typedef struct tVar tVar;           // Type Liste de variables

typedef struct tPtVarSgn tPtVarSgn; // Type Liste de pointeurs (indiquant le signe d'une variable dans une clause)

typedef struct tClause tClause;     // Type Liste de clauses

typedef struct tPtVar tPtVar;       // Type Liste de pointeurs vers une variable contenue dans une clause

// Type Liste de clauses
struct tClause {
   unsigned int indCls; // Indice de la clause : entier non-signé (0 -> 255)
   tPtVar       *vars  ; // Pointeur sur la liste des variables de la clause
   tClause      *suiv  ; // Pointeur chaînant
};

// Type Liste de variables
struct tVar {
   unsigned int  indVar; // Indice de la variable
   tPtVarSgn     *clsPos; // Pointeur vers les clauses dans lesquelles la variable est positive
   tPtVarSgn     *clsNeg; // Pointeur vers les clauses dans lesquelles la variable est négative
   tVar          *suiv  ; // Pointeur chaînant
};

// Type Liste de pointeurs vers une variable contenue dans une clause
struct tPtVar {
   tVar   *var ; // Pointeur vers une variable de la clause
   tPtVar *suiv; // Pointeur chaînant
};

// Type Liste de pointeurs (indiquant le signe d'une variable dans une clause)
struct tPtVarSgn {
   tClause   *clause; // Pointeur vers une clause qui utilise la variable positivement
   tPtVarSgn *suiv  ; // Pointeur chaînant
};

// Type Graphe
typedef struct tGraphe {
   tClause *clauses; // Pointeur vers la liste des clauses
   tVar    *vars   ; // Pointeur vers la liste des variables
} tGraphe;

// Libère la mémoire occupée par le graphe
void sat_free(tGraphe **pGraph);

// Renvoie un graphe vide
tGraphe *sat_mk();

// Charge un fichier dans un graph vide à l'origine
tGraphe *sat_load_file(char *pNom_fic);

// Teste si une variable appartient à une clause --------------------------
int sat_ex_var_in_cls(int pVar, tClause *pCls);

// Compare une clause et un tableau de variables
int sat_compare_cls_tabVar(tClause *pCls, unsigned int pIndCls, int *pTabVar, int pSize);

// Teste si une clause existe
int sat_ex_clause(tGraphe *pGraph, unsigned int pIndCls, int *pTabVar, int pSize);

// Teste si une variable existe
tVar *sat_ex_var(tGraphe *pGraph, unsigned int pIndVar);

// Lie une variable à une clause et inversement
void sat_lnk_clsVar(tClause *pCls, tVar *pVar, int pSignVar);

// Construit un tableau de variables
int *sat_mk_tabVar(char *pStr, int *pSize);

// Renvoie le signe d'un entier
int sat_sign(int pNbe);

// Crée et ajoute une variable à la liste des variables
tVar *sat_add_var(tGraphe *pGraph, int pIndVar);

// Ajoute une variable à une clause
int sat_add_var_to_cls(tGraphe *pGraph, unsigned int pIndCls, int pVar);

// Ajoute une clause à un graphe
int sat_add_clause(tGraphe *pGraph, unsigned int pIndCls, int *pTabVar, int pSize);

// Renvoie le signe d'une variable appartenant à une clause
int sat_get_sign(tVar *pVar, unsigned int pIndCls);

// Affiche les clauses
void sat_see(tGraphe *pGraph);

// Supprime une variable
int sat_sub_var(tGraphe *pGraph, unsigned int pIndVar);

// Supprime une variable dans une clause
int sat_sub_var_in_cls(tPtVar *pPtVar, int pSign, tClause *pCls, tGraphe *pGraph);

// Renvoie le pointeur sur la cellule de la variable pointant sur la clause
tPtVarSgn *sat_get_ptr_varSgn(tPtVarSgn *pVarSgn, tClause *pCls);

// Recherche et supprime la cellule de la variable pointant sur la clause
int sat_unlnk_varSgn_cls(tVar *pVar, int pSign, tClause *pCls);

// Enlève les liens entre une clause et sa première variable
int sat_unlnk_cls_var(tGraphe *pGraph, tClause *pCls);
 
// Supprime une clause
int sat_sub_clause(tGraphe *pGraph, unsigned int pIndCls);

// Renvoie la variable de la première clause unitaire trouvée
//  0 si non trouvée
int sat_get_var_cls_unit(tGraphe *pGraph);

#endif // SAT_H
