/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/journal_v2.h
 * PURPOSE: Record an append-only plan/action/evidence journal for Helix v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Each entry hashes the previous entry, so accidental edits or
 * reordering are detected by a deterministic verification pass. */
#ifndef INCLUDE_UMICOM_HELIX_JOURNAL_V2_H
#define INCLUDE_UMICOM_HELIX_JOURNAL_V2_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/helix/types.h"
#include "umicom/helix/v2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiHelixJournalKindV2 {
    UMI_HELIX_JOURNAL_PLAN = 1,
    UMI_HELIX_JOURNAL_ACTION = 2,
    UMI_HELIX_JOURNAL_EVIDENCE = 3,
    UMI_HELIX_JOURNAL_APPROVAL = 4,
    UMI_HELIX_JOURNAL_ROLLBACK = 5
} UmiHelixJournalKindV2;

typedef struct UmiHelixJournalEntryV2 {
    uint64_t sequence;
    UmiHelixJournalKindV2 kind;
    char operation_id[UMI_HELIX_ID_CAPACITY];
    char summary[UMI_HELIX_TEXT_CAPACITY];
    uint64_t previous_hash;
    uint64_t entry_hash;
} UmiHelixJournalEntryV2;

typedef struct UmiHelixJournalV2 {
    UmiHelixJournalEntryV2 entries[UMI_HELIX_V2_MAX_JOURNAL_ENTRIES];
    size_t count;
} UmiHelixJournalV2;

void umi_helix_journal_v2_init(UmiHelixJournalV2 *journal);
UmiStatus umi_helix_journal_v2_append(UmiHelixJournalV2 *journal,
                                     UmiHelixJournalKindV2 kind,
                                     const char *operation_id,
                                     const char *summary);
int umi_helix_journal_v2_verify(const UmiHelixJournalV2 *journal);
const UmiHelixJournalEntryV2 *umi_helix_journal_v2_at(
    const UmiHelixJournalV2 *journal,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
