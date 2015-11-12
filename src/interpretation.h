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
#ifndef INTERPRETATION_H
#define INTERPRETATION_H

#include "literal.h"

#define SATISFIABLE   0
#define UNSATISFIABLE 1

typedef struct tLitt tLitt;

// Littéral
struct tLitt {
  Literal litt; // Valeur dans la liste
  tLitt*  suiv; // Ptr sur le suivant
};

// Interprétation
typedef struct {
  int insatisfiable; // Si 1, insatisfiable; sinon satisfiable
  tLitt *deb; // ptr sur début de liste
  tLitt *fin; // ptr sur fin de liste
} tIntr;


/**
 * Creates an empty interpretation.
 * 
 * @return an empty interpretation
 */
tIntr* intr_new();


/**
 * Deletes an interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 */
void intr_free(tIntr** p_interpretation);


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
int intr_push(tIntr* p_interpretation, Literal p_literal);


/**
 * Removes the last literal of the interpretation.
 * 
 * @param p_interpretation
 *            the interpretation
 */          
void intr_poke(tIntr* p_interpretation);


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
int intr_is_insatisfiable(tIntr* p_interpretation);


/**
 * Marks the interpretation as unsatisfiable.
 * 
 * @param p_interpretation
 *            the interpretation
 */
void intr_set_insatisfiable(tIntr* p_interpretation);


/**
 * Prints an interpretation to stderr.
 * 
 * @param p_interpretation
 */
void intr_print(tIntr* p_interpretation);

#endif // INTERPRETATION_H
