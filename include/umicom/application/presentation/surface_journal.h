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

/**
 * Represent the application presentation surface journal entry data shared with callers of
 * this public contract.
 */
typedef struct UmiApplicationPresentationSurfaceJournalEntry {
    uint64_t sequence;
    UmiApplicationPresentationSurfaceEvent event;
    const char *component_id;
    UmiStatus status;
} UmiApplicationPresentationSurfaceJournalEntry;

/**
 * Represent the application presentation surface journal data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationSurfaceJournal {
    UmiApplicationPresentationSurfaceJournalEntry
        entries[UMI_APPLICATION_PRESENTATION_JOURNAL_CAPACITY];
    size_t count;
    uint64_t next_sequence;
} UmiApplicationPresentationSurfaceJournal;

/**
 * Initialise application presentation surface journal from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_presentation_surface_journal_init(
    UmiApplicationPresentationSurfaceJournal *journal);
/**
 * Provide the application presentation surface journal record operation used by this
 * module and its client applications.
 */
void umi_application_presentation_surface_journal_record(
    UmiApplicationPresentationSurfaceJournal *journal,
    UmiApplicationPresentationSurfaceEvent event,
    const char *component_id,
    UmiStatus status);
/**
 * Find application presentation surface journal while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationPresentationSurfaceJournalEntry *
umi_application_presentation_surface_journal_at(
    const UmiApplicationPresentationSurfaceJournal *journal,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif
