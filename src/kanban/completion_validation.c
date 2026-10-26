/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/completion_validation.c
 *
 * PURPOSE:
 *   Admit bounded reviewer records without converting assertions into proof of execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "completion_plan_internal.h"
#include "board_internal.h"
/* Reuse the component's bounded UTF-8/control validation. Private review text
 * needs the same safe plain-text encoding as public display fields. */
#include "publication_plan_internal.h"
#include <string.h>

static bool Digest(const char *text, bool optional)
{
    if (optional && text[0] == '\0') return true;
    for (size_t i = 0U; i < 64U; ++i)
        if (!((text[i] >= '0' && text[i] <= '9') || (text[i] >= 'a' && text[i] <= 'f')))
            return false;
    return text[64] == '\0';
}
UmiStatus UmiKanbanCompletionValidate(const UmiKanbanCompletionPlan *plan)
{
    if (!plan || !UmiKanbanId(plan->info.board_id, false) ||
        !UmiKanbanId(plan->info.done_column, false) || !UmiKanbanId(plan->info.reviewer, false) ||
        !UmiKanbanPublicText(plan->info.source_identity, sizeof plan->info.source_identity, false) ||
        !UmiKanbanPublicText(plan->info.configuration, sizeof plan->info.configuration, false) ||
        !Digest(plan->info.board_sha256, false) || !plan->info.reviewed_revision ||
        !plan->info.card_count || plan->info.card_count > UMI_KANBAN_MAX_EDITS || !plan->records)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < plan->info.card_count; ++i) {
        const UmiKanbanCompletionRecord *r = &plan->records[i];
        if (!UmiKanbanId(r->card_id, false) ||
            (r->decision != UMI_KANBAN_COMPLETION_REVIEWED && r->decision != UMI_KANBAN_COMPLETION_EXCEPTION) ||
            !Digest(r->evidence_sha256, r->decision == UMI_KANBAN_COMPLETION_EXCEPTION) ||
            !UmiKanbanPublicText(r->rationale, sizeof r->rationale, false))
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (!strcmp(r->card_id, plan->records[j].card_id)) return UMI_STATUS_ALREADY_EXISTS;
    }
    return UMI_STATUS_OK;
}
