/*  Copyright 2015 Olivier Serve
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */
#include "MostUsedPolaritySelector.h"

#include "Variable.h"
#include "Literal.h"


namespace sat {
namespace solver {
namespace selectors {


// METHODS
Literal MostUsedPolaritySelector::getLiteral(const shared_ptr<Variable>& p_variable) {
	if (p_variable == nullptr) {
		return Literal(nullptr, SIGN_POSITIVE);
	}

	int sign = SIGN_POSITIVE;
	if (p_variable->countNegativeOccurences() > p_variable->countPositiveOccurences())
		sign = SIGN_NEGATIVE;
	return Literal(p_variable, sign);
}

} // namespace sat::solver::selectors
} // namespace sat::solver
} // namespace sat
