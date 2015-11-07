/* Fonctions de la libDP -- Algorithme de Davis-Putnam pour probl�mes SAT
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

#include "libDP.h"

#include <stdio.h>
#include <stdlib.h>
#include "libHist.h"
#include "libSat.h"


#define LIBDP_VERS "1.2.4"
#define LIBDP_NAME "libDP"
#define LIBDP_AUTH "Olivier Serve & Mickael Sibelle & Philippe Strelezki"


// Affiche les infos de version de la librairie
void dp_vers() {
  printf("\n   %s --- Algorithme de Davis-Putnam --- v%s\n", LIBDP_NAME, LIBDP_VERS);
  printf(" Cette librairie est un logiciel libre. Elle est fournie sans\n");
  printf(" AUCUNE GARANTIE. Consultez le fichier COPYING pour plus de\n");
  printf(" d�tails sur la licence GPL.\n\n");
  printf(" Auteur(s): %s\n\n", LIBDP_AUTH);
} // dp_vers


// Lancement de la r�solution
tIntr *dp_sat(tGraphe **pGraph) {

  // V�rification de pGraph
  if (!pGraph) {
    fprintf(stderr, " Ooops: Le pointeur du graphe est NULL.\n");
    return NULL;
  }

  return dp_davis_putnam(pGraph, intr_mk());
} // dp_sat


// Davis-Putnam
tIntr *dp_davis_putnam(tGraphe **pGraph, tIntr *pIntr) {
  int var_cls_unit;
  tIntr *lIntr, *rslt;
  tHist *lHist;

  // V�rification de pGraph
  if (!(*pGraph)) {
    fprintf(stderr, " Ooops: Le pointeur de graphe est NULL.\n");
    return NULL;
  }

  // V�rification de pIntr
  if (!pIntr) {
    fprintf(stderr, " Ooops: Le pointeur de l'interpr�tation est NULL.\n");
    return NULL;
  }

  fprintf(stderr, "\n\n Davis-Putnam...\n");
  //  sat_see(*pGraph);
  //  intr_see(pIntr);

  if (!(*pGraph)->clauses) return pIntr;

  // Prendre la variable de la premi�re clause unitaire qui traine
  var_cls_unit = sat_get_var_cls_unit(*pGraph);

  // Cr�ation d'un historique
  fprintf(stderr, " Cr�ation d'un historique...");
  lHist = hist_mk();
  fprintf(stderr, "OK\n");

  // Premi�re r�duction et test du r�sultat
  fprintf(stderr, " Premi�re tentative de r�duction...\n");
  dp_reduire(var_cls_unit, pGraph, lHist);
  lIntr = dp_test_sat(pGraph, var_cls_unit, pIntr);

  if (intr_is_insatisfiable(lIntr)) {

    fprintf(stderr, " R�g�n�ration du graphe pour seconde tentative...\n");
    hist_redo(lHist, pGraph);
 
    // Seconde r�duction et test du r�sultat
    fprintf(stderr, " Seconde tentative de r�duction...\n");
    dp_reduire(-var_cls_unit, pGraph, lHist);
    rslt = dp_test_sat(pGraph, -var_cls_unit, pIntr);

    // Reconstruction du graphe & destruction de l'historique
    hist_redo(lHist, pGraph);
    hist_free(&lHist);

    return rslt;
  }
  else {
    // Destruction de l'historique
    hist_free(&lHist);

    return pIntr;
  }
} // dp_davis_putnam


// Renvoie l'interpr�tation du graphe
tIntr *dp_test_sat(tGraphe **pGraph, int pLitt, tIntr *pLsLitt) {
  tIntr *lIntr;

  fprintf(stderr, "\n  Test de satisfiabilit�: ");
  // Si insatisfiable...
  if (!(*pGraph)) {
    fprintf(stderr, "Insatisfiable\n");
    lIntr = intr_mk();
    intr_set_insatisfiable(&lIntr);
    return lIntr;
  }
  // Si graphe est vide
  else if (!(*pGraph)->clauses) {
    fprintf(stderr, "Plus de clauses\n");
    intr_add(pLsLitt, pLitt);
    return pLsLitt;
  }
  else {
    fprintf(stderr, "Il reste des clauses\n");
  // ajout du litt�ral � l'interpr�tation
  fprintf(stderr, "  Ajout de ");
  if (pLitt < 0) fprintf(stderr, "�");
  fprintf(stderr, "X%d � l'interpr�tation.\n", abs(pLitt));
  intr_add(pLsLitt, pLitt);
  return dp_davis_putnam(pGraph, pLsLitt);
  }
} // dp_test_sat


// R�duit une clause en f� du litt�ral
int dp_reduire_cls(int pLitt, tClause *pCls, tGraphe *pGraph, tHist *pHist) {
  tPtVar *lPtVar;

  // V�rification de tClause
  if (!pCls) {
    fprintf(stderr, " Ooops: Le pointeur de clause est NULL.\n");
    return -1;
  }

  //  sat_see(pGraph);
  fprintf(stderr, "\n   R�duction de la Clause n�%d par ", pCls->indCls);
  if (pLitt < 0) fprintf(stderr, "�");
  fprintf(stderr, "X%d.\n",  abs(pLitt));

  lPtVar = pCls->vars;

  while (lPtVar) {
    fprintf(stderr, "    Test de X%d == X%d: ", lPtVar->var->indVar, abs(pLitt));
    if (lPtVar->var->indVar == abs(pLitt)) {
      fprintf(stderr, "positif\n");
      fprintf(stderr, "     Test de polarit�: ");
      if (sat_sign(pLitt) == sat_get_sign(lPtVar->var, pCls->indCls)) {

	// m�me polarit�: on vire la clause
	fprintf(stderr, "positif\n");

	// Ajout de la suppresion � l'historique
	fprintf(stderr, "    Sauvegarde de la clause dans l'historique.\n");
	hist_add_cls(pHist, pCls);

	// Suppression de la clause
	sat_sub_clause(pGraph, pCls->indCls);
	return 1;
      }
      else { // polarit� contraire: on vire le litt�ral de la clause
	// retourne: 2 si la clause est non vide (apr�s traitement)
	//           3 si elle est vide (insatisfiable).
	fprintf(stderr, "n�gatif\n");

	// Ajout de la modif
	fprintf(stderr, "    Sauvegarde de la variable dans l'historique.\n");
	hist_add_var(pHist, pCls->indCls, lPtVar->var->indVar * (-sat_sign(pLitt)));

	// Suppresion de la variable de la clause
	return sat_sub_var_in_cls(lPtVar, -sat_sign(pLitt), pCls, pGraph);
      }
    }
    else {
      fprintf(stderr, "n�gatif\n");
      lPtVar = lPtVar->suiv;
    }
  }
  return 0;
} // dp_reduire_cls


// R�duit le graphe en fonction du litt�ral
void dp_reduire(int pLitt, tGraphe **pGraph, tHist *pHist) {
  tClause *lCls, *lCls2;
  int rslt;

  if (!*pGraph) {
    fprintf(stderr, "  Waouu: Le pointeur de graphe est NULL.\n");
  }
  else {
    fprintf(stderr, "  R�duction par ");
    if (pLitt < 0) fprintf(stderr, "�");
    fprintf(stderr, "X%d...\n", abs(pLitt));
    lCls = (*pGraph)->clauses;

    while (lCls) {
      lCls2 = lCls->suiv;
      rslt = dp_reduire_cls(pLitt, lCls, (*pGraph), pHist);
      if (rslt == 3) { // Clause vide produite: insatisfiable
	fprintf(stderr, "  Insatisfiable: return NULL\n");


	// Sauvegarde des modifs dans l'historique
	fprintf(stderr, "  Sauvegarde du graphe actuel...\n");
	lCls2 = (*pGraph)->clauses;
	while (lCls2) {
	  rslt = hist_add_cls(pHist, lCls2); 
	  if (rslt == -3) 
	    fprintf(stderr, "   La clause n�%d est vide et n'a pas �t� ajout�e.\n", lCls2->indCls);
	  else if (rslt == 0) 
	    fprintf(stderr, "   Clause n�%d mise dans l'historique.\n", lCls2->indCls);
	  lCls2 = lCls2->suiv;
	}

	// Suppression du graphe
	fprintf(stderr, "  Suppression du graphe...\n");
        sat_free(pGraph);

	// Pour sortir du while
	lCls = NULL;
      }
      else {
	if (rslt == 1) lCls = lCls2;
	else lCls = lCls->suiv;
      }
    }
  }
} // dp_reduire

// FIN DE libDP
