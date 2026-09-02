/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/project_record.h
 *
 * PURPOSE:
 *   Define the reusable project event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PROJECT_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PROJECT_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event project record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventProjectRecord {
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
} UmiWorkbenchContextEventProjectRecord;

/**
 * Initialise workbench context event project record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_project_record_init(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *record_id);
/**
 * Check that workbench context event project record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_project_record_validate(
    const UmiWorkbenchContextEventProjectRecord *record);
/**
 * Provide the workbench context event project record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_project_record_set_source(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event project record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_project_record_set_subject(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event project record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_project_record_set_group(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event project record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_project_record_set_label(
    UmiWorkbenchContextEventProjectRecord *record,
    const char *label);
/**
 * Provide the workbench context event project record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_project_record_hash(
    const UmiWorkbenchContextEventProjectRecord *record);
/**
 * Provide the workbench context event project record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_project_record_touch(
    UmiWorkbenchContextEventProjectRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
