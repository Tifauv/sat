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
#ifndef RESOLLUTION_STACK_H
#define RESOLLUTION_STACK_H

#include <deque>
#include "ResolutionStackLevel.h"
#include "Valuation.h"

namespace sat {
namespace solver {

/**
 * @brief Explicit storage for the stacked histories.
 * It is needed by the iterative solver as opposed to the recursive one which
 * lets the recursion stack manage it.
 */
class ResolutionStack {
public:
	ResolutionStack();
	~ResolutionStack();

	/* Level management */
	void nextLevel();
	void popLevel();
	std::stack<ResolutionStackLevel*>::size_type currentLevel() const;

	/* Current level literals operations */
	/**
	 * Appends a literal.
	 *
	 * @param p_literal
	 *            the literal
	 */
	void pushLiteral(Literal p_literal);

	/**
	 * Gives the last decision literal.
	 * This is the first literal from the current level.
	 *
	 * @return the last literal
	 */
	Literal lastDecisionLiteral() const;

	void logCurrentLiterals() const;

	/* Current level history operations */
	void addClause(Clause* p_clause);
	void addLiteral(Clause* p_clause, Literal p_literal);
	void replay(Formula& p_formula) const;

	/* Valuation generation */
	const Valuation generateValuation() const;

private:
	/** The histories for each level. */
	std::deque<ResolutionStackLevel*> m_resolutionLevels;
};

} // namespace sat::solver
} // namespace sat

#endif // ResolutionStack_h
