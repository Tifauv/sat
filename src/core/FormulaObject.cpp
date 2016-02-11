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
#include "FormulaObject.h"


namespace sat {


// CONSTRUCTORS
/**
 *
 */
FormulaObject::FormulaObject(Id p_id) :
	m_id(p_id),
	m_unused(false) {
	
}


// DESTRUCTORS
/**
 *
 */
FormulaObject::~FormulaObject() {
	
}


// METHODS
Id FormulaObject::id() const {
	return m_id;
}


bool FormulaObject::isUnused() const {
	return m_unused;
}


void FormulaObject::setUnused() {
	m_unused = true;
}


void FormulaObject::setUsed() {
	m_unused = false;
}


// OPERATORS
bool FormulaObject::operator==(const FormulaObject& p_object) {
	return id() == p_object.id();
}


bool FormulaObject::operator!=(const FormulaObject& p_object) {
	return id() != p_object.id();
}

} // namespace sat
