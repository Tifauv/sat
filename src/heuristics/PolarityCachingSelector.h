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
#ifndef POLARITY_CACHING_SELECTOR_H
#define POLARITY_CACHING_SELECTOR_H

#include <unordered_map>
#include "PolaritySelector.h"
#include "NoopSolverListener.h"

using namespace std;

class Variable;
class Literal;


namespace sat {
namespace solver {
namespace selectors {

class PolarityCachingSelector : public PolaritySelector, public listeners::NoopSolverListener {
public:
	PolarityCachingSelector(PolaritySelector& p_defaultSelector);

	Literal getLiteral(shared_ptr<Variable> p_variable) override;

	void onAssert(Literal& p_literal) override;

private:
	/** The cache of polarities used. */
	unordered_map<unsigned int, int> m_preferredPolarity;

	/** The default polarity selector. */
	PolaritySelector& m_defaultSelector;
};

} // namespace sat::solver::selectors
} // namespace sat::solver
} // namespace sat

#endif // POLARITYCACHINGSELECTOR_H
