/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/journal.c
 *
 * PURPOSE:
 *   Implement the bounded chronological developer-runtime event journal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/journal.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperJournal {
    UmiDeveloperJournalEntry *entries;
    size_t start;
    size_t count;
    uint64_t dropped_count;
    uint64_t next_sequence;
    uint64_t revision;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) {
        return;
    }
    if (source == NULL) {
        source = "";
    }

    length = strlen(source);
    if (length >= capacity) {
        length = capacity - 1U;
    }
    if (length > 0U) {
        memcpy(destination, source, length);
    }
    destination[length] = '\0';
}

UmiStatus umi_developer_journal_create(UmiDeveloperJournal **out_journal)
{
    UmiDeveloperJournal *journal;

    if (out_journal == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_journal = NULL;
    journal = (UmiDeveloperJournal *)calloc(1U, sizeof(*journal));
    if (journal == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    journal->entries = (UmiDeveloperJournalEntry *)calloc(
        UMI_DEVELOPER_JOURNAL_CAPACITY,
        sizeof(*journal->entries));
    if (journal->entries == NULL) {
        free(journal);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    journal->next_sequence = 1U;
    journal->revision = 1U;
    *out_journal = journal;
    return UMI_STATUS_OK;
}

void umi_developer_journal_destroy(UmiDeveloperJournal *journal)
{
    if (journal == NULL) {
        return;
    }
    free(journal->entries);
    free(journal);
}

UmiStatus umi_developer_journal_append(
    UmiDeveloperJournal *journal,
    UmiDeveloperEventKind kind,
    const char *operation_id,
    const char *message,
    UmiDeveloperJournalEntry *out_entry)
{
    UmiDeveloperJournalEntry entry;
    size_t destination_index;

    if (journal == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(&entry, 0, sizeof(entry));
    entry.struct_size = (uint32_t)sizeof(entry);
    entry.api_version = UMI_DEVELOPER_JOURNAL_API_VERSION;
    entry.sequence = journal->next_sequence++;
    entry.kind = kind;
    copy_text(entry.operation_id, sizeof(entry.operation_id), operation_id);
    copy_text(entry.message, sizeof(entry.message), message);

    if (journal->count < UMI_DEVELOPER_JOURNAL_CAPACITY) {
        destination_index =
            (journal->start + journal->count) % UMI_DEVELOPER_JOURNAL_CAPACITY;
        journal->count += 1U;
    } else {
        destination_index = journal->start;
        journal->start =
            (journal->start + 1U) % UMI_DEVELOPER_JOURNAL_CAPACITY;
        journal->dropped_count += 1U;
    }

    journal->entries[destination_index] = entry;
    journal->revision += 1U;

    if (out_entry != NULL) {
        *out_entry = entry;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_journal_at(
    const UmiDeveloperJournal *journal,
    size_t index,
    UmiDeveloperJournalEntry *out_entry)
{
    size_t physical_index;

    if (journal == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= journal->count) {
        return UMI_STATUS_NOT_FOUND;
    }

    physical_index =
        (journal->start + index) % UMI_DEVELOPER_JOURNAL_CAPACITY;
    *out_entry = journal->entries[physical_index];
    return UMI_STATUS_OK;
}

size_t umi_developer_journal_count(const UmiDeveloperJournal *journal)
{
    return journal != NULL ? journal->count : 0U;
}

UmiStatus umi_developer_journal_snapshot(
    const UmiDeveloperJournal *journal,
    UmiDeveloperJournalSnapshot *out_snapshot)
{
    if (journal == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEVELOPER_JOURNAL_API_VERSION;
    out_snapshot->entry_count = journal->count;
    out_snapshot->dropped_count = journal->dropped_count;
    out_snapshot->next_sequence = journal->next_sequence;
    out_snapshot->revision = journal->revision;
    return UMI_STATUS_OK;
}

void umi_developer_journal_clear(UmiDeveloperJournal *journal)
{
    if (journal == NULL) {
        return;
    }

    journal->start = 0U;
    journal->count = 0U;
    journal->dropped_count = 0U;
    journal->revision += 1U;
}
