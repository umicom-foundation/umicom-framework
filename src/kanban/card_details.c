/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/card_details.c
 *
 * PURPOSE:
 *   Preserve hidden card fields while applying explicitly reviewed detail edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/kanban/card_details.h"
#include "board_internal.h"
#include <string.h>

UmiStatus UmiKanbanCardDetailsCheck(const UmiKanbanCardDetails *details)
{
    if (details == NULL ||
        !UmiKanbanText(details->title, sizeof details->title, false) ||
        !UmiKanbanText(details->description, sizeof details->description, true) ||
        !UmiKanbanText(details->blocked_reason, sizeof details->blocked_reason, true) ||
        details->priority < UMI_KANBAN_LOW || details->priority > UMI_KANBAN_URGENT)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus FindActive(const UmiKanbanBoard *board, uint64_t revision,
    const char *id, UmiKanbanCardSnapshot *out)
{
    if (board == NULL || !UmiKanbanId(id, false)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_kanban_board_revision(board) != revision) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = umi_kanban_board_find(board, id, out);
    if (status != UMI_STATUS_OK) return status;
    return out->archived ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
UmiStatus UmiKanbanCardDetailsRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardDetailSnapshot *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanCardDetailSnapshot result = {0};
    memcpy(result.card_id, row.card.id, strlen(row.card.id) + 1U);
    result.board_revision = revision;
    memcpy(result.details.title, row.card.title, strlen(row.card.title) + 1U);
    memcpy(result.details.description, row.card.description, strlen(row.card.description) + 1U);
    memcpy(result.details.blocked_reason, row.card.blocked_reason, strlen(row.card.blocked_reason) + 1U);
    result.details.priority = row.card.priority;
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCardDetailsApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardDetails *details)
{
    if (UmiKanbanCardDetailsCheck(details) != UMI_STATUS_OK || !UmiKanbanId(actor, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    /* Read the full card only after matching the reviewed revision. This keeps
     * fields absent from a particular native form owned by the shared board. */
    UmiKanbanEdit edit = {0};
    edit.kind = UMI_KANBAN_UPDATE_CARD;
    edit.card = row.card;
    memset(edit.card.title, 0, sizeof edit.card.title);
    memset(edit.card.description, 0, sizeof edit.card.description);
    memset(edit.card.blocked_reason, 0, sizeof edit.card.blocked_reason);
    memcpy(edit.card.title, details->title, strlen(details->title) + 1U);
    memcpy(edit.card.description, details->description, strlen(details->description) + 1U);
    memcpy(edit.card.blocked_reason, details->blocked_reason, strlen(details->blocked_reason) + 1U);
    edit.card.priority = details->priority;
    return umi_kanban_board_apply(board, revision, actor, &edit, 1U, NULL);
}

UmiStatus UmiKanbanCardChecklistRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardChecklistSnapshot *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanCardChecklistSnapshot result = {0};
    memcpy(result.card_id, row.card.id, strlen(row.card.id) + 1U);
    result.board_revision = revision; result.checklist.count = row.card.check_count;
    for (size_t i = 0U; i < row.card.check_count; ++i) {
        memcpy(result.checklist.checks[i].text, row.card.checks[i].text, strlen(row.card.checks[i].text) + 1U);
        result.checklist.checks[i].complete = row.card.checks[i].complete;
    }
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCardChecklistApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardChecklist *checklist)
{
    if (checklist == NULL || checklist->count > UMI_KANBAN_MAX_CHECKS || !UmiKanbanId(actor, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < checklist->count; ++i)
        if (!UmiKanbanText(checklist->checks[i].text, sizeof checklist->checks[i].text, false) ||
            (checklist->checks[i].complete != 0 && checklist->checks[i].complete != 1))
            return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanEdit edit = {0};
    edit.kind = UMI_KANBAN_UPDATE_CARD; edit.card = row.card;
    edit.card.check_count = checklist->count;
    memset(edit.card.checks, 0, sizeof edit.card.checks);
    for (size_t i = 0U; i < checklist->count; ++i) {
        memcpy(edit.card.checks[i].text, checklist->checks[i].text, strlen(checklist->checks[i].text) + 1U);
        edit.card.checks[i].complete = checklist->checks[i].complete;
    }
    return umi_kanban_board_apply(board, revision, actor, &edit, 1U, NULL);
}
