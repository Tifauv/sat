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
#ifndef FORMULA_H
#define FORMULA_H

#include <list>
#include <stdlib.h>
#include "literal.h"
#include "clause.h"

#define LN_SIZE 1024

typedef struct tVar tVar;           // Type Liste de variables

typedef struct tPtVarSgn tPtVarSgn; // Type Liste de pointeurs (indiquant le signe d'une variable dans une clause)

typedef struct tClause tClause;     // Type Liste de clauses

typedef struct tPtVar tPtVar;       // Type Liste de pointeurs vers une variable contenue dans une clause

// Type Liste de clauses
struct tClause {
   ClauseId      indCls; // Indice de la clause : entier non-signé (0 -> 255)
   tPtVar       *vars  ; // Pointeur sur la liste des variables de la clause
   tClause      *suiv  ; // Pointeur chaînant
};

// Type Liste de variables
struct tVar {
   LiteralId      indVar; // Indice de la variable
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


// Renvoie un graphe vide
tGraphe* sat_new();

// Libère la mémoire occupée par le graphe
void sat_free(tGraphe** p_formula);

// Teste si une variable existe
tVar* sat_ex_var(tGraphe* p_formula, LiteralId p_literalId);

// Lie une variable à une clause et inversement
void sat_lnk_clsVar(tClause* p_clause, tVar* p_literal, int p_literalSignVar);

// Renvoie le signe d'un entier
int sat_sign(int pNbe);

// Crée et ajoute une variable à la liste des variables
tVar* sat_add_var(tGraphe* p_formula, LiteralId p_literalId);

// Ajoute une variable à une clause
int sat_add_var_to_cls(tGraphe* p_formula, ClauseId p_clauseId, Literal p_literal);

// Ajoute une clause à un graphe
int sat_add_clause(tGraphe* p_formula, ClauseId p_clauseId, std::list<Literal>& p_literals);

// Renvoie le signe d'une variable appartenant à une clause
int sat_get_sign(tVar* p_literal, ClauseId p_clauseId);

// Affiche les clauses
void sat_see(tGraphe* p_formula);

// Supprime une variable
int sat_sub_var(tGraphe* p_formula, LiteralId p_literalId);

// Supprime une variable dans une clause
int sat_sub_var_in_cls(tPtVar* pPtVar, int p_literalSign, tClause* p_clause, tGraphe* p_formula);

// Renvoie le pointeur sur la cellule de la variable pointant sur la clause
tPtVarSgn* sat_get_ptr_varSgn(tPtVarSgn* p_signedLiteral, tClause* p_clause);

// Recherche et supprime la cellule de la variable pointant sur la clause
int sat_unlnk_varSgn_cls(tVar* p_literal, int p_literalSign, tClause* p_clause);

// Enlève les liens entre une clause et sa première variable
int sat_unlnk_cls_var(tGraphe* p_formula, tClause* p_clause);
 
// Supprime une clause
int sat_sub_clause(tGraphe* p_formula, ClauseId p_clauseId);

// Renvoie la variable de la première clause unitaire trouvée
//  0 si non trouvée
int sat_get_var_cls_unit(tGraphe* p_formula);

#endif // FORMULA_H
