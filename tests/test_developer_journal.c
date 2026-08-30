/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_journal.c
 *
 * PURPOSE:
 *   Verify chronological event ordering and explicit ring-buffer displacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>

#include "umicom/developer/journal.h"

int main(void)
{
    UmiDeveloperJournal *journal = NULL;
    UmiDeveloperJournalEntry entry;
    UmiDeveloperJournalSnapshot snapshot;
    size_t index;

    assert(umi_developer_journal_create(&journal) == UMI_STATUS_OK);

    for (index = 0U; index < UMI_DEVELOPER_JOURNAL_CAPACITY + 1U; ++index) {
        assert(umi_developer_journal_append(
            journal,
            UMI_DEVELOPER_EVENT_PROGRESS,
            "operation",
            "event",
            NULL) == UMI_STATUS_OK);
    }

    assert(umi_developer_journal_snapshot(journal, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.entry_count == UMI_DEVELOPER_JOURNAL_CAPACITY);
    assert(snapshot.dropped_count == 1U);
    assert(snapshot.next_sequence ==
           (uint64_t)UMI_DEVELOPER_JOURNAL_CAPACITY + 2U);

    assert(umi_developer_journal_at(journal, 0U, &entry) == UMI_STATUS_OK);
    assert(entry.sequence == 2U);
    assert(umi_developer_journal_at(
        journal, UMI_DEVELOPER_JOURNAL_CAPACITY - 1U, &entry) == UMI_STATUS_OK);
    assert(entry.sequence ==
           (uint64_t)UMI_DEVELOPER_JOURNAL_CAPACITY + 1U);

    umi_developer_journal_clear(journal);
    assert(umi_developer_journal_count(journal) == 0U);

    umi_developer_journal_destroy(journal);
    return 0;
}
