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

#include <memory>
#include <vector>
#include "FormulaObject.h"

#define SIGN_POSITIVE  1
#define SIGN_NEGATIVE -1

using namespace std;

namespace sat {

class Clause;


/**
 * 
 */
class Variable : public FormulaObject {
public:
	explicit Variable(Id p_id);

	void addOccurence(shared_ptr<Clause> p_clauseId, int p_sign);

	bool hasPositiveOccurence() const;
	bool hasNegativeOccurence() const;

	unsigned int countPositiveOccurences() const;
	unsigned int countNegativeOccurences() const;
	unsigned int countOccurences() const;

	shared_ptr<Clause> occurence(int p_sign);
	vector<shared_ptr<Clause>>::iterator beginOccurence(int p_sign);
	vector<shared_ptr<Clause>>::iterator endOccurence(int p_sign);

	void removePositiveOccurence(shared_ptr<Clause> p_clause);
	void removeNegativeOccurence(shared_ptr<Clause> p_clause);

private:
	vector<shared_ptr<Clause>> m_positiveOccurences;
	vector<shared_ptr<Clause>> m_negativeOccurences;
};

} // namespace sat

#endif // VARIABLE_H
