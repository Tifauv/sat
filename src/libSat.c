/* Fonctions de la libSAT -- Librairie de gestion de graphe SAT
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
#include "libSat.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LN_SIZE 1024
#define LIBSAT_VERS "1.5.2"
#define LIBSAT_NAME "libSAT"
#define LIBSAT_AUTH "Olivier Serve & Micka�l Sibelle & Philippe Strelezki"


// Affiche les infos de version de la librairie
void sat_vers() {
  printf("\n   %s --- Gestion de graphe SAT --- v%s\n", LIBSAT_NAME, LIBSAT_VERS);
  printf(" Cette librairie est un logiciel libre. Elle est fournie sans\n");
  printf(" AUCUNE GARANTIE. Consultez le fichier COPYING pour plus de\n");
  printf(" d�tails sur la licence GPL.\n\n");
  printf(" Auteur(s): %s\n\n", LIBSAT_AUTH);
} // sat_vers


// Lib�re le graphe
void sat_free(tGraphe **pGraph) {
  tClause *lCls;

  if (*pGraph) {
    // Il y a des clauses
    lCls = (*pGraph)->clauses;

    printf("  [");

    // Suppression des clauses une � une
    while (lCls) {
      sat_sub_clause((*pGraph), lCls->indCls);
      lCls = (*pGraph)->clauses;
      printf("#");
    }

    printf("]\n");
    // Normalement, quand toutes les variables sont supprim�es
    // toutes les variables aussi: on peut donc supprimer pGraph
    free(*pGraph);
    (*pGraph) = NULL;
  }
} // sat_free


// Cr�e un graphe vide --------------------------------------------------------
tGraphe *sat_mk() {
  tGraphe *lGraph;
  lGraph = (tGraphe *) malloc(sizeof(tGraphe));
  lGraph -> clauses = NULL;
  lGraph -> vars = NULL;
  return lGraph;
} // sat_mk
 

// Charge un fichier dans un graphe vide � l'origine --------------------------
tGraphe *sat_load_file(char *pNom_fic) {
  FILE *fic;
  char *str;
  int *tab;
  int size=0, i=0, lIndCls=1;
  tGraphe *pGraph;

  // Initialisation de pGraph
  pGraph = sat_mk();

  // Initialisation de str
  str = malloc(LN_SIZE);
  strcpy(str, "\0");

  // Ouverture du fichier
  fic = fopen(pNom_fic, "r");
  if (fic == NULL) {
    fprintf(stderr, " Ooops: Impossible d'ouvrir le fichier %s.\n", pNom_fic);
    exit(1);
  }
  fprintf(stderr, " Fichier %s ouvert.\n", pNom_fic);

  printf("  [");

  while (!feof(fic)) {
    
    // Compteur de ligne
    i++;

    // lecture de la ligne
    fgets(str, LN_SIZE, fic);
    if (feof(fic)) break;
    printf("#");
    str[strlen(str)-1] = '\0';
    fprintf(stderr, "\n Ligne[%d] = |%s|\n", i, str);

    // Transformation string -> tab
    tab = sat_mk_tabVar(str, &size);

    if (sat_add_clause(pGraph, lIndCls, tab, size)) {
      fprintf(stderr, " Waouu: Erreur non fatale � l'ajout de la clause.\n");
      fprintf(stderr, "        La clause n'a pas �t� ajout�e.\n");
    }
    else ++lIndCls;

  }
  printf("]\n");
  fclose(fic);
  fprintf(stderr, " Fichier %s ferm�.\n", pNom_fic);

  return pGraph;
} // sat_load_file


// Teste si une variable appartient � une clause --------------------------
int sat_ex_var_in_cls(int pVar, tClause *pCls) {
  tPtVar *lPtVar;

  // Initialisation
  lPtVar = pCls->vars;

  // Parcours des variables de pCls
  while (lPtVar) {
    fprintf(stderr, "    Test de %d et %d...", pVar, lPtVar->var->indVar * sat_get_sign(lPtVar->var, pCls->indCls));
    if ((lPtVar->var->indVar == abs(pVar)) && (sat_get_sign(lPtVar->var, pCls->indCls) == sat_sign(pVar))) {
      fprintf(stderr, "positif\n");
      fprintf(stderr, "    La Variable %d appartient � la Clause %d.\n", pVar, pCls->indCls);
      return 1;
    }
    else {
      fprintf(stderr, "n�gatif\n");
      lPtVar = lPtVar->suiv;
    }
  }

  fprintf(stderr, "    La Variable %d n'appartient pas � la Clause %d.\n", pVar, pCls->indCls);
  return 0;
} // sat_ex_var_in_cls


// Compare une clause et un tableau de variables --------------------------
int sat_compare_cls_tabVar(tClause *pCls, unsigned int pIndCls, int *pTabVar, int pSize) {
  int nb_var_in_cls=0, i;
  tPtVar *lPtVar;

  // I)  Calculer nb_var_in_cls
  lPtVar = pCls->vars;
  while (lPtVar) {
    lPtVar = lPtVar->suiv;
    ++nb_var_in_cls;
  }

  fprintf(stderr, "   %d variables trouv�es dans la Clause %d, %d dans le tableau.\n", nb_var_in_cls, pCls->indCls, pSize);
  // II) Si nb_var_in_cls == pSize
  if (nb_var_in_cls == pSize) {
    for (i=0; i<pSize; ++i)
      if (sat_ex_var_in_cls(pTabVar[i], pCls) == 0) {
	fprintf(stderr, "   Les deux clauses sont diff�rentes.\n");
	return 0;
      }
    fprintf(stderr, "   Les deux clauses sont �gales.\n");
    return 1;
  }
  else {
    fprintf(stderr, "   Les deux clauses sont diff�rentes.\n");
    return 0;
  }
} // sat_compare_cls_tabVar


// Renvoie un Boolean != 0 si une clause �gale existe d�j�. -----------------
int sat_ex_clause(tGraphe *pGraph, unsigned int pIndCls, int *pTabVar, int pSize) {
  tClause   *pc; // Pointeur permettant le parcours

  // Initialisation du pointeur sur les clauses
  pc = pGraph->clauses;

  // Recherche de la clause d�sir�e
  while (pc) {
    
    if (sat_compare_cls_tabVar(pc, pIndCls, pTabVar, pSize) == 1) 
      return 1;
    else pc = pc->suiv;
  }

  return 0;
} // sat_ex_clause 


// Renvoie le pointeur sur une variable, NULL si elle n'existe pas ------------
tVar *sat_ex_var(tGraphe *pGraph, unsigned int pIndVar) {
  tVar *lVar;
  int trouve=0;

  // Initialisation de lVar
  lVar = pGraph->vars;

  // Recherche
  while (lVar && (!trouve)) {
    if (lVar->indVar == pIndVar) trouve = 1;
    else lVar = lVar->suiv;
  }

  if (trouve) {
    fprintf(stderr, "   Variable %d trouv�e.\n", pIndVar);
    return lVar;
  }
  else {
    fprintf(stderr, "   Variable %d non trouv�e.\n", pIndVar);
    return NULL;
  }
} // sat_ex_var


/* Cr�e un lien entre une variable et une clause dans laquelle elle apparait en tenant compte du signe
   et un lien entre ces m�mes clause et variable */
