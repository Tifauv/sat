#ifndef LOG_CATEGORIES_H
#define LOG_CATEGORIES_H

#define LOG_CATEGORY_LOADER "sat.loader"
#define LOG_CATEGORY_LOADER_CNF "sat.loader.cnf"
#define LOG_CATEGORY_HISTORY "sat.history"

#define log_history() log4c_category_get(LOG_CATEGORY_HISTORY)

#endif