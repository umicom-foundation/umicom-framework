/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/completion_plan.c
 *
 * PURPOSE:
 *   Capture explicit review and refuse stale completion before mutating a board.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "completion_plan_internal.h"
#include "publication_plan_internal.h"
#include "board_internal.h"
#include <stdlib.h>
#include <string.h>

void UmiKanbanCompletionPlanDestroy(UmiKanbanCompletionPlan *plan)
{
    if (plan) { free(plan->records); free(plan); }
}
UmiStatus UmiKanbanCompletionPlanRead(const UmiKanbanCompletionPlan *plan,
    UmiKanbanCompletionPlanInfo *out)
{
    if (!plan || !out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = plan->info;
    return UMI_STATUS_OK;
}
const UmiKanbanCompletionRecord *UmiKanbanCompletionPlanAt(
    const UmiKanbanCompletionPlan *plan, size_t index)
{
    return plan && index < plan->info.card_count ? &plan->records[index] : NULL;
}
UmiStatus UmiKanbanCompletionPlanCreate(const UmiKanbanBoard *board, uint64_t revision,
    const char *done_column, const char *reviewer, const char *source_identity,
    const char *configuration, const UmiKanbanCompletionRecord *records, size_t count,
    UmiKanbanCompletionPlan **out)
{
    if (!out) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!board || !records || !count || !UmiKanbanId(done_column, false) ||
        !UmiKanbanId(reviewer, false) || !UmiKanbanPublicText(source_identity, 256U, false) ||
        !UmiKanbanPublicText(configuration, 128U, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_KANBAN_MAX_EDITS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_kanban_board_revision(board) != revision) return UMI_STATUS_INVALID_STATE;
    UmiKanbanConfig config;
    UmiStatus status = umi_kanban_board_config(board, &config);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanCompletionPlan *plan = calloc(1U, sizeof *plan);
    if (!plan) return UMI_STATUS_OUT_OF_MEMORY;
    plan->records = calloc(count, sizeof *plan->records);
    if (!plan->records) { free(plan); return UMI_STATUS_OUT_OF_MEMORY; }
    strcpy(plan->info.board_id, config.id);
    strcpy(plan->info.done_column, done_column);
    strcpy(plan->info.reviewer, reviewer);
    strcpy(plan->info.source_identity, source_identity);
    strcpy(plan->info.configuration, configuration);
    plan->info.reviewed_revision = revision;
    plan->info.card_count = count;
    memcpy(plan->records, records, count * sizeof *records);
    UmiKanbanBoard *copy = NULL;
    status = UmiKanbanCompletionCaptureBoard(board, revision, plan->info.board_sha256, &copy);
    if (status == UMI_STATUS_OK) status = UmiKanbanCompletionValidate(plan);
    if (status == UMI_STATUS_OK) status = UmiKanbanCompletionMove(plan, copy);
    umi_kanban_board_destroy(copy);
    if (status != UMI_STATUS_OK) { UmiKanbanCompletionPlanDestroy(plan); return status; }
    *out = plan;
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCompletionPlanCheck(const UmiKanbanCompletionPlan *plan,
    const UmiKanbanBoard *board, uint64_t revision)
{
    UmiStatus status = UmiKanbanCompletionValidate(plan);
    if (status != UMI_STATUS_OK || !board) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanConfig config;
    status = umi_kanban_board_config(board, &config);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(config.id, plan->info.board_id) || revision != plan->info.reviewed_revision ||
        revision != umi_kanban_board_revision(board))
        return UMI_STATUS_INVALID_STATE;
    char digest[UMI_SHA256_HEX_CAPACITY];
    UmiKanbanBoard *copy = NULL;
    status = UmiKanbanCompletionCaptureBoard(board, revision, digest, &copy);
    if (status == UMI_STATUS_OK && strcmp(digest, plan->info.board_sha256))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiKanbanCompletionMove(plan, copy);
    umi_kanban_board_destroy(copy);
    return status;
}
UmiStatus UmiKanbanCompletionPlanApply(const UmiKanbanCompletionPlan *plan,
    UmiKanbanBoard *board, uint64_t revision)
{
    UmiStatus status = UmiKanbanCompletionPlanCheck(plan, board, revision);
    return status == UMI_STATUS_OK ? UmiKanbanCompletionMove(plan, board) : status;
}
