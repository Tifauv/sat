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
#include "Variable.h"

#include <algorithm>
#include <log4c.h>
#include "Clause.h"
#include "utils.h"
#include "log.h"


namespace sat {


// CONSTRUCTORS
/**
 *
 */
Variable::Variable(Id p_id) :
FormulaObject(p_id) {
}


// METHODS
void Variable::addOccurence(shared_ptr<Clause> p_clause, int p_sign) {
	// Parameters check
	if (isNull(p_clause)) {
		log4c_category_error(log_formula, "Attempted to add a NULL clause to variable x%u.", id());
		return;
	}
	
	switch (p_sign) {
		case SIGN_POSITIVE:
			m_positiveOccurences.push_back(p_clause);
			log4c_category_debug(log_formula, "Clause %u added to positive occurences of variable x%u.", p_clause->id(), id());
			break;
		
		case SIGN_NEGATIVE:
			m_negativeOccurences.push_back(p_clause);
			log4c_category_debug(log_formula, "Clause %u added to negative occurences of variable x%u.", p_clause->id(), id());
			break;
		
		default:
			log4c_category_error(log_formula, "Sign '%d' is invalid.", p_sign);
			// Error Invalid sign
	}
}


bool Variable::hasPositiveOccurence() const {
	return !m_positiveOccurences.empty();
}


bool Variable::hasNegativeOccurence() const {
	return !m_negativeOccurences.empty();
}


unsigned int Variable::countPositiveOccurences() const {
	return m_positiveOccurences.size();
}


unsigned int Variable::countNegativeOccurences() const {
	return m_negativeOccurences.size();
}


unsigned int Variable::countOccurences() const {
	return countPositiveOccurences() + countNegativeOccurences();
}


shared_ptr<Clause> Variable::occurence(int p_sign) {
	if (p_sign == SIGN_POSITIVE) {
		if (m_positiveOccurences.empty())
			return nullptr;
		return m_positiveOccurences.front();
	}
	
	if (m_negativeOccurences.empty())
		return nullptr;
	return m_negativeOccurences.front();
}


vector<shared_ptr<Clause>>::iterator Variable::beginOccurence(int p_sign) {
	if (p_sign == SIGN_POSITIVE)
		return m_positiveOccurences.begin();
	return m_negativeOccurences.begin();
}


vector<shared_ptr<Clause>>::iterator Variable::endOccurence(int p_sign) {
	if (p_sign == SIGN_POSITIVE)
		return m_positiveOccurences.end();
	return m_negativeOccurences.end();
}


void Variable::removePositiveOccurence(shared_ptr<Clause> p_clause) {
	m_positiveOccurences.erase(
		remove(
			m_positiveOccurences.begin(),
			m_positiveOccurences.end(),
			p_clause),
		m_positiveOccurences.end());
}


void Variable::removeNegativeOccurence(shared_ptr<Clause> p_clause) {
	m_negativeOccurences.erase(
		remove(
			m_negativeOccurences.begin(),
			m_negativeOccurences.end(),
			p_clause),
		m_negativeOccurences.end());
}

} // namespace sat
