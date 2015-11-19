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
#ifndef CLAUSE_H
#define CLAUSE_H

#include <list>
#include "variable.h"

/** A clause identifier. */
typedef unsigned int ClauseId;


/**
 * Represents a Horn clause of a CNF formula.
 */
class Clause {
public:
	Clause(ClauseId p_id);
	~Clause();

private:
	/** The clause identifier. */
	ClauseId m_id;

	/** The list of variables. */
	std::list<Variable*> m_literals;

	/** Tells whether this clause is in the free list. */
	bool m_isUnused;
};
#endif // CLAUSE_H
