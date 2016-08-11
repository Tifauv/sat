/*  Copyright 2016 Olivier Serve
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
#include "PolarityCachingSelector.h"

#include "Literal.h"
#include "utils.h"

namespace sat {
namespace solver {
namespace selectors {

// CONSTRUCTOR
PolarityCachingSelector::PolarityCachingSelector(PolaritySelector& p_defaultSelector) :
	m_defaultSelector(p_defaultSelector) {
}


// PolaritySelector interface
Literal PolarityCachingSelector::getLiteral(Variable* p_variable) {
	// If a polarity has already been cached, return it
	auto iter = m_preferredPolarity.find(p_variable->id());
	if (iter != m_preferredPolarity.end())
		return Literal(p_variable, iter->second);

	// Otherwise, use the default selector
	return m_defaultSelector.getLiteral(p_variable);
}


// SolverListener interface
void PolarityCachingSelector::onAssert(Literal& p_literal) {
	m_preferredPolarity[p_literal.id()] = p_literal.sign();
}

} // namespace sat::solver::selectors
} // namespace sat::solver
} // namespace sat
