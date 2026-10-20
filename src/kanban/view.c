/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/view.c
 * PURPOSE: Build deterministic board projections from one observed owner revision.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "board_internal.h"
#include "umicom/kanban/view.h"
#include <string.h>
static bool Blocked(const UmiKanbanBoard *board, const UmiKanbanRow *row)
{ return row->card.blocked_reason[0] != '\0' || UmiKanbanUnfinishedDependencies(board, &row->card) != 0U; }
static bool Overdue(const UmiKanbanBoard *board, const UmiKanbanRow *row, uint64_t now)
{
    return !row->archived && board->config.columns[row->column].stage != UMI_KANBAN_DONE &&
        row->card.due_unix_seconds != 0U && row->card.due_unix_seconds < now;
}
static bool Matches(const UmiKanbanBoard *board, const UmiKanbanRow *row, const UmiKanbanFilter *filter)
{
    if ((filter->archived == 0 && row->archived) || (filter->archived == 2 && !row->archived) ||
        row->card.priority < filter->minimum_priority) return false;
    if (filter->column_id[0] && strcmp(filter->column_id, board->config.columns[row->column].id) != 0) return false;
    if (filter->assignee[0] && strcmp(filter->assignee, row->card.assignee) != 0) return false;
    if (filter->text[0] && strstr(row->card.title, filter->text) == NULL &&
        strstr(row->card.description, filter->text) == NULL) return false;
    if (filter->label[0]) {
        bool found = false;
        for (size_t i = 0U; i < row->card.label_count; ++i)
            if (strcmp(filter->label, row->card.labels[i]) == 0) found = true;
        if (!found) return false;
    }
    return (!filter->blocked_only || Blocked(board, row)) &&
        (!filter->overdue_only || Overdue(board, row, filter->now_unix_seconds));
}
UmiStatus umi_kanban_board_page(const UmiKanbanBoard *board, uint64_t expected_revision,
    const UmiKanbanFilter *filter, size_t offset, UmiKanbanCardSnapshot *out_cards,
    size_t capacity, size_t *out_count, size_t *out_total)
{
    if (board == NULL || out_count == NULL || out_total == NULL ||
        (capacity != 0U && out_cards == NULL) || capacity > UMI_KANBAN_MAX_CARDS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (board->revision != expected_revision) return UMI_STATUS_INVALID_STATE;
    UmiKanbanFilter all = {0};
    if (filter == NULL) filter = &all;
    if (!UmiKanbanId(filter->column_id, true) || !UmiKanbanId(filter->assignee, true) ||
        !UmiKanbanText(filter->label, sizeof(filter->label), true) ||
        !UmiKanbanText(filter->text, sizeof(filter->text), true) ||
        filter->minimum_priority < UMI_KANBAN_LOW || filter->minimum_priority > UMI_KANBAN_URGENT ||
        filter->archived < 0 || filter->archived > 2 ||
        (filter->blocked_only != 0 && filter->blocked_only != 1) ||
        (filter->overdue_only != 0 && filter->overdue_only != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t total = 0U, copied = 0U;
    for (size_t i = 0U; i < board->count; ++i) {
        if (!Matches(board, &board->rows[i], filter)) continue;
        if (total >= offset && copied < capacity) UmiKanbanCopyRow(board, i, &out_cards[copied++]);
        ++total;
    }
    *out_count = copied; *out_total = total;
    return UMI_STATUS_OK;
}
UmiStatus umi_kanban_board_summary(const UmiKanbanBoard *board, uint64_t expected_revision,
    uint64_t now_unix_seconds, UmiKanbanSummary *out_summary)
{
    if (board == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (board->revision != expected_revision) return UMI_STATUS_INVALID_STATE;
    UmiKanbanSummary summary = {0};
    summary.board_revision = board->revision;
    for (size_t i = 0U; i < board->count; ++i) {
        const UmiKanbanRow *row = &board->rows[i];
        if (row->archived) { ++summary.archived; continue; }
        ++summary.active; ++summary.columns[row->column];
        if (board->config.columns[row->column].stage == UMI_KANBAN_DONE) ++summary.done;
        if (Blocked(board, row)) ++summary.blocked;
        if (Overdue(board, row, now_unix_seconds)) ++summary.overdue;
    }
    *out_summary = summary;
    return UMI_STATUS_OK;
}
