/*  Copyright (c) 2015 Olivier Serve
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
#ifndef VariablePolarityLiteralSelector_h
#define VariablePolarityLiteralSelector_h

#include "LiteralSelector.h"
#include "VariableSelector.h"
#include "PolaritySelector.h"


class VariablePolarityLiteralSelector : public LiteralSelector {
public:
	VariablePolarityLiteralSelector(VariableSelector& p_variableSelector, PolaritySelector& p_polaritySelector);
	~VariablePolarityLiteralSelector();

	Literal getLiteral(Formula& p_formula);

private:
	VariableSelector& m_variableSelector;
	PolaritySelector& m_polaritySelector;
};

#endif // VariablePolarityLiteralSelector_h