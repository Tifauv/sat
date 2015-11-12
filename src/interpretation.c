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
#include <log4c.h>

#include "utils.h"
#include "log.h"


/**
 * Creates an empty interpretation.
 * 
 * @return an empty interpretation
 */
tIntr* intr_new() {
	// Création de la liste
	tIntr* interpretation = (tIntr*) malloc(sizeof(tIntr));
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
 */
void intr_free(tIntr** p_interpretation) {
	// Parameters check
	if (isNull(*p_interpretation))
		return;

	// Remove all the list elements
	tLitt* element = (*p_interpretation)->deb;
	while (notNull(element)) {
		intr_poke(*p_interpretation);
		element = (*p_interpretation)->deb;
	}

	free(*p_interpretation);
	*p_interpretation = NULL;
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
	// Parameters check
	if (isNull(p_interpretation)) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_ERROR, "The interpretation pointer is NULL.");
		return -1;
	}

	// Create the element to add
	tLitt* element = (tLitt*) malloc(sizeof(tLitt));
	element->litt = p_literal;
	element->suiv = NULL;

	if (isNull(p_interpretation->deb)) { // interpretation is empty
		p_interpretation->deb = element;
		p_interpretation->fin = element;
	}
	else { // Append the element
		p_interpretation->fin->suiv = element;
		p_interpretation->fin = element;
	}
	log4c_category_log(log_interpretation(), LOG4C_PRIORITY_INFO, "Literal %sx%u added to the interpretation.", (p_literal < 0 ? "¬" : ""), abs(p_literal));

	// Reset the satisfiability status
	if (p_interpretation->insatisfiable == UNSATISFIABLE) {
		p_interpretation->insatisfiable = SATISFIABLE;
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_WARN, "The interpretation was unsatisfiable, and has now been marked satisfiable.");
		return 1;
	}

	return 0;
}


/**
 * Removes the last literal of the interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 */          
void intr_poke(tIntr* p_interpretation) {
	// Parameters check
	if (isNull(p_interpretation)) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_ERROR, "The interpretation pointer is NULL.");
		return;
	}

	if (isNull(p_interpretation->deb)) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_DEBUG, "The interpretation is empty, nothing to remove.");
		return;
	}
 
	// One element only : remove it
	Literal literal = 0;
	if (p_interpretation->deb == p_interpretation->fin) {
		tLitt* element = p_interpretation->deb;
		p_interpretation->deb = NULL;
		p_interpretation->fin = NULL;
		literal = element->litt;
		free(element);
	}
	// List of elements : remove the last element and set the penultimate as new last
	else {
		// The element to remove
		tLitt* lastElement  = p_interpretation->fin;
		
		// Find the penultimate element
		tLitt* currentElement = p_interpretation->deb;
		while (currentElement->suiv != lastElement)
			currentElement = currentElement->suiv;
		
		// Link the penultimate as new last element
		p_interpretation->fin = currentElement;
		p_interpretation->fin->suiv = NULL;
		literal = lastElement->litt;
		free(lastElement);
	}
	log4c_category_log(log_interpretation(), LOG4C_PRIORITY_INFO, "Literal %sx%u removed from the interpretation.", (literal < 0 ? "¬" : ""), abs(literal));
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
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_ERROR, "The interpretation pointer is NULL.");
		return -1;
	}

	return p_interpretation->insatisfiable;
}


/**
 * Marks the interpretation as unsatisfiable.
 * 
 * @param p_interpretation
 *            the interpretation
 */
void intr_set_insatisfiable(tIntr* p_interpretation) {
	// Parameters check
	if (isNull(p_interpretation)) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_ERROR, "The interpretation pointer is NULL.");
		return;
	}

	p_interpretation->insatisfiable = UNSATISFIABLE;
}


/**
 * Prints an interpretation to stderr.
 * 
 * @param p_interpretation
 */
void intr_print(tIntr* p_interpretation) {
	// Parameters check
	if (isNull(p_interpretation)) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_ERROR, "The interpretation pointer is NULL.");
		return;
	}

	// Tests if unsatisfiable
	if (p_interpretation->insatisfiable == UNSATISFIABLE) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_INFO, "The interpretation is unsatisfiable.");
		printf("The interpretation is unsatisfiable.\n");
		return;
	}

	// Browses the literals...
	log4c_category_log(log_interpretation(), LOG4C_PRIORITY_INFO, "Interpretation:");
	printf("Interpretation: (");
	tLitt* litteral = p_interpretation->deb;
	while (notNull(litteral)) {
		log4c_category_log(log_interpretation(), LOG4C_PRIORITY_INFO, "  %sx%u", (litteral->litt < 0 ? "¬" : ""), abs(litteral->litt));
		printf(" %sx%d", (litteral->litt < 0 ? "¬" : ""), abs(litteral->litt));
		litteral = litteral->suiv;
	}
	printf(" )\n");
}
