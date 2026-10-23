/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/transaction.c
 * PURPOSE: Publish bounded reviewed edits atomically without losing the previous board on refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "board_internal.h"
#include <stdlib.h>
#include <string.h>

static void Record(UmiKanbanBoard *board, const UmiKanbanEdit *edit, const char *actor,
    const char *from, const char *to)
{
    size_t index = (board->activity_start + board->activity_count) % UMI_KANBAN_ACTIVITY_CAPACITY;
    if (board->activity_count == UMI_KANBAN_ACTIVITY_CAPACITY)
        board->activity_start = (board->activity_start + 1U) % UMI_KANBAN_ACTIVITY_CAPACITY;
    else ++board->activity_count;
    UmiKanbanActivity *event = &board->activity[index];
    memset(event, 0, sizeof(*event));
    event->sequence = board->next_sequence++;
    event->board_revision = board->revision;
    event->kind = edit->kind;
    if (actor != NULL) memcpy(event->actor, actor, strlen(actor) + 1U);
    if (edit->kind != UMI_KANBAN_SET_WIP)
        memcpy(event->card_id, edit->card.id, strlen(edit->card.id) + 1U);
    if (from != NULL) memcpy(event->from_column, from, strlen(from) + 1U);
    if (to != NULL) memcpy(event->to_column, to, strlen(to) + 1U);
}
/* Move the owned row, preserving all other relative order. Archived rows remain
 * in storage but never serve as a visible insertion anchor. */
