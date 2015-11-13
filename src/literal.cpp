#include "literal.h"

#include <stdlib.h>

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