void sat_lnk_clsVar(tClause *pCls, tVar *pVar, int pSignVar) {
  tPtVarSgn *ps; // Pointeur vers un pointeur dans clsPos ou clsNeg
  tPtVar    *pv; // Pointeur vers une variable 

  //I) Lien Var --> Clause
  
  // Cr�ation de la cellule
  ps = (tPtVarSgn *) malloc(sizeof(tPtVarSgn));

  // Lien de la variable vers la clause
  ps->clause = pCls;

  if (pSignVar == 1) {
    ps->suiv = pVar->clsPos;
    pVar->clsPos = ps;
  }
  else {
    ps->suiv = pVar->clsNeg;
    pVar->clsNeg = ps;
  }

  //II) Lien Clause --> Var
  // Cr�ation de la cellule
  pv = (tPtVar *) malloc(sizeof(tPtVar));

  // Lien de la clause vers la variable
  pv->var = pVar;

  // Cha�nage
  pv->suiv = pCls->vars;
  pCls->vars = pv;

  fprintf(stderr, "   Lien Clause %d <-> Variable %d cr��.\n", pCls->indCls, pSignVar * pVar->indVar);
} // sat_lnk_varCls


// Test si la variable actuelle du tableau n'a pas d�j� �t� trouv�e
int sat_test_prev_var(int *pTabVar, int pVar, int pSize) {
  int i;

  for (i=0; i<pSize; ++i) {
    if ( abs(pVar) == abs(pTabVar[i]) ) {
      fprintf(stderr, "   Variable %d d�j� en position %d (%d).\n", pVar, i, pTabVar[i]);
      return sat_sign(pVar * pTabVar[i]);
    }
  }
  return 0;
} // sat_test_prev_var

