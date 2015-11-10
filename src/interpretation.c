/* Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki
   Copyright (C) 2015
   
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
#include "utils.h"


/**
 * Creates an empty interpretation.
 * 
 * @return an empty interpretation
 */
tIntr* intr_new() {
	tIntr* interpretation;

	// Création de la liste
	interpretation = (tIntr*) malloc(sizeof(tIntr));

	// Initialisation
	interpretation->insatisfiable = SATISFIABLE;
	interpretation->deb = NULL;
	interpretation->fin = NULL;

	return interpretation;
}


/**
 * Deletes an interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 * 
 * @return -1 if p_interpretation is NULL,
 *          0 otherwise
 */
int intr_free(tIntr** p_interpretation) {
	// Vérification de l
	if (isNull(*p_interpretation)) {
		fprintf(stderr, " Ooops: Le pointeur de liste est NULL.\n");
		return -1;
	}

	// Suppression des éléments
	tLitt* e = (*p_interpretation)->deb;
	while (notNull(e)) {
		intr_poke(p_interpretation);
		e = (*p_interpretation)->deb;
	}

	free(*p_interpretation);
	*p_interpretation = NULL;
	return 0;
}


/**
 * Appends a literal to an interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 * @param p_literal
 *            the literal
 * 
 * @return -1 if p_interpretation is NULL,
 *          0 if the literal was appended,
 *          1 if the literal was appended and the satisfiability status reset
 */
int intr_push(tIntr* p_interpretation, Literal p_literal) {
	// Vérification de p_interpretation
	if (isNull(p_interpretation)) {
		fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
		return -1;
	}

	// Création de e
	tLitt* e = (tLitt *) malloc(sizeof(tLitt));

	// Initialisation de e
	e->litt = p_literal;
	e->suiv = NULL;

	if (isNull(p_interpretation->deb)) {
		// Si vide
		p_interpretation->deb = e;
		p_interpretation->fin = e;
	}
	else {
		// Ptr de p_interpretation
		p_interpretation->fin->suiv = e;
		p_interpretation->fin = e;
	}

	if (p_interpretation->insatisfiable == UNSATISFIABLE) {
		fprintf(stderr, " Waouu: L'interprétation était insatisfiable.\n");
		p_interpretation->insatisfiable = SATISFIABLE;
		fprintf(stderr, "        Elle est maintenant satisfiable");
		return 1;
	}

	return 0;
}


/**
 * Removes the last literal of the interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 * 
 * @return -2 if p_interpretation is empty,
 *         -1 if p_interpretation is NULL,
 *          0 if the last literal could be removed
 */          
int intr_poke(tIntr** p_interpretation) {
	// Parameters check
	if (isNull(*p_interpretation)) {
		fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
		return -1;
	}

	// Check the size
	if (isNull((*p_interpretation)->deb)) {
		fprintf(stderr, " Waouu: L'interprétation est vide: rien à supprimer.\n");
		return -2;
	}
 
	// One element only : remove it
	if ((*p_interpretation)->deb == (*p_interpretation)->fin) {
		tLitt* e = (*p_interpretation)->deb;
		(*p_interpretation)->deb = NULL;
		(*p_interpretation)->fin = NULL;
		free(e);
	}
	// List of elements : get the last element and remove it
	else {
		tLitt* e  = (*p_interpretation)->fin;
		tLitt* e2 = (*p_interpretation)->deb;
		while (e2->suiv != e)
			e2 = e2->suiv;
		(*p_interpretation)->fin = e2;
		(*p_interpretation)->fin->suiv = NULL;
		free(e);
	}
	return 0;
}


/**
 * Gives the value of the 'unsatisfiable' bit of an interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 * 
 * @return -1 if p_interpretation is NULL,
 *          0 if p_interpretation is satisfiable,
 *          1 if p_interpretation is unsatisfiable
 */
int intr_is_insatisfiable(tIntr* p_interpretation) {
	// Parameters check
	if (isNull(p_interpretation)) {
		fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
		return -1;
	}

	return p_interpretation->insatisfiable;
}


/**
 * Marks the interpretation as unsatisfiable.
 * 
 * @param p_interpretation
 *            the interpretation
 * 
 * @return -1 if p_interpretation is NULL,
 *          0 if p_interpretation could be set unsatisfiable
 */
int intr_set_insatisfiable(tIntr** p_interpretation) {
	// Parameters check
	if (isNull(*p_interpretation)) {
		fprintf(stderr, " Ooops: Le pointeur d'interprétation est NULL.\n");
		return -1;
	}

	(*p_interpretation)->insatisfiable = UNSATISFIABLE;
	return 0;
}


/**
 * Prints an interpretation to stderr.
 * 
 * @param p_interpretation
 */
void intr_see(tIntr* p_interpretation) {
	// Parameters check
	if (isNull(p_interpretation)) {
		fprintf(stderr, "  Ooops: Le pointeur d'interprétation est NULL.\n");
		return;
	}

	// Tests if unsatisfiable
	if (p_interpretation->insatisfiable == UNSATISFIABLE) {
		fprintf(stderr, "  L'interprétation est insatisfiable.\n");
		return;
	}

	// Browses the literals...
	fprintf(stderr, "  Interprétation = (");
	tLitt* litteral = p_interpretation->deb;
	while (notNull(litteral)) {
		fprintf(stderr, " %sx%d", (litteral->litt < 0 ? "¬" : ""), abs(litteral->litt));
		litteral = litteral->suiv;
	}
	fprintf(stderr, " )\n");
}
