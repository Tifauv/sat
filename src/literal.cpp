/* Copyright (C) 2015 Olivier Serve
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */
#include "literal.h"


// CONSTRUCTORS
Literal::Literal(Variable* p_variable, int p_sign) :
m_variable(p_variable),
m_sign(p_sign) {
}


// DESTRUCTORS
Literal::~Literal() {
}


// GETTERS
Variable* Literal::var() const {
	return m_variable;
}


VariableId Literal::id() const {
	return m_variable->id();
}


int Literal::sign() const {
	return m_sign;
}


bool Literal::isPositive() const {
	return sign() >= 0;
}


bool Literal::isNegative() const {
	return sign() < 0;
}


// METHODS
std::list<Clause*>::iterator Literal::beginOccurence() {
	return var()->beginOccurence(m_sign);
}


std::list<Clause*>::iterator Literal::endOccurence() {
	return var()->endOccurence(m_sign);
}


std::list<Clause*>::iterator Literal::erase(std::list<Clause*>::iterator p_iterator) {
	return var()->erase(p_iterator, m_sign);
}


std::list<Clause*>::iterator Literal::beginOppositeOccurence() {
	return var()->beginOccurence(-m_sign);
}


std::list<Clause*>::iterator Literal::endOppositeOccurence() {
	return var()->endOccurence(-m_sign);
}


std::list<Clause*>::iterator Literal::eraseOpposite(std::list<Clause*>::iterator p_iterator) {
	return var()->erase(p_iterator, -m_sign);
}


// OPERATORS
Literal Literal::operator-() {
	return Literal(m_variable, -m_sign);
}


bool Literal::operator==(const Literal& p_literal) {
	return id() == p_literal.id();
}


bool Literal::operator!=(const Literal& p_literal) {
	return id() != p_literal.id();
}
