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
#include "LeastUsedVariableSelector.h"

#include <limits>
#include "Formula.h"
#include "Variable.h"


namespace sat {
namespace solver {
namespace selectors {


// METHODS
std::shared_ptr<Variable> LeastUsedVariableSelector::getVariable(Formula& p_formula) {
	if (!p_formula.hasVariables())
		return nullptr;
	
	auto minimumUse = std::numeric_limits<unsigned int>::max();
	auto selectedVar = std::shared_ptr<Variable>();
	for (auto it = p_formula.beginVariable(); it != p_formula.endVariable(); ++it) {
		if ((*it)->countOccurences() < minimumUse) {
			selectedVar = *it;
			minimumUse = selectedVar->countOccurences();
		}
	}
	
	return selectedVar;
}

} // namespace sat::solver::selectors
} // namespace sat::solver
} // namespace sat
