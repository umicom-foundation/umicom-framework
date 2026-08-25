/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/operation_journal.c
 *
 * PURPOSE:
 *   Implement bounded source-control operation evidence and audit state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/operation_journal.h"

#include <string.h>

void umi_vcs_advanced_operation_journal_init(
    UmiVcsAdvancedOperationJournal *journal)
{
    if (journal == NULL) {
        return;
    }
    (void)memset(journal, 0, sizeof(*journal));
    journal->struct_size = (uint32_t)sizeof(*journal);
    journal->api_version = UMI_VCS_ADVANCED_API_VERSION;
    journal->revision = 1U;
}

UmiStatus umi_vcs_advanced_operation_journal_append(
    UmiVcsAdvancedOperationJournal *journal,
    UmiVcsAdvancedOperationKind kind,
    UmiVcsAdvancedState state,
    const char *reference,
    uint64_t timestamp,
    UmiStatus status)
{
    UmiVcsAdvancedOperationJournalEntry *entry;
    UmiStatus copy_status;

    if (journal == NULL || kind == UMI_VCS_ADVANCED_OPERATION_NONE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (journal->count >= UMI_VCS_ADVANCED_LIST_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry = &journal->entries[journal->count];
    entry->kind = kind;
    entry->state = state;
    entry->timestamp_seconds = timestamp;
    entry->status = status;
    copy_status = umi_vcs_advanced_copy_text(
        entry->reference, sizeof(entry->reference), reference);
    if (copy_status != UMI_STATUS_OK) {
        return copy_status;
    }

    journal->count += 1U;
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiVcsAdvancedOperationJournalEntry *
umi_vcs_advanced_operation_journal_latest(
    const UmiVcsAdvancedOperationJournal *journal)
{
    if (journal == NULL || journal->count == 0U) {
        return NULL;
    }
    return &journal->entries[journal->count - 1U];
}
