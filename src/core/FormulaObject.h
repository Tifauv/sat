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
#ifndef FORMULA_OBJECT_H
#define FORMULA_OBJECT_H


namespace sat {


/** A variable id is an unsigned int. */
using Id = unsigned int;


/**
 * 
 */
class FormulaObject {
public:
	explicit FormulaObject(Id p_id);

	Id id() const;
	bool isUnused() const;

	void setUnused();
	void setUsed();

	bool operator==(const FormulaObject& p_object);
	bool operator!=(const FormulaObject& p_object);

private:
	/** The object identifier. */
	Id m_id;

	/** Is the object currently used ?*/
	bool m_unused;
};

} // namespace sat

#endif // FORMULA_OBJECT_H