static UmiStatus Reorder(UmiKanbanBoard *board, size_t source, size_t column,
    const char *before, UmiKanbanProblem *problem)
{
    size_t destination = board->count;
    if (before[0] != '\0') {
        destination = UmiKanbanFind(board, before);
        if (destination == UMI_KANBAN_MISSING || board->rows[destination].archived ||
            board->rows[destination].column != column)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
                UMI_STATUS_NOT_FOUND, before, board->config.columns[column].id);
        if (destination == source) return UMI_STATUS_OK;
    }
    UmiKanbanRow row = board->rows[source];
    row.column = column;
    if (destination > source) {
        --destination;
        memmove(&board->rows[source], &board->rows[source + 1U],
            (destination - source) * sizeof(*board->rows));
    } else {
        memmove(&board->rows[destination + 1U], &board->rows[destination],
            (source - destination) * sizeof(*board->rows));
    }
    board->rows[destination] = row;
    return UMI_STATUS_OK;
}
/* The dispatcher below adds field-specific assignment through the same checked transaction, preserving other card data and activity ownership. Retained for engineering review. */
#if 0
static UmiStatus Edit(UmiKanbanBoard *board, const UmiKanbanEdit *edit,
    const char *actor, UmiKanbanProblem *problem)
{
    if (edit->kind < UMI_KANBAN_CREATE_CARD || edit->kind > UMI_KANBAN_SET_WIP)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t column = UMI_KANBAN_MISSING;
    if (edit->kind == UMI_KANBAN_SET_WIP || edit->kind == UMI_KANBAN_CREATE_CARD ||
        edit->kind == UMI_KANBAN_MOVE_CARD) {
        if (!UmiKanbanId(edit->column_id, false)) return UMI_STATUS_INVALID_ARGUMENT;
        column = UmiKanbanColumnFind(&board->config, edit->column_id);
        if (column == UMI_KANBAN_MISSING)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
                UMI_STATUS_NOT_FOUND, NULL, edit->column_id);
    }
    if (edit->kind == UMI_KANBAN_SET_WIP) {
        if (edit->wip_limit > board->config.card_capacity) return UMI_STATUS_INVALID_ARGUMENT;
        board->config.columns[column].wip_limit = edit->wip_limit;
        Record(board, edit, actor, NULL, edit->column_id);
        return UMI_STATUS_OK;
    }
    if (!UmiKanbanId(edit->card.id, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((edit->kind == UMI_KANBAN_CREATE_CARD || edit->kind == UMI_KANBAN_UPDATE_CARD) &&
        UmiKanbanCardCheck(&edit->card) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiKanbanFind(board, edit->card.id);
    if (edit->kind == UMI_KANBAN_CREATE_CARD) {
        if (index != UMI_KANBAN_MISSING)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DUPLICATE,
                UMI_STATUS_ALREADY_EXISTS, edit->card.id, edit->column_id);
        if (board->count == board->config.card_capacity)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_CAPACITY,
                UMI_STATUS_CAPACITY_EXCEEDED, edit->card.id, edit->column_id);
        if (board->config.columns[column].stage != UMI_KANBAN_TODO)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_TRANSITION,
                UMI_STATUS_INVALID_STATE, edit->card.id, edit->column_id);
        UmiKanbanRow *row = &board->rows[board->count++];
        row->card = edit->card; row->column = column; row->archived = 0;
        Record(board, edit, actor, NULL, edit->column_id);
        return UMI_STATUS_OK;
    }
    if (index == UMI_KANBAN_MISSING)
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
            UMI_STATUS_NOT_FOUND, edit->card.id, NULL);
    UmiKanbanRow *row = &board->rows[index];
    const char *from = board->config.columns[row->column].id;
    if ((row->archived && edit->kind != UMI_KANBAN_RESTORE_CARD) ||
        (!row->archived && edit->kind == UMI_KANBAN_RESTORE_CARD))
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_ARCHIVED,
            UMI_STATUS_INVALID_STATE, edit->card.id, from);
    if (edit->kind == UMI_KANBAN_UPDATE_CARD) row->card = edit->card;
    else if (edit->kind == UMI_KANBAN_ARCHIVE_CARD) row->archived = 1;
    else if (edit->kind == UMI_KANBAN_RESTORE_CARD) row->archived = 0;
    else if (edit->kind == UMI_KANBAN_MOVE_CARD) {
        if (!UmiKanbanId(edit->before_card_id, true)) return UMI_STATUS_INVALID_ARGUMENT;
        if (column != row->column) {
            if ((board->config.transitions[row->column] & (UINT32_C(1) << column)) == 0U)
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_TRANSITION,
                    UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
            UmiKanbanStage stage = board->config.columns[column].stage;
            if (stage == UMI_KANBAN_DOING || stage == UMI_KANBAN_TESTING) {
                if (row->card.blocked_reason[0] != '\0')
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_BLOCKED,
                        UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
                if (UmiKanbanUnfinishedDependencies(board, &row->card) != 0U)
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DEPENDENCY,
                        UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
            }
        }
        UmiStatus status = Reorder(board, index, column, edit->before_card_id, problem);
        if (status != UMI_STATUS_OK) return status;
    }
    Record(board, edit, actor, from, edit->kind == UMI_KANBAN_MOVE_CARD ? edit->column_id : from);
    return UMI_STATUS_OK;
}
#endif

