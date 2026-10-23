/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/view.h
 * PURPOSE: Provide filtered board pages and planning summaries without sharing mutable rows.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KANBAN_VIEW_H
#define UMICOM_KANBAN_VIEW_H
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiKanbanFilter {
    char column_id[UMI_KANBAN_ID_SIZE]; /* Empty means any. */
    char assignee[UMI_KANBAN_ID_SIZE];
    char label[UMI_KANBAN_ID_SIZE];
    char text[256]; /* Case-sensitive byte substring of title or description. */
    UmiKanbanPriority minimum_priority;
    int archived; /* 0: active only; 1: include archived; 2: archived only. */
    int blocked_only; /* Explicit Blocked stage, blocker text or an unfinished prerequisite. */
    int overdue_only; /* Active, not done, with a nonzero deadline before now. */
    uint64_t now_unix_seconds;
} UmiKanbanFilter;
typedef struct UmiKanbanSummary {
    uint64_t board_revision;
    size_t active, archived, done, blocked, overdue;
    size_t columns[UMI_KANBAN_MAX_COLUMNS];
} UmiKanbanSummary;
/* NULL filter selects active cards. Page order follows the board's manual order.
 * Every page must use the same observed revision; a stale page is refused.
 * A zero-capacity page measures total matches without writing rows. Refusal
 * leaves rows, out_count and out_total unchanged. Outputs may not overlap.
 */
UmiStatus umi_kanban_board_page(const UmiKanbanBoard *board, uint64_t expected_revision,
    const UmiKanbanFilter *filter, size_t offset, UmiKanbanCardSnapshot *out_cards,
    size_t capacity, size_t *out_count, size_t *out_total);
UmiStatus umi_kanban_board_summary(const UmiKanbanBoard *board, uint64_t expected_revision,
    uint64_t now_unix_seconds, UmiKanbanSummary *out_summary);
#ifdef __cplusplus
}
#endif
#endif
