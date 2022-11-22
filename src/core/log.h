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
#ifndef LOG_H
#define LOG_H

#define LOG_CATEGORY_LOADER_CNF "sat.loader.cnf"
#define LOG_CATEGORY_DPLL       "sat.solver.dpll"
#define LOG_CATEGORY_FORMULA    "sat.formula"
#define LOG_CATEGORY_HISTORY    "sat.history"
#define LOG_CATEGORY_VALUATION  "sat.valuation"
#define LOG_SUDOKU              "sudoku"


#ifdef WITHOUT_LOG4C

#define log_cnf
#define log_dpll
#define log_formula
#define log_history
#define log_valuation
#define log_sudoku

#define log_setup()              false
#define log_cleanup()            false

#define log_debug(c, ...)
#define log_info(c, ...)
#define log_warn(c, ...)
#define log_error(c, ...)

#define log_is_debug_enabled(c)  false
#define log_is_info_enabled(c)   false

#else // WITHOUT_LOG4C

#include <log4c.h>

#define log_cnf                  log4c_category_get(LOG_CATEGORY_LOADER_CNF)
#define log_dpll                 log4c_category_get(LOG_CATEGORY_DPLL)
#define log_formula              log4c_category_get(LOG_CATEGORY_FORMULA)
#define log_history              log4c_category_get(LOG_CATEGORY_HISTORY)
#define log_valuation            log4c_category_get(LOG_CATEGORY_VALUATION)
#define log_sudoku               log4c_category_get(LOG_SUDOKU)

#define log_setup()              log4c_init()
#define log_cleanup()            log4c_fini()

#define log_debug(c, ...)        log4c_category_debug(c, __VA_ARGS__)
#define log_info(c, ...)         log4c_category_info(c, __VA_ARGS__)
#define log_warn(c, ...)         log4c_category_warn(c, __VA_ARGS__)
#define log_error(c, ...)        log4c_category_error(c, __VA_ARGS__)

#define log_is_debug_enabled(c)  log4c_category_is_debug_enabled(c)
#define log_is_info_enabled(c)   log4c_category_is_info_enabled(c)

#endif // WITHOUT_LOG4C

#endif // log_h
