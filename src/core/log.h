/*  Copyright (c) 2015 Olivier Serve
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
#ifndef LOG_H
#define LOG_H

#define LOG_CATEGORY_LOADER_CNF     "sat.loader.cnf"
#define LOG_CATEGORY_DPLL           "sat.solver.dpll"
#define LOG_CATEGORY_FORMULA        "sat.formula"
#define LOG_CATEGORY_HISTORY        "sat.history"
#define LOG_CATEGORY_INTERPRETATION "sat.interpretation"

#define log_cnf            log4c_category_get(LOG_CATEGORY_LOADER_CNF)
#define log_dpll           log4c_category_get(LOG_CATEGORY_DPLL)
#define log_formula        log4c_category_get(LOG_CATEGORY_FORMULA)
#define log_history        log4c_category_get(LOG_CATEGORY_HISTORY)
#define log_interpretation log4c_category_get(LOG_CATEGORY_INTERPRETATION)

#endif // LOG_H