// Construit un tableau de variables ------------------------------------------
int *sat_mk_tabVar(char *pStr, int *pSize) {
  int *tab=NULL;
  char *token=NULL;
  int nb_tok=0, rslt=0;
  char *str2, *str1;

  // Sauvegarde de la cha�ne originale
  str1 = malloc(LN_SIZE);
  strcpy(str1, pStr);
  str2 = malloc(LN_SIZE);
  strcpy(str2, pStr);

  // I/ Nombre de tokens dans la cha�ne
  token = strtok(str1, " ");
  while (token) {
    nb_tok++;
    token = strtok(NULL, " ");
  }

  fprintf(stderr, "  %d tokens trouv�s:\n", nb_tok);

  if (nb_tok == 0) return NULL;
  

  *pSize = nb_tok;

  // II/ Cr�ation du tableau
  tab = (int *) calloc(nb_tok, sizeof(int));

  // III/ Initialisation du tableau
  token = strtok(str2, " ");
  nb_tok = 0;
  while (token) {
    // On teste si l'entier n'appara�t pas d�j� dans la variable
    rslt = sat_test_prev_var(tab, atoi(token), nb_tok);
    if (rslt == 1)  { // Le token appara�t 2 fois avec le m�me "signe" -> pas ajout� cette fois
      fprintf(stderr, "  Waouu: La variable %s a d�j� �t� ajout�e.\n", token);
      --*pSize;
    }
    else if (rslt == -1) { // Le token et son contraire appara�ssent -> tab = NIL, pSize = 0;
      fprintf(stderr, "  Waouu: La variable %s et son contraire appara�ssent: tableau vide.\n", token);
      free(tab);
      *pSize = 0;
      return NULL;
    }
    else {
      tab[nb_tok] = atoi(token);
      fprintf(stderr, "  Ajout de |%d| au rang %d.\n", tab[nb_tok], nb_tok);
      nb_tok++;
    }
    token = strtok(NULL, " ");
  }

  return tab;
} // sat_mk_tabVar


// Renvoie le signe d'un entier -----------------------------------------------
int sat_sign(int pNbe) {
  if (pNbe <0) return -1;
  else return 1;
} // sat_sign
 

// Ajoute une variable au graphe ----------------------------------------------
tVar *sat_add_var(tGraphe *pGraph, int pIndVar) {
  tVar *var;

  // Cr�ation de var
  var = (tVar *) malloc(sizeof(tVar));

  // Initialisation de var
  var->indVar = abs(pIndVar);
  var->clsPos = NULL;
  var->clsNeg = NULL;
  var->suiv = NULL;
  fprintf(stderr, "   Variable %d cr��e.\n", pIndVar);

  // Cha�nage
  var->suiv = pGraph->vars;
  pGraph->vars = var;
  fprintf(stderr, "   Variable %d cha�n�e.\n", pIndVar);

  return var;
} // sat_add_var


