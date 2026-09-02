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

/*
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

/**
 * Represent the developer journal entry data shared with callers of this public contract.
 */
typedef struct UmiDeveloperJournalEntry {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t sequence;
    UmiDeveloperEventKind kind;
    char operation_id[UMI_DEVELOPER_ID_CAPACITY];
    char message[UMI_DEVELOPER_SUMMARY_CAPACITY];
} UmiDeveloperJournalEntry;

/**
 * Represent the developer journal snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperJournalSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t entry_count;
    uint64_t dropped_count;
    uint64_t next_sequence;
    uint64_t revision;
} UmiDeveloperJournalSnapshot;

/**
 * Represent the developer journal data shared with callers of this public contract.
 */
typedef struct UmiDeveloperJournal UmiDeveloperJournal;

/**
 * Initialise developer journal from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_developer_journal_create(UmiDeveloperJournal **out_journal);
/**
 * Release or reset state held by developer journal so the same storage can be reused
 * safely.
 */
void umi_developer_journal_destroy(UmiDeveloperJournal *journal);

/**
 * Add developer journal only after its inputs and available capacity have been checked.
 */
UmiStatus umi_developer_journal_append(
    UmiDeveloperJournal *journal,
    UmiDeveloperEventKind kind,
    const char *operation_id,
    const char *message,
    UmiDeveloperJournalEntry *out_entry);

/**
 * Find developer journal while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_developer_journal_at(
    const UmiDeveloperJournal *journal,
    size_t index,
    UmiDeveloperJournalEntry *out_entry);

/**
 * Return the number of records represented by developer journal without changing their
 * state.
 */
size_t umi_developer_journal_count(const UmiDeveloperJournal *journal);

/**
 * Provide the developer journal snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_journal_snapshot(
    const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out_snapshot);

/**
 * Release or reset state held by developer journal so the same storage can be reused
 * safely.
 */
void umi_developer_journal_clear(UmiDeveloperJournal *journal);

#ifdef __cplusplus
}
#endif

#endif
