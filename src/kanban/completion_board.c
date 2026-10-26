/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/completion_board.c
 *
 * PURPOSE:
 *   Preview completion against a private board copy using the existing transaction authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "completion_plan_internal.h"
#include "umicom/kanban/archive.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiKanbanCompletionCaptureBoard(const UmiKanbanBoard *board, uint64_t revision,
    char digest[UMI_SHA256_HEX_CAPACITY], UmiKanbanBoard **copy)
{
    *copy = NULL;
    size_t size = 0U;
    UmiStatus status = umi_kanban_board_encode(board, revision, NULL, 0U, &size);
    if (status != UMI_STATUS_OK) return status;
    void *bytes = malloc(size);
    if (!bytes) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_kanban_board_encode(board, revision, bytes, size, &size);
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer(bytes, size, digest);
    uint64_t saved_revision = 0U;
    if (status == UMI_STATUS_OK) status = umi_kanban_board_decode(bytes, size, copy, &saved_revision);
    free(bytes);
    return status;
}
UmiStatus UmiKanbanCompletionMove(const UmiKanbanCompletionPlan *plan, UmiKanbanBoard *board)
{
    UmiKanbanConfig config;
    UmiStatus status = umi_kanban_board_config(board, &config);
    if (status != UMI_STATUS_OK) return status;
    size_t destination = SIZE_MAX;
    for (size_t i = 0U; i < config.column_count; ++i)
        if (!strcmp(config.columns[i].id, plan->info.done_column)) destination = i;
    if (destination == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (config.columns[destination].stage != UMI_KANBAN_DONE) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanEdit *edits = calloc(plan->info.card_count, sizeof *edits);
    if (!edits) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < plan->info.card_count; ++i) {
        UmiKanbanCardSnapshot card;
        status = umi_kanban_board_find(board, plan->records[i].card_id, &card);
        if (status != UMI_STATUS_OK) break;
        if (card.archived) { status = UMI_STATUS_INVALID_STATE; break; }
        for (size_t j = 0U; j < config.column_count; ++j)
            if (!strcmp(config.columns[j].id, card.column_id) && config.columns[j].stage == UMI_KANBAN_DONE)
                status = UMI_STATUS_INVALID_STATE;
        edits[i].kind = UMI_KANBAN_MOVE_CARD;
        strcpy(edits[i].card.id, plan->records[i].card_id);
        strcpy(edits[i].column_id, plan->info.done_column);
    }
    if (status == UMI_STATUS_OK)
        status = umi_kanban_board_apply(board, umi_kanban_board_revision(board),
            plan->info.reviewer, edits, plan->info.card_count, NULL);
    free(edits);
    return status;
}