// Ajoute une variable � une clause
int sat_add_var_to_cls(tGraphe *pGraph, unsigned int pIndCls, int pVar) {
  tClause *lCls;
  tVar *lVar;
  int *lTabVar;

  if (!pGraph) {
    fprintf(stderr, " Ooops: Le pointeur de graphe est NULL.\n");
    return -1;
  }

  // I) Recherche de la clause...
  lCls = pGraph->clauses;
  while (lCls && (lCls->indCls != pIndCls))
    lCls = lCls->suiv;
  if (!lCls) {
    fprintf(stderr, "  Clause %d non trouv�e.\n", pIndCls);
    lTabVar = malloc(sizeof(int));
    lTabVar[0] = pVar;
    sat_add_clause(pGraph, pIndCls, lTabVar, 1);
    return 1;
  }
  else {
    fprintf(stderr, "  Clause n�%d trouv�e.\n", lCls->indCls);

    // Teste si la variable existe
    lVar = sat_ex_var(pGraph, abs(pVar));
    if (!lVar) {
      fprintf(stderr, "   Variable %d non trouv�e: ajout.\n", abs(pVar));
      lVar = sat_add_var(pGraph, abs(pVar));
    }

    // Lien entre la variable et la clause
    sat_lnk_clsVar(lCls, lVar, sat_sign(pVar));
    return 0;
  }
} // sat_add_var_to_cls


// Ajoute une clause � un graphe ----------------------------------------------
int sat_add_clause(tGraphe *pGraph, unsigned int pIndCls, int *pTabVar, int pSize) {
  tClause *lCls;
  int *lTabVar;
  tVar *lVar;
  int i;

  // Teste si on essaye d'ajouter une clause vide (stupide)
  if (pSize == 0) {
    fprintf(stderr, "  Waouu: Tentative d'ajout de la clause vide.\n         Continue...\n");
    return 1;
  }

  // Teste si la clause existe
  if (sat_ex_clause(pGraph, pIndCls, pTabVar, pSize) == 1) {
    fprintf(stderr, "  Waouu: Tentative d'ajout de clause existante.\n         Continue...\n");
    return 2;
  }

  // Construction et Ajout de la clause

  // Cr�ation de la clause
  lCls = (tClause *)malloc(sizeof(tClause));

  fprintf(stderr, "  Clause %d cr��e.\n", pIndCls);

  // M�j de l'indice
  lCls->indCls = pIndCls;
  lCls->suiv = NULL;
  lCls->vars = NULL;

  // Cha�nage des variables
  lTabVar = pTabVar;
  // Parcours des variables de la clause
  for (i=0; i<pSize; i++) {

    fprintf(stderr, "  Lien Clause %d <-> Variable %d en cours...\n", pIndCls, lTabVar[i]);

    // Si la variable n'existe pas, l'ajouter.
    if ( !(lVar = sat_ex_var(pGraph, abs(lTabVar[i]))) )
      lVar = sat_add_var(pGraph, lTabVar[i]);
    // Linkage Clause <--> Variable    
    sat_lnk_clsVar(lCls, lVar, sat_sign(lTabVar[i]));
    
  }
  fprintf(stderr, "  Clause %d initialis�e.\n", pIndCls);

  // Cha�nage des clauses
  lCls->suiv = pGraph->clauses;
  pGraph->clauses = lCls;

  fprintf(stderr, "  Clause %d cha�n�e.\n", pIndCls);

  return 0;
} // sat_add_clause


// Renvoie le signe d'une variable dans une clause ----------------------------
int sat_get_sign(tVar *pVar, unsigned int pIndCls) {
  tPtVarSgn *ps; // Pointeur de parcours des clsPos et clsNeg de tVar
  int trouve=0; // Boolean != 0 si on a trouv� la clause

  // Recherche parmis les clauses "positives"
  if (!pVar) {
    fprintf (stderr, "Ooops: la variable est vide.\n");
    exit(1);
  }
  else
  {
    ps = pVar->clsPos;

  while (ps && !trouve) {
    if (ps->clause->indCls == pIndCls) {
      trouve = 1;
      return 1;
    }
    else ps = ps->suiv;
  }

  // Si pas trouv�, recherche parmis les "n�gatifs"
  ps = pVar->clsNeg;

  while (ps && !trouve) {
    if (ps->clause->indCls == pIndCls) {
      trouve = 1;
      return -1;
    }
    else ps = ps->suiv;
  }
  }
  // Cas qui ne devrait pas arriver, mais on ne sait jamais...
  return 0;
} // sat_get_sign