/* Developer edits below share the atomic candidate board and its revision, so failed card operations cannot leak directory changes. Retained for engineering review. */
#if 0
static UmiStatus Edit(UmiKanbanBoard *board, const UmiKanbanEdit *edit,
    const char *actor, UmiKanbanProblem *problem)
{
    if (edit->kind < UMI_KANBAN_CREATE_CARD || edit->kind > UMI_KANBAN_ASSIGN_CARD)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t column = UMI_KANBAN_MISSING;
    if (edit->kind == UMI_KANBAN_SET_WIP || edit->kind == UMI_KANBAN_CREATE_CARD ||
        edit->kind == UMI_KANBAN_MOVE_CARD) {
        if (!UmiKanbanId(edit->column_id, false)) return UMI_STATUS_INVALID_ARGUMENT;
        column = UmiKanbanColumnFind(&board->config, edit->column_id);
        if (column == UMI_KANBAN_MISSING)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
                UMI_STATUS_NOT_FOUND, NULL, edit->column_id);
    }
    if (edit->kind == UMI_KANBAN_SET_WIP) {
        if (edit->wip_limit > board->config.card_capacity) return UMI_STATUS_INVALID_ARGUMENT;
        board->config.columns[column].wip_limit = edit->wip_limit;
        Record(board, edit, actor, NULL, edit->column_id);
        return UMI_STATUS_OK;
    }
    if (!UmiKanbanId(edit->card.id, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((edit->kind == UMI_KANBAN_CREATE_CARD || edit->kind == UMI_KANBAN_UPDATE_CARD) &&
        UmiKanbanCardCheck(&edit->card) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiKanbanFind(board, edit->card.id);
    if (edit->kind == UMI_KANBAN_CREATE_CARD) {
        if (index != UMI_KANBAN_MISSING)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DUPLICATE,
                UMI_STATUS_ALREADY_EXISTS, edit->card.id, edit->column_id);
        if (board->count == board->config.card_capacity)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_CAPACITY,
                UMI_STATUS_CAPACITY_EXCEEDED, edit->card.id, edit->column_id);
        if (board->config.columns[column].stage != UMI_KANBAN_TODO)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_TRANSITION,
                UMI_STATUS_INVALID_STATE, edit->card.id, edit->column_id);
        UmiKanbanRow *row = &board->rows[board->count++];
        row->card = edit->card; row->column = column; row->archived = 0;
        Record(board, edit, actor, NULL, edit->column_id);
        return UMI_STATUS_OK;
    }
    if (index == UMI_KANBAN_MISSING)
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
            UMI_STATUS_NOT_FOUND, edit->card.id, NULL);
    UmiKanbanRow *row = &board->rows[index];
    const char *from = board->config.columns[row->column].id;
    if ((row->archived && edit->kind != UMI_KANBAN_RESTORE_CARD) ||
        (!row->archived && edit->kind == UMI_KANBAN_RESTORE_CARD))
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_ARCHIVED,
            UMI_STATUS_INVALID_STATE, edit->card.id, from);
    if (edit->kind == UMI_KANBAN_ASSIGN_CARD) {
        if (!UmiKanbanId(edit->card.assignee, true)) return UMI_STATUS_INVALID_ARGUMENT;
        /* Only the assignment changes; caller snapshots cannot overwrite newer
         * descriptions or checklists when the same transaction updates them. */
        memset(row->card.assignee, 0, sizeof row->card.assignee);
        memcpy(row->card.assignee, edit->card.assignee, strlen(edit->card.assignee) + 1U);
    }
    else if (edit->kind == UMI_KANBAN_UPDATE_CARD) row->card = edit->card;
    else if (edit->kind == UMI_KANBAN_ARCHIVE_CARD) row->archived = 1;
    else if (edit->kind == UMI_KANBAN_RESTORE_CARD) row->archived = 0;
    else if (edit->kind == UMI_KANBAN_MOVE_CARD) {
        if (!UmiKanbanId(edit->before_card_id, true)) return UMI_STATUS_INVALID_ARGUMENT;
        if (column != row->column) {
            if ((board->config.transitions[row->column] & (UINT32_C(1) << column)) == 0U)
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_TRANSITION,
                    UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
            UmiKanbanStage stage = board->config.columns[column].stage;
            if (stage == UMI_KANBAN_DOING || stage == UMI_KANBAN_TESTING) {
                if (row->card.blocked_reason[0] != '\0')
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_BLOCKED,
                        UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
                if (UmiKanbanUnfinishedDependencies(board, &row->card) != 0U)
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DEPENDENCY,
                        UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
            }
        }
        UmiStatus status = Reorder(board, index, column, edit->before_card_id, problem);
        if (status != UMI_STATUS_OK) return status;
    }
    Record(board, edit, actor, from, edit->kind == UMI_KANBAN_MOVE_CARD ? edit->column_id : from);
    return UMI_STATUS_OK;
}
#endif

