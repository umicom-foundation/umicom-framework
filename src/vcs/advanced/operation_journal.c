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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/operation_journal.h"

#include <string.h>

/*
 * Initialise vcs advanced operation journal from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_operation_journal_init(
    UmiVcsAdvancedOperationJournal *journal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL) {
        return;
    }
    (void)memset(journal, 0, sizeof(*journal));
    journal->struct_size = (uint32_t)sizeof(*journal);
    journal->api_version = UMI_VCS_ADVANCED_API_VERSION;
    journal->revision = 1U;
}

/*
 * Add vcs advanced operation journal only after its inputs and available capacity have
 * been checked.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || kind == UMI_VCS_ADVANCED_OPERATION_NONE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (copy_status != UMI_STATUS_OK) {
        return copy_status;
    }

    journal->count += 1U;
    journal->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced operation journal latest operation used by this module and its
 * client applications.
 */
const UmiVcsAdvancedOperationJournalEntry *
umi_vcs_advanced_operation_journal_latest(
    const UmiVcsAdvancedOperationJournal *journal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || journal->count == 0U) {
        return NULL;
    }
    return &journal->entries[journal->count - 1U];
}
