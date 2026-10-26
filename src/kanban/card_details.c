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

static UmiStatus ChangeDependency(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *required_card_id, const char *actor, bool add)
{
    if (!UmiKanbanId(required_card_id, false) || !UmiKanbanId(actor, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    size_t found = row.card.dependency_count;
    for (size_t i = 0U; i < row.card.dependency_count; ++i)
        if (!strcmp(row.card.dependencies[i], required_card_id)) { found = i; break; }
    if (add && found < row.card.dependency_count) return UMI_STATUS_ALREADY_EXISTS;
    if (!add && found == row.card.dependency_count) return UMI_STATUS_NOT_FOUND;
    if (add && row.card.dependency_count == UMI_KANBAN_MAX_DEPENDENCIES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiKanbanEdit edit = {0};
    edit.kind = UMI_KANBAN_UPDATE_CARD; edit.card = row.card;
    if (add) {
        char *slot = edit.card.dependencies[edit.card.dependency_count++];
        memset(slot, 0, UMI_KANBAN_ID_SIZE);
        memcpy(slot, required_card_id, strlen(required_card_id) + 1U);
    } else {
        for (size_t i = found + 1U; i < edit.card.dependency_count; ++i)
            memcpy(edit.card.dependencies[i - 1U], edit.card.dependencies[i], UMI_KANBAN_ID_SIZE);
        --edit.card.dependency_count;
        memset(edit.card.dependencies[edit.card.dependency_count], 0, UMI_KANBAN_ID_SIZE);
    }
    return umi_kanban_board_apply(board, revision, actor, &edit, 1U, NULL);
}
UmiStatus UmiKanbanCardDependencyAdd(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *required_card_id, const char *actor)
{ return ChangeDependency(board, revision, card_id, required_card_id, actor, true); }
UmiStatus UmiKanbanCardDependencyRemove(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *required_card_id, const char *actor)
{ return ChangeDependency(board, revision, card_id, required_card_id, actor, false); }


UmiStatus UmiKanbanCardLabelsRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardLabelsSnapshot *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanCardLabelsSnapshot result = {0};
    memcpy(result.card_id, row.card.id, strlen(row.card.id) + 1U);
    result.board_revision = revision; result.labels.count = row.card.label_count;
    for (size_t i = 0U; i < row.card.label_count; ++i)
        memcpy(result.labels.labels[i], row.card.labels[i], strlen(row.card.labels[i]) + 1U);
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCardLabelsApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, const UmiKanbanCardLabels *labels)
{
    if (labels == NULL || labels->count > UMI_KANBAN_MAX_LABELS || !UmiKanbanId(actor, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < labels->count; ++i) {
        if (!UmiKanbanText(labels->labels[i], UMI_KANBAN_ID_SIZE, false))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (!strcmp(labels->labels[j], labels->labels[i])) return UMI_STATUS_ALREADY_EXISTS;
    }
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    /* Reuse the original card so absent fields stay with their canonical owner.
     * The normal transaction also preserves Done and prerequisite invariants. */
    UmiKanbanEdit edit = {0};
    edit.kind = UMI_KANBAN_UPDATE_CARD; edit.card = row.card;
    edit.card.label_count = labels->count;
    memset(edit.card.labels, 0, sizeof edit.card.labels);
    for (size_t i = 0U; i < labels->count; ++i)
        memcpy(edit.card.labels[i], labels->labels[i], strlen(labels->labels[i]) + 1U);
    return umi_kanban_board_apply(board, revision, actor, &edit, 1U, NULL);
}


UmiStatus UmiKanbanCardDeadlineRead(const UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, UmiKanbanCardDeadlineSnapshot *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    UmiKanbanCardDeadlineSnapshot result = {0};
    memcpy(result.card_id, row.card.id, strlen(row.card.id) + 1U);
    result.board_revision = revision; result.due_unix_seconds = row.card.due_unix_seconds;
    *out = result;
    return UMI_STATUS_OK;
}
UmiStatus UmiKanbanCardDeadlineApply(UmiKanbanBoard *board, uint64_t revision,
    const char *card_id, const char *actor, uint64_t due_unix_seconds)
{
    if (!UmiKanbanId(actor, false)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiKanbanCardSnapshot row;
    UmiStatus status = FindActive(board, revision, card_id, &row);
    if (status != UMI_STATUS_OK) return status;
    /* A deadline edits planning metadata only. No scheduling, completion,
     * assignment, timezone conversion or notification occurs in this service. */
    UmiKanbanEdit edit = {0};
    edit.kind = UMI_KANBAN_UPDATE_CARD; edit.card = row.card;
    edit.card.due_unix_seconds = due_unix_seconds;
    return umi_kanban_board_apply(board, revision, actor, &edit, 1U, NULL);
}
