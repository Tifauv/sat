/* Copyright (C) 2015 Olivier Serve

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
#ifndef LITERAL_H
#define LITERAL_H


#define SIGN_POSITIVE  1
#define SIGN_NEGATIVE -1

/** A literal id is an unsigned int. */
typedef unsigned int LiteralId;

/** A literal is represented as an int. */
typedef int Literal;


/**
 * Tells whether a literal is positive or negative.
 * 
 * @param p_literal
 *            the literal
 *
 * @return SIGN_NEGATIVE if p_literal < 0,
 *         SIGN_POSITIVE otherwise
 */
int sat_literal_sign(Literal p_literal);


/**
 * Gives the id of a literal.
 *
 * @param p_literal
 *            the literal
 *
 * @return the literal's identifier
 */
LiteralId sat_literal_id(Literal p_literal);

#endif // LITERAL_H
