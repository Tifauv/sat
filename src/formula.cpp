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
#include "formula.h"

#include <iostream>

// CONSTRUCTORS
/**
 *
 */
Formula::Formula() {
	cerr << "Formula created." << endl;
}


// DESTRUCTORS
/**
 * 
 */
Formula::~Formula() {
	cerr << "Formula deleted." << endl;
}


// METHODS
Clause* Formula::createClause(ClauseId p_clauseId) {
	Clause* clause = new Clause(p_clauseId);
	m_freeClauses.push_back(clause);
	cerr << "Clause " << p_clauseId << " added to the free list." << endl;
	
	return clause;
}


Variable* Formula::createVariable(VariableId p_variableId) {
	Variable* variable = new Variable(p_variableId);
	m_freeVariables.push_back(variable);
	cerr << "Variable x" << p_variableId << " added to the free list." << endl;
	
	return variable;
}


void Formula::linkClause(ClauseId p_clauseId, std::list<Literal>& p_literals) {
	Clause* clause = findFreeClause(p_clauseId);
	if (isNull(clause))
		return;

	for (auto literal = p_literals.cbegin(); literal != p_literals.cend(); ++literal)
		linkClause(clause, *literal);
}


void Formula::linkClause(ClauseId p_clauseId, Literal& p_literal) {
	Clause* clause = findFreeClause(p_clauseId);
	if (isNull(clause))
		return;
	
	linkClause(clause, p_literal);
}


void Formula::linkClause(Clause* p_clause, std::list<Literal>& p_literals) {
	for (auto literal = p_literals.cbegin(); literal != p_literals.cend(); ++literal)
		linkClause(p_clause, *literal);
}


void Formula::linkClause(Clause* p_clause, Literal& p_literal) {
	if (p_clause->isUnused())
		// Move the clause from the unused list to the used one
		m_freeClauses.remove(p_clause);
		m_clauses.push_back(p_clause);
		p_clause->setUsed();
	}

	// Link the clause to the literal
	p_clause.addLiteral()
}