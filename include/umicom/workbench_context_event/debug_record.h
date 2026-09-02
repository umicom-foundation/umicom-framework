/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/debug_record.h
 *
 * PURPOSE:
 *   Define the reusable debug event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DEBUG_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DEBUG_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event debug record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventDebugRecord {
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
} UmiWorkbenchContextEventDebugRecord;

/**
 * Initialise workbench context event debug record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_debug_record_init(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *record_id);
/**
 * Check that workbench context event debug record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_debug_record_validate(
    const UmiWorkbenchContextEventDebugRecord *record);
/**
 * Provide the workbench context event debug record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_debug_record_set_source(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event debug record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_debug_record_set_subject(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event debug record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_debug_record_set_group(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event debug record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_debug_record_set_label(
    UmiWorkbenchContextEventDebugRecord *record,
    const char *label);
/**
 * Provide the workbench context event debug record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_debug_record_hash(
    const UmiWorkbenchContextEventDebugRecord *record);
/**
 * Provide the workbench context event debug record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_debug_record_touch(
    UmiWorkbenchContextEventDebugRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
