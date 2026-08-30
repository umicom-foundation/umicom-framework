/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/journal.h
 *
 * PURPOSE:
 *   Record a bounded chronological journal of developer-runtime lifecycle
 *   events.  The journal provides deterministic sequence numbers for replay,
 *   diagnostics, tests and product-neutral activity views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * When the journal reaches capacity it keeps accepting new events by replacing
 * the oldest record.  dropped_count tells callers how many old events were
 * displaced, so loss is explicit rather than silent.
 */
#ifndef UMICOM_DEVELOPER_JOURNAL_H
#define UMICOM_DEVELOPER_JOURNAL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_JOURNAL_CAPACITY 4096U
#define UMI_DEVELOPER_JOURNAL_API_VERSION 1U

typedef struct UmiDeveloperJournalEntry {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t sequence;
    UmiDeveloperEventKind kind;
    char operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char message[UMI_DEVELOPER_SUMMARY_CAPACITY];
} UmiDeveloperJournalEntry;

typedef struct UmiDeveloperJournalSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t entry_count;
    uint64_t dropped_count;
    uint64_t next_sequence;
    uint64_t revision;
} UmiDeveloperJournalSnapshot;

typedef struct UmiDeveloperJournal UmiDeveloperJournal;

UmiStatus umi_developer_journal_create(UmiDeveloperJournal **out_journal);
void umi_developer_journal_destroy(UmiDeveloperJournal *journal);

UmiStatus umi_developer_journal_append(
    UmiDeveloperJournal *journal,
    UmiDeveloperEventKind kind,
    const char *operation_id,
    const char *message,
    UmiDeveloperJournalEntry *out_entry);

UmiStatus umi_developer_journal_at(
    const UmiDeveloperJournal *journal,
    size_t index,
    UmiDeveloperJournalEntry *out_entry);

size_t umi_developer_journal_count(const UmiDeveloperJournal *journal);

UmiStatus umi_developer_journal_snapshot(
    const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out_snapshot);

void umi_developer_journal_clear(UmiDeveloperJournal *journal);

#ifdef __cplusplus
}
#endif

#endif
