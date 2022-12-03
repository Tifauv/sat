/*  Copyright 2022 Olivier Serve
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
#ifndef CLAUSE_BUILDER_H
#define CLAUSE_BUILDER_H

#include <memory>
#include <vector>
# include "RawLiteral.h"


namespace sat {

class Formula;

/**
 * A helper class following the Builder pattern.
 */
class ClauseBuilder {
public:
	/**
	 * Creates an empty satisfiable valuation.
	 */
	explicit ClauseBuilder(Formula& p_formula);

	/**
	 * @brief Copy-constructor for ClauseBuilder
	 *
	 * @param p_builder
	 *            the builder to copy
	 */
	ClauseBuilder(ClauseBuilder& p_builder);

	/**
	 * Prepares the builder for a new clause.
	 *
	 * @param p_clauseId
	 *            the id of the new clause
	 */
	ClauseBuilder& reset(Id p_clauseId);

	/**
	 * Adds a positive literal to the clause.
	 */
	ClauseBuilder& withPositiveLiteral(unsigned int p_variable);

	/**
	 * Adds a negative literal to the clause.
	 */
	ClauseBuilder& withNegativeLiteral(unsigned int p_variable);

	/**
	 * Builds the clause and adds it to the Formula.
	 */
	Formula& build();

private:
	/** The formula to add the clause to. */
	Formula& m_formula;

	/** The clause's id. */
	Id m_clauseId;

	/** The list of literals. */
	std::unique_ptr<std::vector<sat::RawLiteral>> m_literals;
};

} // namespace sat

#endif // CLAUSE_BUILDER_H
