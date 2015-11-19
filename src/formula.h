/* Copyright (C) 2015 Olivier Serve

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
*/
#ifndef FORMULA_H
#define FORMULA_H

#include <list>
#include "clause.h"
#include "literal.h"

typedef struct Clause Clause;
typedef struct Variable Variable;

struct Clause {
	ClauseId id;
	std::list<Variable*> literals;
	bool isFree;
};

struct Variable {
	LiteralId id;
	std::list<Clause*> positiveOccurences;
	std::list<Clause*> negativeOccurences;
	bool isFree;
};

struct Literal {
	Variable* variable;
	int sign;
} Literal;

class Formula {
public:
	Formula();
	~Formula();

	Clause*   createClause(ClauseId p_clauseId);
	Variable* createVariable(LiteralId p_variableId);

	void linkClause(ClauseId p_clauseId, std::list<Literal>& p_literals);
	void linkClause(ClauseId p_clauseId, Literal p_literal);
	void linkClause(Clause* p_clause, std::list<Literal>& p_literals);
	void linkClause(Clause* p_clause, Literal p_literal);

	Literal findLiteralFromUnaryClause() const;
	Literal getFirstLiteral() const;

	void unlinkClause(ClauseId p_clauseId);
	void unlinkClause(Clause* p_clause);

	void unlinkLiteralFromClause(ClauseId p_clauseId, Literal& p_literal);
	void unlinkLiteralFromClause(Clause* p_clause, Literal& p_literal);

protected:
	Clause*   findClause(ClauseId p_clauseId) const;
	Clause*   findFreeClause(ClauseId p_clauseId) const;
	Variable* findVariable(VariableId p_variableId) const;
	Variable* findFreeVariable(VariableId p_variableId) const;
	
private:
	std::list<Clause*> m_clauses;
	std::list<Clause*> m_freeClauses;
	std::list<Variable*> m_variables;
	std::list<Variable*> m_freeVariables;
}

#endif // FORMULA_H