/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/journal.c
 * PURPOSE: Implement the deterministic hash-linked Helix v2 journal.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/journal.h"

#include <stdio.h>
#include <string.h>

/* Provide the hash bytes operation used by this module and its client applications. */
static uint64_t hash_bytes(uint64_t value, const unsigned char *bytes, size_t size)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < size; ++index) {
        value ^= (uint64_t)bytes[index];
        value *= UINT64_C(1099511628211);
    }
    return value;
}

/* Provide the entry hash operation used by this module and its client applications. */
static uint64_t entry_hash(uint64_t sequence, UmiHelixJournalKind kind,
                           const char *operation_id, const char *summary,
                           uint64_t previous_hash)
{
    uint64_t value = UINT64_C(1469598103934665603);
    value = hash_bytes(value, (const unsigned char *)&sequence, sizeof(sequence));
    value = hash_bytes(value, (const unsigned char *)&kind, sizeof(kind));
    value = hash_bytes(value, (const unsigned char *)operation_id,
                       strlen(operation_id));
    value = hash_bytes(value, (const unsigned char *)summary, strlen(summary));
    return hash_bytes(value, (const unsigned char *)&previous_hash,
                      sizeof(previous_hash));
}

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/*
 * Initialise helix journal from caller-provided values so later operations receive a known
 * state.
 */
void umi_helix_journal_init(UmiHelixJournal *journal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal != NULL) (void)memset(journal, 0, sizeof(*journal));
}

/* Add helix journal only after its inputs and available capacity have been checked. */
UmiStatus umi_helix_journal_append(UmiHelixJournal *journal,
                                     UmiHelixJournalKind kind,
                                     const char *operation_id,
                                     const char *summary)
{
    UmiHelixJournalEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL || operation_id == NULL || summary == NULL ||
        kind < UMI_HELIX_JOURNAL_PLAN || kind > UMI_HELIX_JOURNAL_ROLLBACK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (journal->count >= UMI_HELIX_RUNTIME_MAX_JOURNAL_ENTRIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &journal->entries[journal->count];
    (void)memset(entry, 0, sizeof(*entry));
    entry->sequence = (uint64_t)journal->count + 1U;
    entry->kind = kind;
    entry->previous_hash = journal->count == 0U ? 0U
        : journal->entries[journal->count - 1U].entry_hash;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(entry->operation_id, sizeof(entry->operation_id),
                   operation_id) ||
        !copy_text(entry->summary, sizeof(entry->summary), summary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry->entry_hash = entry_hash(entry->sequence, entry->kind,
                                   entry->operation_id, entry->summary,
                                   entry->previous_hash);
    journal->count++;
    return UMI_STATUS_OK;
}

/*
 * Provide the helix journal verify operation used by this module and its client
 * applications.
 */
int umi_helix_journal_verify(const UmiHelixJournal *journal)
{
    size_t index;
    uint64_t previous = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (journal == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < journal->count; ++index) {
        const UmiHelixJournalEntry *entry = &journal->entries[index];
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (entry->sequence != (uint64_t)index + 1U ||
            entry->previous_hash != previous ||
            entry->entry_hash != entry_hash(entry->sequence, entry->kind,
                                            entry->operation_id,
                                            entry->summary, previous)) return 0;
        previous = entry->entry_hash;
    }
    return 1;
}

/* Find helix journal while leaving the underlying catalogue or model owned by this module. */
const UmiHelixJournalEntry *umi_helix_journal_at(
    const UmiHelixJournal *journal,
    size_t index)
{
    return journal != NULL && index < journal->count
        ? &journal->entries[index] : NULL;
}
