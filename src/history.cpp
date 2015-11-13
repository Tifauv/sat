/* Copyright (C) 2002 Olivier Serve, Mickaël Sibelle & Philippe Strelezki
   Copyright (C) 2015 Olivier Serve

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
#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <log4c.h>

#include "utils.h"
#include "log.h"



/**
 * Creates a new history.
 * 
 * @return a new history
 */
History* sat_history_new() {
	History* history = (History*) malloc(sizeof(History));
	history->last = NULL;
	return history;
}


/**
 * Frees the memory used by an history.
 *
 * @param p_history
 *            the history
 */
void sat_history_free(History** p_history) {
	// Parameters check
	if (isNull(*p_history))
		return;

	// Free each step...
	HistoryStep* step = (*p_history)->last;
	while (step) {
		sat_history_remove_last_step(*p_history);
		step = (*p_history)->last;
	}

	// Free the top structure
	free(*p_history);
	(*p_history) = NULL;
}


/**
 * Checks whether an history is empty.
 *
 * @param p_history
 *            the history to check
 *
 * @return -1 if p_history is NULL,
 *          0 if p_history is not empty,
 *          1 if p_history is empty
 */
int sat_history_is_empty(History* p_history) {
	// Parameters check
	if (isNull(p_history)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The history pointer is NULL.");
		return -1;
	}

	return (p_history->last == NULL);
}


/**
 * Adds an operation of type OP_ADD_CLAUSE as last step of the history.
 *
 * @param p_history
 *            the history
 * @param p_clause
 *            the clause to save
 *
 * @return -3 if p_clause is empty,
 *         -2 if p_clause is NULL,
 *         -1 if p_history is NULL,
 *          0 if the new step was added
 */
int sat_history_add_clause(History* p_history, tClause* p_clause) {
	// Parameters check
	if (isNull(p_history)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The history pointer is NULL.");
		return -1;
	}

	if (isNull(p_clause)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The clause pointer is NULL.");
		return -2;
	}

	// Check the clause is not empty
	if (isNull(p_clause->vars)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The clause to add is empty.");
		return -3;
	}

	// Count the number of literals in the clause
	int nbLiterals = 0;
	tPtVar* literalIter = p_clause->vars;
	while (literalIter) {
		nbLiterals++;
		literalIter = literalIter->suiv;
	}

	// Create the literals array
	Literal* literals = (int*) calloc(nbLiterals, sizeof(int));
	int i = 0;
	literalIter = p_clause->vars;
	while (literalIter) {
		literals[i++] = literalIter->var->indVar * sat_get_sign(literalIter->var, p_clause->indCls);
		literalIter = literalIter->suiv;
	}

	// Create the new step
	HistoryStep* newStep = (HistoryStep*) malloc(sizeof(HistoryStep));
	newStep->operation = OP_ADD_CLAUSE;
	newStep->clauseId = p_clause->indCls;
	newStep->literals = literals;
	newStep->size = nbLiterals;

	// Link the new step
	newStep->next = p_history->last;
	p_history->last = newStep;

	return 0;
}


/**
 * Adds an operation of type OP_ADD_LITERAL_TO_CLAUSE as last step of the history.
 *
 * @param p_history
 *            the history
 * @param p_clauseId
 *            the id of the clause
 * @param p_literal
 *            the literal to save
 *
 * @return -1 if p_history is NULL,
 *          0 if the new step was added
 */
int sat_history_add_literal(History *p_history, ClauseId p_clauseId, Literal p_literal) {
	// Parameters check
	if (isNull(p_history)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The history pointer is NULL.");
		return -1;
	}

	// Create the literals array (of one)
	Literal* literals = (Literal*) malloc(sizeof(Literal));
	literals[0] = p_literal;

	// Create the new step
	HistoryStep* newStep = (HistoryStep*) malloc(sizeof(HistoryStep));
	newStep->operation = OP_ADD_LITERAL_TO_CLAUSE;
	newStep->clauseId = p_clauseId;
	newStep->literals = literals;
	newStep->size = 1;

	// Linking the new step
	newStep->next = p_history->last;
	p_history->last = newStep;

	return 0;
}


/**
 * Removes and frees the last step of the history.
 *
 * @param p_history
 *            the history
 */
void sat_history_remove_last_step(History* p_history) {
	// Parameters check
	if (isNull(p_history)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The history pointer is NULL.");
		return;
	}

	if (isNull(p_history->last)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The history is empty.");
		return;
	}

	HistoryStep* last_step = p_history->last;
	p_history->last = last_step->next;
	free(last_step->literals);
	free(last_step);
}


/**
 * Replays the modifications stored in the history.
 *
 * @param p_history
 *            the operations to replay
 * @param p_formula
 *            the formula in which to replay the operations
 */
void sat_history_replay(History* p_history, tGraphe** p_formula) {
	// Parameters check
	if (isNull(p_history)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_ERROR, "The history pointer is NULL.");
		return;
	}

	if (isNull(*p_formula)) {
		log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "The formula is NULL.");
		(*p_formula) = sat_new();
		log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "New formula created.");
	}

	// Replaying...
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Replaying the history...");
	while (!sat_history_is_empty(p_history)) {
		ClauseId clauseId   = p_history->last->clauseId;
		Literal* literals   = p_history->last->literals;
		size_t literalsSize = p_history->last->size;

		switch (p_history->last->operation) {
		case OP_ADD_CLAUSE:
			sat_add_clause(*p_formula, clauseId, literals, literalsSize);
			log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Added clause %u", clauseId);
			break;

		case OP_ADD_LITERAL_TO_CLAUSE:
			sat_add_var_to_cls(*p_formula, clauseId, *literals);
			log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "Added %sx%u to clause %u", (*literals < 0 ? "¬" : ""), sat_literal_id(*literals), clauseId);
			break;

		default:
			log4c_category_log(log_history(), LOG4C_PRIORITY_WARN, "An unknown operation code (%u) has been found in the history, skipping.", p_history->last->operation);
		}

		sat_history_remove_last_step(p_history);
	}

	// Result
	log4c_category_log(log_history(), LOG4C_PRIORITY_DEBUG, "History replayed.");
	//sat_see(*p_formula);
}

