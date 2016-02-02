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
#ifndef VARIABLE_H
#define VARIABLE_H

#include <list>
#include "FormulaObject.h"

class Clause;

#define SIGN_POSITIVE  1
#define SIGN_NEGATIVE -1


/**
 * 
 */
class Variable : public FormulaObject {
public:
	explicit Variable(Id p_id);
	~Variable();

	void addOccurence(Clause* p_clauseId, int p_sign);

	bool hasPositiveOccurence() const;
	bool hasNegativeOccurence() const;

	unsigned int countPositiveOccurences() const;
	unsigned int countNegativeOccurences() const;
	unsigned int countOccurences() const;

	std::list<Clause*>::iterator beginOccurence(int p_sign);
	std::list<Clause*>::iterator endOccurence(int p_sign);
	std::list<Clause*>::iterator erase(std::list<Clause*>::iterator p_iterator, int p_sign);

	void removePositiveOccurence(Clause* p_clause);
	void removeNegativeOccurence(Clause* p_clause);

private:
	std::list<Clause*> m_positiveOccurences;
	std::list<Clause*> m_negativeOccurences;
};

#endif // VARIABLE_H
