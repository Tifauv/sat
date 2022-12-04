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
#include "VariablePolarityLiteralSelector.h"

#include "Variable.h"
#include "Literal.h"
#include "log.h"
#include "utils.h"


namespace sat {
namespace solver {


// CONSTRUCTORS
VariablePolarityLiteralSelector::VariablePolarityLiteralSelector(VariableSelector& p_variableSelector, PolaritySelector& p_polaritySelector) :
m_variableSelector(p_variableSelector),
m_polaritySelector(p_polaritySelector) {
}


// METHODS
Literal VariablePolarityLiteralSelector::getLiteral(Formula& p_formula) {
	auto variable = m_variableSelector.getVariable(p_formula);

	// No variable found : return a literal pointing to no variable
	if (isNull(variable)) {
		log_error(log_dpll, "There is no more literal in the formula.");
		return Literal();
	}

	// Variable found : select the polarity of the literal
	auto literal = m_polaritySelector.getLiteral(variable);
	log_debug(log_dpll, "Literal %sx%u selected.", (literal.isNegative() ? "¬" : ""), literal.id());
	return literal;
}

} // namespace sat::solver
} // namespace sat
