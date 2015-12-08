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
#ifndef HISTORY_H
#define HISTORY_H

#include <list>
#include "Literal.h"

class Formula;
class Clause;


/**
 * An history is a list of steps.
 */
class History {

	/**
	 * The history manages steps for two operations:
	 *   - add a new clause,
	 *   - add a literal to a clause
	 */
	enum class Operation {
		AddClause,
		AddLiteralToClause
	};


	/**
	 * This is a step of the history.
	 */
	class Step {
	public:
		Step(Operation p_operation, Clause* p_clause);
		Step(Operation p_operation, Clause* p_clause, Literal p_literal);

		Operation operation() const;
		Clause*   clause()    const;
		Literal   literal()   const;

	private:
		Operation m_operation;
		Clause*   m_clause;
		Literal   m_literal;
	};


public:
	/**
	 * Creates a new history.
	 */
	History();

	/**
	 * Frees the memory used by an history.
	 */
	~History();

	/**
	 * Adds an operation of type OP_ADD_CLAUSE as last step of the history.
	 *
	 * @param p_clause
	 *            the clause to save
	 */
	void addClause(Clause* p_clause);

	/**
	 * Adds an operation of type OP_ADD_LITERAL_TO_CLAUSE as last step of the history.
	 *
	 * @param p_clause
	 *            the clause
	 * @param p_literal
	 *            the literal to save
	 */
	void addLiteral(Clause* p_clause, Literal p_literal);

	/**
	 * Replays the modifications stored in the history.
	 *
	 * @param p_formula
	 *            the formula in which to replay the operations
	 */
	void replay(Formula& p_formula);

protected:
	/**
	 * Removes and frees the steps of the history.
	 */
	void clear();

private:
	/** The steps of the history. */
	std::list<Step*> m_steps;
};

#endif // HISTORY_H

