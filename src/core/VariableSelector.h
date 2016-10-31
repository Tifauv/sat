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
#ifndef VARIABLE_SELECTOR_H
#define VARIABLE_SELECTOR_H

#include <memory>

using namespace std;

namespace sat {

class Formula;
class Variable;

namespace solver {


class VariableSelector {
public:
	/**
	 * @return a pointer to the variable found,
	 *         or nullptr if there is no variable in the formula
	 */
	virtual shared_ptr<Variable> getVariable(Formula& p_formula) = 0;
};

} // namespace sat::solver
} // namespace sat

#endif // VariableSelector_h
