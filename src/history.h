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
#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>
#include "clause.h"
#include "literal.h"
#include "sat.h"

#define OP_REMOVE_CLAUSE              1
#define OP_REMOVE_LITERAL_FROM_CLAUSE 2

typedef unsigned int Operation;
typedef struct HistoryStep HistoryStep;

/** A History step. */
struct HistoryStep {
  Operation     operation;
  ClauseId      clauseId;
  Literal*      literals;
  size_t        size;
  HistoryStep*  next;
};

/** A History. */
typedef struct {
  HistoryStep* last;
} History;


/**
 * Creates a new history.
 * 
 * @return a new history
 */
History* sat_history_new();


/**
 * Frees the memory used by an history.
 *
 * @param p_history
 *            the history
 *
 * @return -1 if p_history is NULL,
 *          0 if p_history was freed
 */
int sat_history_free(History** p_history);


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
int sat_history_is_empty(History* p_history);


/**
 * Adds an operation of type OP_REMOVE_CLAUSE as last step of the history.
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
int sat_history_add_clause(History* p_history, tClause* p_clause);


/**
 * Adds an operation of type OP_REMOVE_LITERAL_FROM_CLAUSE as last step of the history.
 *
 * @param p_history
 *            the history
 * @param p_clause_id
 *            the id of the clause
 * @param p_literal
 *            the literal to save
 *
 * @return -1 if p_history is NULL,
 *          0 if the new step was added
 */
int sat_history_add_literal(History* p_history, ClauseId p_clauseId, Literal p_literal);


/**
 * Removes and frees the last step of the history.
 *
 * @param p_history
 *            the history
 *
 * @return -2 if p_history is empty,
 *         -1 if p_history is NULL,
 *          0 if the last step could be removed
 */
int sat_history_remove_last_step(History* p_history);


/**
 * Replays the modifications stored in the history.
 *
 * @param p_history
 *            the operations to replay
 * @param p_formula
 *            the formula in which to replay the operations
 *
 * @return -1 if p_history is NULL,
 *         -2 if p_formula is NULL,
 *          0 if the replay was done.
 */
int sat_history_replay(History* p_history, tGraphe** p_formula);

#endif // HISTORY_H
