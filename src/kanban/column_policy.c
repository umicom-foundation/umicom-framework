/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/column_policy.c
 *
 * PURPOSE:
 *   Share revision-bound list-limit inspection and atomic policy edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/column_policy.h"
#include "umicom/kanban/view.h"
#include "board_internal.h"
#include <string.h>

UmiStatus UmiKanbanColumnPolicyRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *column_id, UmiKanbanColumnPolicySnapshot *out)
{
    if (board == NULL || out == NULL || !UmiKanbanId(column_id, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_kanban_board_revision(board) != revision) return UMI_STATUS_INVALID_STATE;
    UmiKanbanConfig config;
    UmiStatus status = umi_kanban_board_config(board, &config);
    if (status != UMI_STATUS_OK) return status;
    size_t column = UmiKanbanColumnFind(&config, column_id);
    if (column == UMI_KANBAN_MISSING) return UMI_STATUS_NOT_FOUND;
    UmiKanbanSummary summary;
    status = umi_kanban_board_summary(board, revision, 0U, &summary);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanColumnPolicySnapshot result = {0};
    result.board_revision = revision; result.column = config.columns[column];
    result.active_cards = summary.columns[column]; result.board_capacity = config.card_capacity;
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanColumnPolicyApply(UmiKanbanBoard *board, uint64_t revision,
    const char *column_id, const char *actor, size_t limit)
{
    if (!UmiKanbanId(actor, false)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanColumnPolicySnapshot snapshot;
    UmiStatus status = UmiKanbanColumnPolicyRead(board, revision, column_id, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    /* Do not implement a second occupancy rule here. Board transactions own
     * capacity checks, activity recording and all-or-nothing publication. */
    UmiKanbanEdit edit = {0};
    edit.kind = UMI_KANBAN_SET_WIP; edit.wip_limit = limit;
    memcpy(edit.column_id, snapshot.column.id, strlen(snapshot.column.id) + 1U);
    return umi_kanban_board_apply(board, revision, actor, &edit, 1U, NULL);
}
