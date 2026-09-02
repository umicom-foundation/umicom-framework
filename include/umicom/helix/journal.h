/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/journal.h
 * PURPOSE: Record an append-only plan/action/evidence journal for Helix v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Each entry hashes the previous entry, so accidental edits or
 * reordering are detected by a deterministic verification pass. */
#ifndef INCLUDE_UMICOM_HELIX_JOURNAL_H
#define INCLUDE_UMICOM_HELIX_JOURNAL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/execution_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named helix journal kind values accepted by this public contract.
 */
typedef enum UmiHelixJournalKind {
    UMI_HELIX_JOURNAL_PLAN = 1,
    UMI_HELIX_JOURNAL_ACTION = 2,
    UMI_HELIX_JOURNAL_EVIDENCE = 3,
    UMI_HELIX_JOURNAL_APPROVAL = 4,
    UMI_HELIX_JOURNAL_ROLLBACK = 5
} UmiHelixJournalKind;

/**
 * Represent the helix journal entry data shared with callers of this public contract.
 */
typedef struct UmiHelixJournalEntry {
    uint64_t sequence;
    UmiHelixJournalKind kind;
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    uint64_t previous_hash;
    uint64_t entry_hash;
} UmiHelixJournalEntry;

/**
 * Represent the helix journal data shared with callers of this public contract.
 */
typedef struct UmiHelixJournal {
    UmiHelixJournalEntry entries[UMI_HELIX_RUNTIME_MAX_JOURNAL_ENTRIES];
    size_t count;
} UmiHelixJournal;

/**
 * Initialise helix journal from caller-provided values so later operations receive a known
 * state.
 */
void umi_helix_journal_init(UmiHelixJournal *journal);
/**
 * Add helix journal only after its inputs and available capacity have been checked.
 */
UmiStatus umi_helix_journal_append(UmiHelixJournal *journal,
                                     UmiHelixJournalKind kind,
                                     const char *operation_id,
                                     const char *summary);
/**
 * Provide the helix journal verify operation used by this module and its client
 * applications.
 */
int umi_helix_journal_verify(const UmiHelixJournal *journal);
/**
 * Find helix journal while leaving the underlying catalogue or model owned by this module.
 */
const UmiHelixJournalEntry *umi_helix_journal_at(
    const UmiHelixJournal *journal,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