static UmiStatus Edit(UmiKanbanBoard *board, const UmiKanbanEdit *edit,
    const char *actor, UmiKanbanProblem *problem)
{
    if (edit->kind < UMI_KANBAN_CREATE_CARD || edit->kind > UMI_KANBAN_PUT_DEVELOPER)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (edit->kind == UMI_KANBAN_PUT_DEVELOPER) {
        UmiStatus status = UmiKanbanDeveloperPut(board, edit->card.id, edit->card.title);
        if (status == UMI_STATUS_OK) Record(board, edit, actor, NULL, NULL);
        else if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_CAPACITY, status, NULL, NULL);
        return status;
    }
    size_t column = UMI_KANBAN_MISSING;
    if (edit->kind == UMI_KANBAN_SET_WIP || edit->kind == UMI_KANBAN_CREATE_CARD ||
        edit->kind == UMI_KANBAN_MOVE_CARD) {
        if (!UmiKanbanId(edit->column_id, false)) return UMI_STATUS_INVALID_ARGUMENT;
        column = UmiKanbanColumnFind(&board->config, edit->column_id);
        if (column == UMI_KANBAN_MISSING)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
                UMI_STATUS_NOT_FOUND, NULL, edit->column_id);
    }
    if (edit->kind == UMI_KANBAN_SET_WIP) {
        if (edit->wip_limit > board->config.card_capacity) return UMI_STATUS_INVALID_ARGUMENT;
        board->config.columns[column].wip_limit = edit->wip_limit;
        Record(board, edit, actor, NULL, edit->column_id);
        return UMI_STATUS_OK;
    }
    if (!UmiKanbanId(edit->card.id, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((edit->kind == UMI_KANBAN_CREATE_CARD || edit->kind == UMI_KANBAN_UPDATE_CARD) &&
        UmiKanbanCardCheck(&edit->card) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = UmiKanbanFind(board, edit->card.id);
    if (edit->kind == UMI_KANBAN_CREATE_CARD) {
        if (index != UMI_KANBAN_MISSING)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DUPLICATE,
                UMI_STATUS_ALREADY_EXISTS, edit->card.id, edit->column_id);
        if (board->count == board->config.card_capacity)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_CAPACITY,
                UMI_STATUS_CAPACITY_EXCEEDED, edit->card.id, edit->column_id);
        if (board->config.columns[column].stage != UMI_KANBAN_TODO)
            return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_TRANSITION,
                UMI_STATUS_INVALID_STATE, edit->card.id, edit->column_id);
        UmiKanbanRow *row = &board->rows[board->count++];
        row->card = edit->card; row->column = column; row->archived = 0;
        Record(board, edit, actor, NULL, edit->column_id);
        return UMI_STATUS_OK;
    }
    if (index == UMI_KANBAN_MISSING)
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_MISSING,
            UMI_STATUS_NOT_FOUND, edit->card.id, NULL);
    UmiKanbanRow *row = &board->rows[index];
    const char *from = board->config.columns[row->column].id;
    if ((row->archived && edit->kind != UMI_KANBAN_RESTORE_CARD) ||
        (!row->archived && edit->kind == UMI_KANBAN_RESTORE_CARD))
        return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_ARCHIVED,
            UMI_STATUS_INVALID_STATE, edit->card.id, from);
    if (edit->kind == UMI_KANBAN_ASSIGN_CARD) {
        if (!UmiKanbanId(edit->card.assignee, true)) return UMI_STATUS_INVALID_ARGUMENT;
        /* Only the assignment changes; caller snapshots cannot overwrite newer
         * descriptions or checklists when the same transaction updates them. */
        memset(row->card.assignee, 0, sizeof row->card.assignee);
        memcpy(row->card.assignee, edit->card.assignee, strlen(edit->card.assignee) + 1U);
    }
    else if (edit->kind == UMI_KANBAN_UPDATE_CARD) row->card = edit->card;
    else if (edit->kind == UMI_KANBAN_ARCHIVE_CARD) row->archived = 1;
    else if (edit->kind == UMI_KANBAN_RESTORE_CARD) row->archived = 0;
    else if (edit->kind == UMI_KANBAN_MOVE_CARD) {
        if (!UmiKanbanId(edit->before_card_id, true)) return UMI_STATUS_INVALID_ARGUMENT;
        if (column != row->column) {
            if ((board->config.transitions[row->column] & (UINT32_C(1) << column)) == 0U)
                return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_TRANSITION,
                    UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
            UmiKanbanStage stage = board->config.columns[column].stage;
            if (stage == UMI_KANBAN_DOING || stage == UMI_KANBAN_TESTING) {
                if (row->card.blocked_reason[0] != '\0')
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_BLOCKED,
                        UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
                if (UmiKanbanUnfinishedDependencies(board, &row->card) != 0U)
                    return UmiKanbanProblemSet(problem, UMI_KANBAN_PROBLEM_DEPENDENCY,
                        UMI_STATUS_INVALID_STATE, row->card.id, edit->column_id);
            }
        }
        UmiStatus status = Reorder(board, index, column, edit->before_card_id, problem);
        if (status != UMI_STATUS_OK) return status;
    }
    Record(board, edit, actor, from, edit->kind == UMI_KANBAN_MOVE_CARD ? edit->column_id : from);
    return UMI_STATUS_OK;
}
UmiStatus umi_kanban_board_apply(UmiKanbanBoard *board, uint64_t expected_revision,
    const char *actor, const UmiKanbanEdit *edits, size_t count, UmiKanbanProblem *out_problem)
{
    UmiKanbanProblem problem = {0};
    problem.edit_index = SIZE_MAX;
    UmiStatus status = UMI_STATUS_OK;
    UmiKanbanBoard *candidate = NULL;
    if (board == NULL || (count != 0U && edits == NULL) ||
        (actor != NULL && !UmiKanbanId(actor, true))) {
        status = UMI_STATUS_INVALID_ARGUMENT; problem.kind = UMI_KANBAN_PROBLEM_ARGUMENT;
    } else if (expected_revision != board->revision) {
        status = UMI_STATUS_INVALID_STATE; problem.kind = UMI_KANBAN_PROBLEM_STALE;
    } else if (count > UMI_KANBAN_MAX_EDITS || (count != 0U &&
        (board->revision == UINT64_MAX || count > UINT64_MAX - board->next_sequence))) {
        status = UMI_STATUS_CAPACITY_EXCEEDED; problem.kind = UMI_KANBAN_PROBLEM_CAPACITY;
    } else if (count != 0U) {
        /* Allocate the candidate before any live mutation. Neither callbacks nor
         * I/O occur between validation and publication on the owning thread. */
        candidate = malloc(sizeof(*candidate));
        if (candidate == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else {
            *candidate = *board;
            candidate->rows = malloc(board->config.card_capacity * sizeof(*board->rows));
            if (candidate->rows == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                memcpy(candidate->rows, board->rows, board->count * sizeof(*board->rows));
                ++candidate->revision;
                for (size_t i = 0U; i < count; ++i) {
                    problem.edit_index = i;
                    status = Edit(candidate, &edits[i], actor, &problem);
                    if (status != UMI_STATUS_OK) break;
                }
                if (status == UMI_STATUS_OK) {
                    problem.edit_index = SIZE_MAX;
                    status = UmiKanbanValidate(candidate, &problem);
                }
                if (status == UMI_STATUS_OK) {
                    free(board->rows);
                    *board = *candidate;
                    candidate->rows = NULL;
                }
            }
        }
    }
    if (status != UMI_STATUS_OK && problem.kind == UMI_KANBAN_PROBLEM_NONE)
        problem.kind = status == UMI_STATUS_OUT_OF_MEMORY ? UMI_KANBAN_PROBLEM_CAPACITY : UMI_KANBAN_PROBLEM_ARGUMENT;
    if (out_problem != NULL) *out_problem = problem;
    umi_kanban_board_destroy(candidate);
    return status;
}
