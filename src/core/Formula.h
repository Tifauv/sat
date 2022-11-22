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
#ifndef FORMULA_H
#define FORMULA_H

#include <memory>
#include <vector>
#include <unordered_set>
#include "ClauseBuilder.h"
#include "RawLiteral.h"

using namespace std;

namespace sat {

class ClauseBuilder;
class Clause;
class Literal;
class Variable;


class Formula {
public:
	Formula();

	ClauseBuilder& newClause(Id p_clauseId);
	void createClause(Id p_clauseId, const unique_ptr<vector<RawLiteral>>& p_literals);

	Literal findUnitLiteral() const;

	void addClause(const shared_ptr<Clause>& p_clause);
	void addLiteralToClause(const shared_ptr<Clause>& p_clause, Literal p_literal);
	void removeClause(const shared_ptr<Clause>& p_clause);
	void removeLiteralFromClause(const shared_ptr<Clause>& p_clause, Literal p_literal);

	bool hasClauses() const;
	bool hasVariables() const;

	unordered_set<shared_ptr<Variable>>::iterator beginVariable();
	unordered_set<shared_ptr<Variable>>::iterator endVariable();
	void removeVariable(const shared_ptr<Variable>& p_variable);
	void addVariable(const shared_ptr<Variable>& p_variable);

	void log() const;

protected:
	shared_ptr<Variable> findOrCreateVariable(Id p_variableId);

	void unlinkVariable(const shared_ptr<Clause>& p_clause, const Literal& p_literal);

private:
	unordered_set<shared_ptr<Clause>> m_clauses;
	unordered_set<shared_ptr<Clause>> m_unusedClauses;
	unordered_set<shared_ptr<Variable>> m_variables;
	unordered_set<shared_ptr<Variable>> m_unusedVariables;

	ClauseBuilder m_builder;
};

} // namespace sat

#endif // FORMULA_H
