/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/presentation/surface_journal.c
 *
 * PURPOSE:
 *   Maintain a bounded, ordered presentation event history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/surface_journal.h"

#include <string.h>

void umi_application_presentation_surface_journal_init(
    UmiApplicationPresentationSurfaceJournal *journal)
{
    if (journal != NULL) {
        (void)memset(journal, 0, sizeof(*journal));
        journal->next_sequence = 1U;
    }
}

void umi_application_presentation_surface_journal_record(
    UmiApplicationPresentationSurfaceJournal *journal,
    UmiApplicationPresentationSurfaceEvent event,
    const char *component_id,
    UmiStatus status)
{
    UmiApplicationPresentationSurfaceJournalEntry *entry;
    if (journal == NULL) return;
    if (journal->count == UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY) {
        (void)memmove(&journal->entries[0], &journal->entries[1],
                      sizeof(journal->entries[0]) *
                          (UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY - 1U));
        journal->count -= 1U;
    }
    entry = &journal->entries[journal->count++];
    entry->sequence = journal->next_sequence++;
    entry->event = event;
    entry->component_id = component_id;
    entry->status = status;
}

const UmiApplicationPresentationSurfaceJournalEntry *
umi_application_presentation_surface_journal_at(
    const UmiApplicationPresentationSurfaceJournal *journal,
    size_t index)
{
    return journal != NULL && index < journal->count
        ? &journal->entries[index]
        : NULL;
}
