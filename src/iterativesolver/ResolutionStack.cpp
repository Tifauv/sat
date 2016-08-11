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
#include <log4c.h>
#include "log.h"

namespace sat {
namespace solver {

// CONSTRUCTORS
ResolutionStack::ResolutionStack() {
	nextLevel();
}


// DESTRUCTORS
ResolutionStack::~ResolutionStack() {
	for (auto it = m_resolutionLevels.begin(); it != m_resolutionLevels.end(); it = m_resolutionLevels.erase(it))
		delete *it;
}


// LEVEL MANAGEMENT
void ResolutionStack::nextLevel() {
	m_resolutionLevels.push_back(new ResolutionStackLevel());
}


void ResolutionStack::popLevel() {
	ResolutionStackLevel* level = m_resolutionLevels.back();
	m_resolutionLevels.pop_back();
	delete level;
}


std::stack<ResolutionStackLevel*>::size_type ResolutionStack::currentLevel() const {
	return m_resolutionLevels.size();
}


// CURRENT LEVEL LITERALS OPERATIONS
void ResolutionStack::pushLiteral(Literal p_literal) {
	m_resolutionLevels.back()->pushLiteral(p_literal);
}


Literal ResolutionStack::lastDecisionLiteral() const {
	return m_resolutionLevels.back()->firstLiteral();
}


/**
 * Logs the valuation.
 */
void ResolutionStack::logCurrentLiterals() const {
	if (!log4c_category_is_info_enabled(log_valuation))
		return;

	std::string line = "Current literals:";
	for (const auto& level : m_resolutionLevels)
		for (const auto& literal : level->literals())
			line.append("  ").append(literal.isNegative() ? "¬" : "").append("x").append(std::to_string(literal.id()));

	log4c_category_info(log_valuation, line.data());
}


// CURRENT LEVEL HISTORY OPERATIONS
void ResolutionStack::addClause(Clause* p_clause) {
	m_resolutionLevels.back()->saveRemovedClause(p_clause);
}


void ResolutionStack::addLiteral(Clause* p_clause, Literal p_literal) {
	m_resolutionLevels.back()->saveRemovedLiteralFromClause(p_clause, p_literal);
}


void ResolutionStack::replay(Formula& p_formula) const {
	m_resolutionLevels.back()->replay(p_formula);
}


// VALUATION GENERATION
const Valuation ResolutionStack::generateValuation() const {
	Valuation valuation;

	for (const auto& level : m_resolutionLevels)
		for (const auto& literal : level->literals())
			valuation.push(literal);

	return valuation;
}

} // namespace sat::solver
} // namespace sat
