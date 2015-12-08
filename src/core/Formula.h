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
#ifndef FORMULA_H
#define FORMULA_H

#include <list>
#include <unordered_set>
#include "RawLiteral.h"

class Clause;
class Literal;
class Variable;
class History;


class Formula {
public:
	Formula();
	~Formula();

	Clause* createClause(Id p_clauseId, std::list<RawLiteral>& p_literals);

	Literal findLiteralFromUnaryClause() const;
	Literal selectLiteral() const;

	void removeClausesWithLiteral(Literal p_literal, History& p_history);
	bool removeOppositeLiteralFromClauses(Literal p_literal, History& p_history);

	void addClause(Clause* p_clause);
	void addLiteralToClause(Clause* p_clause, Literal p_literal);

	bool hasClauses() const;
	bool hasVariables() const;

	std::unordered_set<Variable*>::iterator beginVariable();
	std::unordered_set<Variable*>::iterator endVariable();
	void removeVariable(Variable* p_variable);
	void addVariable(Variable* p_variable);

	void log() const;

protected:
	Variable* findOrCreateVariable(Id p_variableId);

	void unlinkVariable(Clause* p_clause, Literal p_literal);

private:
	std::unordered_set<Clause*> m_clauses;
	std::unordered_set<Clause*> m_unusedClauses;
	std::unordered_set<Variable*> m_variables;
	std::unordered_set<Variable*> m_unusedVariables;
};

#endif // FORMULA_H