// Affiche les clauses d'un graphe --------------------------------------------
void sat_see(tGraphe *pGraph) {
  tClause *pc; // Pointeur de parcours de la liste des clauses
  tPtVar  *pv; // Pointeur de parcours de la liste des variables
  int   sign; // Variable contenant le signe d'une variable dans une clause
  int  deb=0; // Indique si l'on est au d�but de la ligne

  // V�rif de pGraph
  if (!pGraph) {
    fprintf(stderr, "  Waouu: Le pointeur de graphe est NULL.\n");
  }
  else {
    // Initialisation de pc
    pc = pGraph->clauses;
    printf("  Clauses = {\n");

    // Parcours des clauses
    while (pc) {
      // Num�ro de la clause
      printf("   %d: ", pc->indCls);
      deb = 1;

      // Affichage de la clause
      pv = pc->vars;
      while (pv) {
	sign = sat_get_sign(pv->var, pc->indCls);

	if (deb != 1) printf("v ");
	if (sign == -1) printf("�");
	printf("X%d ", pv->var->indVar);
	if (deb == 1) deb = 0;

	// Variable suivante
	pv = pv->suiv;
      }
     
      // Clause suivante
      printf("\n");
      pc = pc->suiv;
    }
    printf("  }\n");
  }
} // sat_see


// Supprime une variable -------------------------------------------
int sat_sub_var(tGraphe *pGraph, unsigned int pIndVar) {
  tVar *lVar, *lVar2;

  // Teste si le graphe est vide
  if (!pGraph) { 
    fprintf(stderr, " Ooops: le graphe est vide, impossible de supprimer une variable.\n");
    return 1;
  }

  // initialisatin de lVar
  lVar = pGraph->vars;

  // V�rification de lVar
  if (!lVar) {
    fprintf(stderr, " Ooops: la liste des variables est vide, impossible de supprimer une variable.\n");
    return 2;
  }

  // Teste si c'est la premi�re variable
  if (lVar->indVar == pIndVar) {
    fprintf(stderr, "  Suppression de la Variable %d.\n", lVar->indVar);
    // Cha�nage pr�c <-> suiv
    pGraph->vars = lVar->suiv;
    // Lib�ration de la variable
    free(lVar);
  }
  else {
    // Recherche parmis les suivantes
    while (lVar->suiv && (lVar->suiv->indVar != pIndVar)) 
      lVar = lVar->suiv;

    // lVar2: �l�ment � supprimer
    lVar2 = lVar->suiv;

    // V�rification de lVar2
    if (!lVar2) {
      fprintf(stderr, "  Ooops: La variable %d n'existe pas dans le graphe.\n", pIndVar);
      return 3;
    }

    fprintf(stderr, "  Suppression de la Variable %d.\n", lVar2->indVar);

    // Cha�nage pr�c�dant <-> suivant
    lVar->suiv = lVar2->suiv;

    // Lib�ration de la variable
    free(lVar2);
  }
  fprintf(stderr,  "  Variable %d supprim�e.\n", pIndVar);
  return 0;
} // sat_sub_var


