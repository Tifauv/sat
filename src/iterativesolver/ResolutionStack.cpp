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
#include "ResolutionStack.h"

#include <string>
#include "log.h"

namespace sat {
namespace solver {

// CONSTRUCTORS
/**
 * Initializes the stack with the first level.
 */
ResolutionStack::ResolutionStack() {
	nextLevel();
}


// LEVEL MANAGEMENT
/**
 * Creates a new resolution level in the stack.
 * This new level becomes the current one.
 */
void ResolutionStack::nextLevel() {
	m_resolutionLevels.push_back(unique_ptr<ResolutionStackLevel>(new ResolutionStackLevel()));
}


/**
 * Deletes the current level from the stack.
 * The previous level becomes the current one (or none if the stack is empty).
 */
void ResolutionStack::popLevel() {
	m_resolutionLevels.pop_back();
}


/**
 * Gives the current depth (number of levels) of the stack.
 */
stack<unique_ptr<ResolutionStackLevel>>::size_type ResolutionStack::currentLevel() const {
	return m_resolutionLevels.size();
}


// CURRENT LEVEL LITERALS OPERATIONS
/**
 * Appends a literal.
 *
 * @param p_literal
 *            the literal
 */
void ResolutionStack::pushLiteral(Literal p_literal) {
	m_resolutionLevels.back()->pushLiteral(p_literal);
}


/**
 * Gives the last decision literal.
 * This is the first literal from the current level.
 *
 * @return the last literal
 */
Literal ResolutionStack::lastDecisionLiteral() const {
	return m_resolutionLevels.back()->firstLiteral();
}


/**
 * Logs the stack.
 */
void ResolutionStack::logCurrentLiterals() const {
	if (!log_is_info_enabled(log_valuation))
		return;

	string line = "Current literals:";
	for (const auto& level : m_resolutionLevels)
		for (const auto& literal : level->literals())
			line.append("  ").append(literal.isNegative() ? "¬" : "").append("x").append(std::to_string(literal.id()));

	log_info(log_valuation, line.data());
}


// CURRENT LEVEL HISTORY OPERATIONS
/**
 * Stores a 'clause removed' operation to the current level's history.
 *
 * @param p_clause
 *            the clause that is removed from the formula
 */
void ResolutionStack::addClause(const shared_ptr<Clause>& p_clause) {
	m_resolutionLevels.back()->saveRemovedClause(p_clause);
}


/**
 * Stores a 'literal removed from clause' operation to the current level's history.
 *
 * @param p_clause
 *            the clause that is modified
 * @param p_literal
 *            the literal that is remove from that clause
 */
void ResolutionStack::addLiteral(const shared_ptr<Clause>& p_clause, Literal p_literal) {
	m_resolutionLevels.back()->saveRemovedLiteralFromClause(p_clause, p_literal);
}


/**
 * Replays the history stored in the current level.
 *
 * @param p_formula
 *            the formula in which to replay the history
 */
void ResolutionStack::replay(Formula& p_formula) const {
	m_resolutionLevels.back()->replay(p_formula);
}


// VALUATION GENERATION
/**
 * Generates a valuation from the current stack.
 *
 * @return a Valuation
 */
const Valuation ResolutionStack::generateValuation() const {
	Valuation valuation;

	for (const auto& level : m_resolutionLevels)
		for (const auto& literal : level->literals())
			valuation.push(literal);

	return valuation;
}

} // namespace sat::solver
} // namespace sat
