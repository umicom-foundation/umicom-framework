/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/journal_entry.h
 *
 * PURPOSE:
 *   Define the reusable event journal entry contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_JOURNAL_ENTRY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_JOURNAL_ENTRY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event journal entry data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventJournalEntry {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventJournalEntry;

/**
 * Initialise workbench context event journal entry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_journal_entry_init(
    UmiWorkbenchContextEventJournalEntry *record,
    const char *record_id);
/**
 * Check that workbench context event journal entry satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_journal_entry_validate(
    const UmiWorkbenchContextEventJournalEntry *record);
/**
 * Provide the workbench context event journal entry set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_journal_entry_set_source(
    UmiWorkbenchContextEventJournalEntry *record,
    const char *source_id);
/**
 * Provide the workbench context event journal entry set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_journal_entry_set_subject(
    UmiWorkbenchContextEventJournalEntry *record,
    const char *subject_id);
/**
 * Provide the workbench context event journal entry set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_journal_entry_set_group(
    UmiWorkbenchContextEventJournalEntry *record,
    const char *group_id);
/**
 * Provide the workbench context event journal entry set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_journal_entry_set_label(
    UmiWorkbenchContextEventJournalEntry *record,
    const char *label);
/**
 * Provide the workbench context event journal entry hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_journal_entry_hash(
    const UmiWorkbenchContextEventJournalEntry *record);
/**
 * Provide the workbench context event journal entry touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_journal_entry_touch(
    UmiWorkbenchContextEventJournalEntry *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