// Supprime une variable dans une clause
int sat_sub_var_in_cls(tPtVar *pPtVar, int pSign, tClause *pCls, tGraphe *pGraph) {
  tPtVar *lPtVar, *lPtVar2;
  tPtVarSgn *lVarSgn;

  // V�rification de pCls
  if (!pCls) {
    fprintf(stderr, "     Ooops: Le pointeur de clause est NULL.\n");
    return -1;
  }

  // V�rification de pGraph
  if (!pGraph) {
    fprintf(stderr, "     Ooops: Le pointeur de Graphe est NULL.\n");
    return -2;
  }

  // Recherche du pointeur sur la variable
  lPtVar = pCls->vars;

  if (lPtVar == pPtVar) { // premier � supprimer 
    // D�fait les liens et tout et tout
    sat_unlnk_cls_var(pGraph, pCls);

    // Si la variable est vide, la virer
    if (!lPtVar->var->clsPos && !lPtVar->var->clsNeg)
      sat_sub_var(pGraph, lPtVar->var->indVar);

    // Codes de retour
    if (!pCls->vars) {
      fprintf(stderr, "      La clause vide a �t� produite.\n");
      return 3;
    }
    else {
      fprintf(stderr, "       La clause modifi�e est non vide.\n");
      return 2;
    }
  }
  while (lPtVar->suiv) {
    if (lPtVar->suiv == pPtVar) {
      // Suppr lien Var-> cls
      // Teste le signe et initialise la recherche
      if ((pSign != 1) && (pSign != -1)) {
        fprintf(stderr, "      Ooops: Le signe demand� (%d) est incorrect: 1 -> positif, -1 -> n�gatif\n", pSign);
        return 1;
      }
      if (pSign ==  1) lVarSgn = lPtVar->suiv->var->clsPos;
      if (pSign == -1) lVarSgn = lPtVar->suiv->var->clsNeg;

      // II/ Suppression du lien Variable -> Clause -------------------------
      if (!lVarSgn) {
	fprintf(stderr, "      Waouu: La Variable %d n'est pas utilis�e.\n", pSign * lPtVar->suiv->var->indVar);
	return 2;
      }

      if (lVarSgn->clause == pCls) { // teste si la premi�re clause correspond
	// Cha�nage de la pr�c�dente avec la suivante
      if (pSign == 1) // Cha�nage des positifs
	lPtVar->suiv->var->clsPos = lVarSgn->suiv;
      else lPtVar->suiv->var->clsNeg = lVarSgn->suiv;
      // Lib�ration de la cellule de la variable
      free(lVarSgn);
      fprintf(stderr, "      Lien Variable %d -> Clause %d d�truit.\n", pSign * lPtVar->suiv->var->indVar, pCls->indCls);
      }
      else { // la clause n'est pas la premi�re
	lVarSgn = sat_get_ptr_varSgn(lVarSgn, pCls);
	if (lVarSgn) { // cellule trouv�e
	  // Lien pr�c <-> suiv
	  lVarSgn->suiv = lVarSgn->suiv->suiv;
	  // Lib�ration de la cellule
	  free(lVarSgn->suiv);
	  fprintf(stderr, "      Lien Variable %d -> Clause %d d�truit.\n", pSign * lPtVar->suiv->var->indVar, pCls->indCls);
	}
	else {
	  fprintf(stderr, "      Waouu: Pas de lien Variable %d -> Clause %d trouv�.\n", pSign * lPtVar->suiv->var->indVar, pCls->indCls);
	  return 3;
	}
      }

      // Sur-lien cls -> var suiv
      lPtVar2 = lPtVar->suiv;
      lPtVar->suiv = lPtVar->suiv->suiv;
      fprintf(stderr, "      Lien Clause %d -> Variable %d d�truit.\n", pCls->indCls, lPtVar2->var->indVar);

      // Si la variable est vide, la virer
      if (!lPtVar2->var->clsPos && !lPtVar2->var->clsNeg)
	sat_sub_var(pGraph, lPtVar2->var->indVar);

      // Suppression de la cellule
      free(lPtVar2);

      // Codes de retour
      if (!pCls->vars) {
	fprintf(stderr, "      La clause vide a �t� produite.\n");
	return 3;
      }
      else {
	fprintf(stderr, "      La clause modifi�e est non vide.\n");
	return 2;
      }
    }
    else lPtVar = lPtVar->suiv;
  }

  fprintf(stderr, "      Waouu: La clause n'a pas �t� modifi�e.\n");
  return 4;

} // sat_sub_var_in_cls


// Renvoie le pointeur sur la cellule de la clause pointant sur la clause
tPtVarSgn *sat_get_ptr_varSgn(tPtVarSgn *pVarSgn, tClause *pCls) {
  tPtVarSgn *lVarSgn;

  // V�rifie que la clause n'est pas NULL
  if (!pCls) {
    fprintf(stderr, "    Ooops: La clause n'existe pas.\n");
    return NULL;
  }

  // V�rification de lVarSgn
  if (!pVarSgn) {
    fprintf(stderr, "    Ooops: Pas de lien entre la variable et une clause");
    return NULL;
  }

  // Recherche
  lVarSgn = pVarSgn;

  while (lVarSgn->suiv && (lVarSgn->suiv->clause != pCls))
    lVarSgn = lVarSgn->suiv;

  // Si pas de r�sultat trouv�, retourne NULL, sinon le pointeur voulu
  if (!lVarSgn->suiv) return NULL;
  else return lVarSgn;
} // sat_get_ptr_varSgn


