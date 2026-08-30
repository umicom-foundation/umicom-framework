/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_journal.h
 *
 * PURPOSE:
 *   Retain a bounded audit trail of panel lifecycle and interaction outcomes
 *   so application behavior can be diagnosed without toolkit-specific logs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_JOURNAL_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_JOURNAL_H

#include "umicom/application/presentation/runtime_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationPresentationSurfaceJournalEntry {
    uint64_t sequence;
    UmiApplicationPresentationSurfaceEvent event;
    const char *component_id;
    UmiStatus status;
} UmiApplicationPresentationSurfaceJournalEntry;

typedef struct UmiApplicationPresentationSurfaceJournal {
    UmiApplicationPresentationSurfaceJournalEntry
        entries[UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY];
    size_t count;
    uint64_t next_sequence;
} UmiApplicationPresentationSurfaceJournal;

void umi_application_presentation_surface_journal_init(
    UmiApplicationPresentationSurfaceJournal *journal);
void umi_application_presentation_surface_journal_record(
    UmiApplicationPresentationSurfaceJournal *journal,
    UmiApplicationPresentationSurfaceEvent event,
    const char *component_id,
    UmiStatus status);
const UmiApplicationPresentationSurfaceJournalEntry *
umi_application_presentation_surface_journal_at(
    const UmiApplicationPresentationSurfaceJournal *journal,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
