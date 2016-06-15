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
#ifndef SolverStack_h
#define SolverStack_h

#include <stack>
#include "History.h"

namespace sat {
namespace solver {

using namespace history;

/**
 * @brief Explicit storage for the stacked histories.
 * It is needed by the iterative solver as opposed to the recursive one which
 * lets the recursion stack manage it.
 */
class SolverStack {
public:
	SolverStack();
	~SolverStack();

	/* Level management */
	void nextLevel();
	void popLevel();
	std::stack<History*>::size_type currentLevel() const;

	/* Current level operations */
	void addClause(Clause* p_clause);
	void addLiteral(Clause* p_clause, Literal p_literal);
	void replay(Formula& p_formula) const;

private:
	/** The histories for each level. */
	std::stack<History*> m_histories;
};

} // namespace sat::solver
} // namespace sat

#endif // SolverStack_h
