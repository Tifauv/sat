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
#include "LoggingListener.h"

#include "log4c.h"
#include "log.h"
#include "Literal.h"


namespace sat {
namespace solver {
namespace listeners {

// INTERFACE METHODS
void LoggingListener::init() {
	log4c_category_info(log_dpll, "Solver initialized.");
}


void LoggingListener::onDecide(Literal& p_literal) {
	log4c_category_info(log_dpll, "Decided to use literal %sx%u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
}


void LoggingListener::onPropagate(Literal& p_literal) {
	log4c_category_info(log_dpll, "Propagated literal %sx%u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
}


void LoggingListener::onAssert(Literal& p_literal) {
	log4c_category_info(log_dpll, "Asserted literal %sx%u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
}


void LoggingListener::onConflict(Clause* p_clause) {
	log4c_category_info(log_dpll, "Clause #%u generated a conflict.", p_clause->id());
}


void LoggingListener::onBacktrack(Literal& p_literal) {
	log4c_category_info(log_dpll, "Backtracked literal %sx%u.", (p_literal.isNegative() ? "¬" : ""), p_literal.id());
}


void LoggingListener::cleanup() {
	log4c_category_info(log_dpll, "Solver cleaned.");
}

} // namespace sat::solver::listeners
} // namespace sat::solver
} // namespace sat
