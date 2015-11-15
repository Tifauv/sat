#include "literal.h"

#include <stdlib.h>


/**
 * Tells whether a literal is positive or negative.
 * 
 * @param p_literal
 *            the literal
 *
 * @return SIGN_NEGATIVE if p_literal < 0,
 *         SIGN_POSITIVE otherwise
 */
int sat_literal_sign(Literal p_literal) {
	if (p_literal < 0)
		return SIGN_NEGATIVE;
	return SIGN_POSITIVE;
}


/**
 * Gives the id of a literal.
 *
 * @param p_literal
 *            the literal
 *
 * @return the literal's identifier*
 */
LiteralId sat_literal_id(Literal p_literal) {
	return abs(p_literal);
}