// Recherche et supprime la cellule de la variable pointant sur la clause
int sat_unlnk_varSgn_cls(tVar *pVar, int pSign, tClause *pCls) {
  tPtVarSgn *lVarSgn;
  tPtVar *lPtVar;

  // I/ Tests d'application --------------------------------------------

  // Teste le signe et initialise la recherche
  if ((pSign != 1) && (pSign != -1)) {
    fprintf(stderr, "   Ooops: Le signe demand� (%d) est incorrect: 1 -> positif, -1 -> n�gatif\n", pSign);
    return 1;
  }
  if (pSign ==  1) lVarSgn = pVar->clsPos;
  if (pSign == -1) lVarSgn = pVar->clsNeg;

  // II/ Suppression du lien Variable -> Clause -------------------------
  if (!lVarSgn) {
    fprintf(stderr, "   Waouu: La Variable %d n'est pas utilis�e.\n", pSign * pVar->indVar);
    return 2;
  }

  if (lVarSgn->clause == pCls) { // teste si la premi�re clause correspond
    // Cha�nage de la pr�c�dente avec la suivante
    if (pSign == 1) // Cha�nage des positifs
      pVar->clsPos = lVarSgn->suiv;
    else pVar->clsNeg = lVarSgn->suiv;
    // Lib�ration de la cellule de la variable
    free(lVarSgn);
    lVarSgn = NULL;
    fprintf(stderr, "   Lien Variable %d -> Clause %d d�truit.\n", pSign * pVar->indVar, pCls->indCls); 
  }
  else { // la clause n'est pas la premi�re
    lVarSgn = sat_get_ptr_varSgn(lVarSgn, pCls);
    if (lVarSgn) { // cellule trouv�e
      // Lien pr�c <-> suiv
      lVarSgn->suiv = lVarSgn->suiv->suiv;    
      // Lib�ration de la cellule
      free(lVarSgn->suiv);
      fprintf(stderr, "   Lien Variable %d -> Clause %d d�truit.\n", pSign * pVar->indVar, pCls->indCls);
    }
    else {
      fprintf(stderr, "   Waouu: Pas de lien Variable %d -> Clause %d trouv�.\n", pSign * pVar->indVar, pCls->indCls);
      return 3;
    }
  }

  // III/ Suppression du lien Clause -> Variable ---------------------
  lPtVar = pCls->vars;
  pCls->vars = pCls->vars->suiv;
  free(lPtVar);
  lPtVar = NULL;
  fprintf(stderr, "   Lien Variable %d <- Clause %d d�truit.\n", pSign * pVar->indVar, pCls->indCls);
  return 0;

} // sat_unlnk_varSgn_cls


// Enl�ve les liens entre une clause et sa premi�re variable
int sat_unlnk_cls_var(tGraphe *pGraph, tClause *pCls) {
  tVar *lVar;

  // V�rification de la clause
  if (!pCls) {
    fprintf(stderr, "  Ooops: La clause n'existe pas.\n");
    return 1;
  }

  fprintf(stderr, "  Suppression des liens entre la clause %d et sa premi�re variable...\n", pCls->indCls);
  // V�rification de la liste des variables de la clause
  if (!pCls->vars) {
    fprintf(stderr, "  Waouu: La clause %d ne contient pas de variables.\n", pCls->indCls);
    return 2;
  }

  // Initialisation de lVar
  lVar = pCls->vars->var;

  // Test de validit� de lVar
  if (!lVar) {
    fprintf(stderr, "  Ooops: La variable est NULL.\n");
    return 3;
  }

  // Appels de sat_unlnk_varSgn_cls
  else {
    if (sat_unlnk_varSgn_cls(lVar, 1, pCls) == 0)
      fprintf(stderr, "  Lien Variable %d <-> Clause %d d�truit.\n", lVar->indVar, pCls->indCls);
    else
      if (sat_unlnk_varSgn_cls(lVar, -1, pCls) == 0)
        fprintf(stderr, "  Lien Variable -%d <-> Clause %d d�truit.\n", lVar->indVar, pCls->indCls);
      else fprintf(stderr, "  Waouu: Impossible de trouver le lien Variable %d -> Clause %d.\n", lVar->indVar, pCls->indCls);
  }

  // Apr�s destruction des liens, on teste si on peut supprimer la variable
  if ((!lVar->clsPos) && (!lVar->clsNeg))
    sat_sub_var(pGraph, lVar->indVar);
  
  return 0;
} // sat_unlnk_cls_var


// Enl�ve les liens entre une clause et toutes ses variables
int sat_sub_clause(tGraphe *pGraph, unsigned int pIndCls) {
  tPtVar *ptVars;
  tClause *cls, *cls2;

  fprintf(stderr, "\n Suppression de la clause %d...\n", pIndCls);
  // Teste si le graphe est vide, auquel cas -> quit !
  if (!pGraph) {
    fprintf(stderr, " Ooops: Aucune clause � supprimer car le graphe est vide !\n");
    return 1;
  }

  // Initialisation
  cls = pGraph->clauses;

  // Test de validit�
  if (!cls) {
    fprintf(stderr, " Ooops: Il n'y a pas de clause dans ce graphe !\n");
    return 2;
  }
  
  // Si c'est la premi�re clause ...
  if (cls->indCls == pIndCls) {
    
    // Destruction des liens
    ptVars = cls->vars;
    while (ptVars) {
      sat_unlnk_cls_var(pGraph, cls);
      ptVars = cls->vars;
    }
    // On re-cha�ne
    pGraph->clauses = cls->suiv;
    // On lib�re cls
    free(cls);
    fprintf(stderr, " Clause %d d�truite.\n", pIndCls);
  }
  else {
    
    // Recherche du pointeur sur la clause d'indice pIndCls
    while (cls->suiv && (cls->suiv->indCls != pIndCls)) 
      cls = cls->suiv;

    cls2 = cls->suiv;

    // On retire les liens
    ptVars = cls2->vars;
    while (ptVars) {
      sat_unlnk_cls_var(pGraph, cls2);
      ptVars = cls2->vars;
    }
    // On re-cha�ne avec le suivant
    cls->suiv = cls->suiv->suiv;
    // On lib�re cls
    free(cls2);
    fprintf(stderr, " Clause %d d�truite.\n", pIndCls);
  }
  return 0;
} // sat_sub_clause


// Renvoie la variable de la premi�re clause unitaire trouv�e
//  0 si non trouv�e
int sat_get_var_cls_unit(tGraphe *pGraph) {
  tClause *lCls;
  tVar    *lVar;
  int      rslt;

  // V�rification de pGraph
  if (!pGraph) {
    fprintf(stderr, "  Ooops: Le pointeur de graphe est NULL.\n");
    return 1;
  }

  // Recherche...
  lCls = pGraph->clauses;

  while (lCls) {
    if (!lCls->vars) {
      fprintf(stderr, "  Waouu: La clause %d ne contient pas de variables.\n", lCls->indCls);
    }
    else if (!lCls->vars->suiv) {
      rslt = sat_get_sign(lCls->vars->var, lCls->indCls) * lCls->vars->var->indVar;
      fprintf(stderr, "  La clause n�%d (", lCls->indCls);
      if (rslt <0) fprintf(stderr, "�");
      fprintf(stderr, "X%d) est unitaire.\n", abs(rslt));
      return rslt;
    }
    else {
      fprintf(stderr, "  La clause %d n'est pas unitaire.\n", lCls->indCls);
      lCls = lCls->suiv;
    }
  }

  // Pas de clause unitaire trouv�e
  if (!pGraph->vars) {
    fprintf(stderr, "  Ooops: Plus de variables dans le graphe.\n");
    return 0;
  }
  lVar = pGraph->vars;
  if (!lVar->clsPos) {
    if (!lVar->clsNeg) {
      fprintf(stderr, "  Ooops: La Variable %d n'est li�e � aucune clause.\n", lVar->indVar);
      return 0;
    }
    else {
      return -lVar->indVar;
    }
  }
else return lVar->indVar;
} // sat_get_var_cls_unit

// FIN DE libSat
